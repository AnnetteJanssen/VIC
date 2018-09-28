/******************************************************************************
 * @section DESCRIPTION
 *
 * This set of routines handel the energy balance of lakes
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_run.h>

/******************************************************************************
 * @brief    This subroutine solves the energy budget for open water bodies.
 *****************************************************************************/
int
solve_lake_tlake(double           snowfall,
           double           rainfall,
           double           tair,
           double           wind,
           double           vp,
           double           shortin,
           double           longin,
           double           vpd,
           double           pressure,
           double           air_density,
           lake_var_struct *lake,
           soil_con_struct  soil_con,
           double           dt,
           double           wind_h,
           dmy_struct       dmy,
           double           fracprv)
{
    extern parameters_struct param;

    double                   LWnetw, LWneti;
    double                   sw_water, sw_ice;
    double                   T[MAX_LAKE_NODES]; /* temp of the water column, open fraction. */
    double                   Ti[MAX_LAKE_NODES]; /* temp of the water column, ice fraction. */
    double                   water_density[MAX_LAKE_NODES],
                             water_cp[MAX_LAKE_NODES];
    double                   albi, albw;
    double                   tempalbs;
    double                   Tcutoff; /* Lake freezing temperature (K). */
    double                   Qhw, Qhi;
    double                   Qew, Qei;
    double                   qw;
    int                      freezeflag;
    int                      mixdepth;
    double                   new_ice_area; /* Ice area formed by freezing in the open water portion. */
    int                      k, i, ErrorFlag;
    double                   Le;
    double                   sumjoulb;
    double                   temphi;
    double                   new_ice_height;
    double                   windw, windi;
    double                   energy_ice_formation;
    double                   energy_ice_melt_bot;
    double                   Qnet_ice;
    double                   energy_out_bottom;
    double                   energy_out_bottom_ice;
    double                   qf;
    double                   inputs, outputs, internal, phasechange;
    double                   new_ice_water_eq;
    double                   temp_refreeze_energy;
    energy_bal_struct       *lake_energy;
    snow_data_struct        *lake_snow;

    /**********************************************************************
    * 1. Initialize variables.
    **********************************************************************/

    lake->sarea_save = lake->sarea;
    lake->volume_save = lake->volume;
    lake->swe_save = lake->swe;

    lake_energy = &(lake->energy);
    lake_snow = &(lake->snow);

    lake_energy->advection = 0.0;
    lake_energy->deltaCC = 0.0;
    lake_energy->grnd_flux = 0.0;
    lake_energy->snow_flux = 0.0;
    lake->snowmlt = 0.0;
    qw = 0.0;
    new_ice_height = new_ice_area = new_ice_water_eq = 0.0;
    lake->evapw = 0.0;
    energy_ice_formation = 0.0;
    energy_out_bottom = energy_out_bottom_ice = 0.0;
    energy_ice_melt_bot = 0.0;
    lake_snow->vapor_flux = 0.0;
    lake->vapor_flux = 0.0;
    lake_energy->Tsurf = lake->temp[0];
    temp_refreeze_energy = 0.0;
    lake->ice_throughfall = 0.0;

    if (lake->activenod > 0 || lake->areai > 0.0) {
        /* --------------------------------------------------------------------
         * Calculate the water freezing point.
         * ------------------------------------------------------------------- */

        rhoinit(&Tcutoff, pressure);      /* degrees C */

        /* --------------------------------------------------------------------
         * Initialize liquid water and ice temperature profiles to be used
         * later in the module.
         * -------------------------------------------------------------------- */

        for (k = 0; k < lake->activenod; k++) {
            T[k] = lake->temp[k];
            Ti[k] = lake->temp[k];
            water_density[k] = calc_density(T[k]);
            water_cp[k] = specheat(T[k]);
        }

        energycalc(lake->temp, &sumjoulb, lake->activenod, lake->dz,
                   lake->surfdz, lake->surface, water_cp, water_density);

        /**********************************************************************
        * 2. Calculate added precipitation and total snow height.
        **********************************************************************/

        // Convert swq from m/(lake area) to m/(ice area)
        if (lake_snow->swq > 0.0) {
            if (fracprv > 0.0) {
                lake_snow->swq /= fracprv;
            }
            else if (fracprv == 0.0) {
                lake->ice_throughfall += (lake->sarea) * (lake_snow->swq);
                lake_snow->swq = 0.0;
            }
        }

        if (fracprv >= 1.0) { /* areai is relevant */
            // If there is no snow, add the rain over ice directly to the lake.
            if (lake_snow->swq <= 0.0 && rainfall > 0.0) {
                lake->ice_throughfall += (rainfall / MM_PER_M) * lake->areai;
                rainfall = 0.0;
            }
        }
        else if (fracprv > param.LAKE_FRACLIM && fracprv < 1.0) { /* sarea is relevant */
            /* Precip over open water directly increases lake volume. */
            lake->ice_throughfall +=
                ((snowfall / MM_PER_M + rainfall /
                  MM_PER_M) * (1 - fracprv) * lake->sarea);

            // If there is no snow, add the rain over ice directly to the lake.
            if (lake_snow->swq <= 0.0 && rainfall > 0.0) {
                lake->ice_throughfall +=
                    (rainfall / MM_PER_M) * fracprv * lake->sarea;
                rainfall = 0.0; /* Because do not want it added to snow->surf_water */
            }
        }
        else {
            lake->ice_throughfall +=
                ((rainfall + snowfall) / MM_PER_M) * lake->sarea;
            rainfall = 0.0;
            snowfall = 0.0;
        }

        /**********************************************************************
        * 3. Calculate incoming solar radiation over water and ice.
        **********************************************************************/

        /* --------------------------------------------------------------------
         * Calculate the albedo of the lake for ice, snow and liquid water.
         * To be consistent with VIC snow model, tempalbs = NEW_SNOW_ALB if snow
         * is falling, but aging routing does not get reset.
         * -------------------------------------------------------------------- */

        alblake(Tcutoff, tair, &lake->SAlbedo, &tempalbs, &albi, &albw,
                snowfall, lake_snow->coldcontent, dt, &lake_snow->last_snow,
                lake_snow->swq, &lake_snow->MELTING, dmy.day_in_year,
                soil_con.lat);

        /* --------------------------------------------------------------------
         * Calculate the incoming solar radiaton for both the ice fraction
         * and the liquid water fraction of the lake.
         * -------------------------------------------------------------------- */

        if (lake_snow->swq > param.LAKE_SNOWCRIT * param.LAKE_RHOSNOW /
            CONST_RHOFW) {
            sw_ice = shortin * (1. - tempalbs);
            lake_energy->AlbedoLake = (fracprv) * tempalbs +
                                      (1. - fracprv) * albw;
        }
        else if (lake_snow->swq > 0. && lake_snow->swq <= param.LAKE_SNOWCRIT *
                 param.LAKE_RHOSNOW /
                 CONST_RHOFW) {
            sw_ice = shortin * (1. - (albi + tempalbs) / 2.);
            lake_energy->AlbedoLake = (fracprv) *
                                      (albi +
                                       tempalbs) / 2. + (1. - fracprv) * albw;
        }
        else if (fracprv > 0. && lake_snow->swq <= 0.) {
            sw_ice = shortin * (1. - albi);
            lake_energy->AlbedoLake = (fracprv) * albi + (1. - fracprv) * albw;
        }
        else {
            sw_ice = 0.0;
            lake_energy->AlbedoLake = albw;
        }
        lake_energy->AlbedoUnder = lake_energy->AlbedoLake;

        sw_water = shortin * (1. - albw);

        /**********************************************************************
        * 4. Calculate initial energy balance over ice-free water.
        **********************************************************************/

        if ((1. - fracprv) > DBL_EPSILON && lake->activenod > 0) {
            freezeflag = 1; /* Calculation for water, not ice. */
            windw = wind *
                    log((2. + param.LAKE_ZWATER) / param.LAKE_ZWATER) / log(
                wind_h / param.LAKE_ZWATER);

            ErrorFlag = water_energy_balance(lake->activenod, lake->surface,
                                             &lake->evapw,
                                             dt, lake->dz,
                                             lake->surfdz,
                                             soil_con.lat, Tcutoff,
                                             tair, windw,
                                             pressure, vp, air_density, longin,
                                             sw_water, wind_h, &Qhw, &Qew,
                                             &LWnetw, T,
                                             water_density,
                                             &lake_energy->deltaH,
                                             &energy_ice_formation, fracprv,
                                             &new_ice_area, water_cp,
                                             &new_ice_height,
                                             &energy_out_bottom,
                                             &new_ice_water_eq,
                                             lake->volume - lake->ice_water_eq);
            if (ErrorFlag == ERROR) {
                return (ERROR);
            }

            /* --------------------------------------------------------------------
             * Do the convective mixing of the lake water.
             * -------------------------------------------------------------------- */

            mixdepth = 0;  /* Set to zero for this time step. */
            tracer_mixer(T, &mixdepth, lake->surface,
                         lake->activenod, lake->dz, lake->surfdz, water_cp);

            lake_energy->AtmosLatent = (1. - fracprv) * Qew;
            lake_energy->AtmosSensible = (1. - fracprv) * Qhw;
            lake_energy->NetLongAtmos = (1. - fracprv) * LWnetw;
            lake_energy->NetShortAtmos = (1. - fracprv) * sw_water;
            lake_energy->refreeze_energy = energy_ice_formation *
                                           (1. - fracprv);
            lake_energy->deltaH *= (1. - fracprv);
            lake_energy->grnd_flux = -1. * (energy_out_bottom * (1. - fracprv));
            lake_energy->Tsurf = (1. - fracprv) * T[0];
        }      /* End of water fraction calculations. */
        else {
            // ice covers 100% of lake, reset open water fluxes
            mixdepth = 0;
            LWnetw = 0;
            Qew = 0;
            Qhw = 0;

            lake_energy->AtmosLatent = 0.0;
            lake_energy->AtmosSensible = 0.0;
            lake_energy->NetLongAtmos = 0.0;
            lake_energy->NetShortAtmos = 0.0;
            lake_energy->refreeze_energy = 0.0;
            lake_energy->deltaH = 0.0;
            lake_energy->grnd_flux = 0.0;
            lake_energy->Tsurf = 0.0;
        }

        /**********************************************************************
        *  6. Calculate initial energy balance over ice.
        **********************************************************************/

        windi = (wind * log((2. + soil_con.snow_rough) / soil_con.snow_rough) /
                 log(wind_h / soil_con.snow_rough));
        if (windi < 1.0) {
            windi = 1.0;
        }

        if (fracprv >= param.LAKE_FRACLIM) {
            freezeflag = 0;   /* Calculation for ice. */
            Le = calc_latent_heat_of_sublimation(tair); /* ice*/

            lake->aero_resist =
                (log((2. + soil_con.snow_rough) / soil_con.snow_rough) *
                 log(wind_h /
                     soil_con.snow_rough) /
                 (CONST_KARMAN * CONST_KARMAN)) / windi;

            /* Calculate snow/ice temperature and change in ice thickness from
               surface melting. */
            ErrorFlag = ice_melt(wind_h + soil_con.snow_rough,
                                 lake->aero_resist, &(lake->aero_resist),
                                 Le, lake_snow, lake, dt, 0.0,
                                 soil_con.snow_rough, 1.0,
                                 rainfall, snowfall, windi, Tcutoff, tair,
                                 sw_ice,
                                 longin, air_density, pressure, vpd, vp,
                                 &lake->snowmlt,
                                 &lake_energy->advection, &lake_energy->deltaCC,
                                 &lake_energy->snow_flux, &Qei, &Qhi, &Qnet_ice,
                                 &temp_refreeze_energy, &LWneti);
            if (ErrorFlag == ERROR) {
                return (ERROR);
            }

            lake_energy->refreeze_energy += temp_refreeze_energy * fracprv;
            lake->tempi = lake_snow->surf_temp;

            /**********************************************************************
            *  7. Adjust temperatures of water column in ice fraction.
            **********************************************************************/

            /* --------------------------------------------------------------------
             * Calculate inputs to temp_area..
             * -------------------------------------------------------------------- */

            if (lake->activenod > 0) {
                ErrorFlag = water_under_ice(freezeflag, sw_ice, wind, Ti,
                                            water_density,
                                            soil_con.lat,
                                            lake->activenod, lake->dz,
                                            lake->surfdz,
                                            Tcutoff, &qw, lake->surface,
                                            &temphi, water_cp,
                                            mixdepth, lake->hice,
                                            lake_snow->swq * CONST_RHOFW / param.LAKE_RHOSNOW,
                                            dt, &energy_out_bottom_ice);
                if (ErrorFlag == ERROR) {
                    return (ERROR);
                }
            }
            else {
                temphi = -sumjoulb;
            }

            /**********************************************************************
            *   8.  Calculate change in ice thickness and fraction
            *    within fraction that already has ice.
            **********************************************************************/
            /* Check to see if ice has already melted (from the top) in this time step. */
            if (lake->ice_water_eq > 0.0) {
                ErrorFlag = lakeice(sw_ice,
                                    fracprv, dt, lake_energy->snow_flux, qw,
                                    &energy_ice_melt_bot,
                                    lake_energy->deltaCC, &qf,
                                    &lake->ice_water_eq,
                                    lake->volume - new_ice_water_eq,
                                    lake->surface[0]);
                if (ErrorFlag == ERROR) {
                    return (ERROR);
                }
            }

            lake_energy->AtmosLatent += fracprv * Qei;
            lake_energy->advection *= fracprv;
            lake_energy->AtmosSensible += fracprv * Qhi;
            lake_energy->NetLongAtmos += fracprv * LWneti;
            lake_energy->NetShortAtmos += fracprv * sw_ice;
            lake_energy->deltaH += fracprv * temphi;
            lake_energy->grnd_flux += -1. * (energy_out_bottom_ice * fracprv);
            lake_energy->refreeze_energy += energy_ice_melt_bot * fracprv;
            lake_energy->Tsurf += fracprv * lake_snow->surf_temp;
        }
        else {
            /* No Lake Ice Fraction */
            LWneti = 0;
            Qei = 0.;
            Qhi = 0;
            qf = 0.0;
            temphi = 0.0;
            lake_energy->refreeze_energy = 0.0;
            if (fracprv > 0.0) {
                energy_ice_melt_bot =
                    (lake->hice * CONST_RHOICE +
                     (snowfall /
                      MM_PER_M) *
                     CONST_RHOFW) * CONST_LATICE / dt;
                lake->areai = 0.0;
                lake->hice = 0.0;
                lake->ice_water_eq = 0.0;
            }
            else {
                energy_ice_melt_bot = 0.0;
                lake->areai = 0.0;
                lake->hice = 0.0;
                lake->ice_water_eq = 0.0;
            }
            lake->aero_resist =
                (log((2. + param.LAKE_ZWATER) / param.LAKE_ZWATER) *
                 log(wind_h / param.LAKE_ZWATER) /
                 (CONST_KARMAN * CONST_KARMAN)) / windi;
        }
        lake->soil.aero_resist[0] = lake->aero_resist;

        /**********************************************************************
        * 9. Average water temperature.
        **********************************************************************/

        if (lake->activenod > 0) {
            // Average ice-covered and non-ice water columns.
            colavg(lake->temp, T, Ti, fracprv, lake->density, lake->activenod,
                   lake->dz, lake->surfdz);

            // Calculate depth average temperature of the lake
            lake->tempavg = 0.0;
            for (i = 0; i < lake->activenod; i++) {
                lake->tempavg += lake->temp[i] / lake->activenod;
            }
        }
        else {
            lake->tempavg = -99;
        }

        /**********************************************************************
        * 10. Calculate the final water heat content and energy balance.
        **********************************************************************/

        /* Incoming energy. */
        inputs = (sw_ice + LWneti + lake_energy->advection + Qhi + Qei);
        outputs = energy_out_bottom_ice;
        internal = temphi;
        phasechange = -1 * (lake_energy->refreeze_energy) - 1. *
                      energy_ice_melt_bot;

        lake_energy->error = inputs - outputs - internal - phasechange;

        lake_energy->snow_flux = 0.0;

        // Sign convention
        lake_energy->deltaH *= -1;

        lake_energy->error = (lake_energy->NetShortAtmos +
                              lake_energy->NetLongAtmos +
                              lake_energy->AtmosSensible +
                              lake_energy->AtmosLatent +
                              lake_energy->deltaH +
                              lake_energy->grnd_flux +
                              lake_energy->refreeze_energy +
                              lake_energy->advection);

        temphi = 0.0;

        /**********************************************************************
        * 11. Final accounting for passing variables back to VIC.
        **********************************************************************/

        // Adjust lake_snow variables to represent storage and flux over entire lake
        lake_snow->swq *= fracprv;
        lake_snow->surf_water *= fracprv;
        lake_snow->pack_water *= fracprv;
        lake_snow->depth = lake_snow->swq * CONST_RHOFW / param.LAKE_RHOSNOW;
        lake_snow->coldcontent *= fracprv;
        lake_snow->vapor_flux *= fracprv;
        lake_snow->blowing_flux *= fracprv;
        lake_snow->surface_flux *= fracprv;
        lake_snow->melt = lake->snowmlt * fracprv;    // in mm

        /* Lake data structure terms */
        lake->evapw *=
            ((1. - fracprv) * dt) / MM_PER_M * lake->sarea;            // in m3
        lake->vapor_flux = lake->snow.vapor_flux * lake->sarea; // in m3
        lake->swe = lake_snow->swq * lake->sarea; // in m3
        lake->snowmlt *= fracprv / MM_PER_M * lake->sarea; // in m3
        lake->pack_water = lake_snow->pack_water * lake->sarea; // in m3
        lake->surf_water = lake_snow->surf_water * lake->sarea; // in m3
        lake->sdepth = lake_snow->depth * lake->sarea;
        lake->pack_temp = lake_snow->pack_temp;
        lake->surf_temp = lake_snow->surf_temp;

        /* Update ice area to include new ice growth in water fraction. */
        lake->new_ice_area = lake->areai;
        if (new_ice_area > 0.0) {
            lake->new_ice_area += new_ice_area;
            lake->ice_water_eq += new_ice_water_eq;
        }
        if (lake->ice_water_eq > 0.0 && lake->new_ice_area > 0.0) {
            lake->hice =
                (lake->ice_water_eq /
                 lake->new_ice_area) * CONST_RHOFW / CONST_RHOICE;
        }
        else {
            lake->hice = 0.0;
        }

        /* Change area of ice-covered fraction if ice has thinned. */
        if (lake->hice <= 0.0) {
            lake->new_ice_area = 0.0;
            lake->hice = 0.0;
        }
        else if (lake->hice < param.LAKE_FRACMIN) {
            lake->new_ice_area =
                (lake->new_ice_area * lake->hice) / param.LAKE_FRACMIN;
            lake->hice = param.LAKE_FRACMIN;
        }

        if (lake->snow.swq > 0) {
            lake->snow.coverage = lake->new_ice_area / lake->sarea;
        }
        else {
            lake->snow.coverage = 0;
        }
    } /* End of if activenods > 0 */


    return (0);
}   /* End of solve_lake function. */
