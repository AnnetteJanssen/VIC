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

//        // Convert swq from m/(lake area) to m/(ice area)
//        if (lake_snow->swq > 0.0) {
//            if (fracprv > 0.0) {
//                lake_snow->swq /= fracprv;
//            }
//            else if (fracprv == 0.0) {
//                lake->ice_throughfall += (lake->sarea) * (lake_snow->swq);
//                lake_snow->swq = 0.0;
//            }
//        }
//
//        if (fracprv >= 1.0) { /* areai is relevant */
//            // If there is no snow, add the rain over ice directly to the lake.
//            if (lake_snow->swq <= 0.0 && rainfall > 0.0) {
//                lake->ice_throughfall += (rainfall / MM_PER_M) * lake->areai;
//                rainfall = 0.0;
//            }
//        }
//        else if (fracprv > param.LAKE_FRACLIM && fracprv < 1.0) { /* sarea is relevant */
//            /* Precip over open water directly increases lake volume. */
//            lake->ice_throughfall +=
//                ((snowfall / MM_PER_M + rainfall /
//                  MM_PER_M) * (1 - fracprv) * lake->sarea);
//
//            // If there is no snow, add the rain over ice directly to the lake.
//            if (lake_snow->swq <= 0.0 && rainfall > 0.0) {
//                lake->ice_throughfall +=
//                    (rainfall / MM_PER_M) * fracprv * lake->sarea;
//                rainfall = 0.0; /* Because do not want it added to snow->surf_water */
//            }
//        }
//        else {
//            lake->ice_throughfall +=
//                ((rainfall + snowfall) / MM_PER_M) * lake->sarea;
//            rainfall = 0.0;
//            snowfall = 0.0;
//        }

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

/******************************************************************************
 * @brief    This routine calculates the water balance of the lake.
 *****************************************************************************/
int
water_balance_tlake(lake_var_struct *lake,
              lake_con_struct  lake_con,
              double           dt,
              all_vars_struct *all_vars,
              int              iveg,
              int              band,
              double           lakefrac,
              soil_con_struct  soil_con,
              veg_con_struct   veg_con)
{
    extern option_struct       options;
    extern parameters_struct   param;
    extern global_param_struct global_param;

    int                        isave_n;
    double                     inflow_volume;
    double                     surfacearea, ldepth;
    double                     i_dbl;
    double                     index;
    size_t                     j, k, frost_area;
    double                     Tnew[MAX_LAKE_NODES];
    double                     circum;
    double                     baseflow_out_mm;
    double                     newfraction, Recharge;
    double                     abovegrnd_storage;
    int                        ErrorFlag;
    cell_data_struct         **cell;
    veg_var_struct           **veg_var;
    snow_data_struct         **snow;
    energy_bal_struct        **energy;
    size_t                     lindex;
    double                     frac;
    double                     Dsmax, resid_moist, liq, rel_moist;
    double                    *frost_fract;
    double                     volume_save;
    double                    *delta_moist = NULL;
    double                    *moist = NULL;
    double                     max_newfraction;

//    cell = all_vars->cell;
//    veg_var = all_vars->veg_var;
//    snow = all_vars->snow;
//    energy = all_vars->energy;
//
//    frost_fract = soil_con.frost_fract;
//
//    delta_moist = calloc(options.Nlayer, sizeof(*delta_moist));
//    check_alloc_status(delta_moist, "Memory allocation error.");
//    moist = calloc(options.Nlayer, sizeof(*moist));
//    check_alloc_status(moist, "Memory allocation error.");
//
//    /**********************************************************************
//    * 1. Preliminary stuff
//    **********************************************************************/
//
//    isave_n = lake->activenod; /* save initial no. of nodes for later */
//
//    inflow_volume = lake->runoff_in + lake->baseflow_in + lake->channel_in;
//
//    /**********************************************************************
//    * 2. calculate change in lake level for lake outflow calculation
//    *     grid cell runoff (m3/TS for non-lake area)
//    *     snow meltwater (mm)
//    *     open water evaporation (mm)
//    *     precip (added in solvelake)
//    **********************************************************************/
//
//    // Add runoff from rest of grid cell and wetland to lake, remove evaporation
//    // (precip was added in solve_lake, to allow for snow interception)
//    // Evaporation is not allowed to exceed the liquid volume of the lake (after incoming runoff & baseflow are added)
//    if (fabs(lake->evapw) > DBL_EPSILON && lake->evapw >
//        ((lake->volume -
//          lake->ice_water_eq) + lake->ice_throughfall + inflow_volume +
//         lake->snowmlt)) {
//        lake->evapw =
//            (lake->volume -
//             lake->ice_water_eq) + lake->ice_throughfall + inflow_volume +
//            lake->snowmlt;
//        lake->volume = lake->ice_water_eq;
//    }
//    else {
//        lake->volume +=
//            (lake->ice_throughfall + inflow_volume + lake->snowmlt -
//             lake->evapw);
//    }
//
//    // Estimate new surface area of liquid water for recharge calculations
//    volume_save = lake->volume;
//    ErrorFlag = get_depth(lake_con, lake->volume - lake->ice_water_eq, &ldepth);
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating depth: volume = %f, depth = %e",
//                lake->volume, ldepth);
//    }
//    ErrorFlag = get_sarea(lake_con, ldepth, &surfacearea);
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating area: depth = %f, sarea = %e",
//                ldepth, surfacearea);
//    }
//
//    // Estimate the new lake fraction (before recharge)
//    if (lake->new_ice_area > surfacearea) {
//        surfacearea = lake->new_ice_area;
//    }
//    newfraction = surfacearea / lake_con.basin[0];
//
//    // Save this estimate of the new lake fraction for use later
//    max_newfraction = newfraction;
//
//    /**********************************************************************
//    * 3. calculate recharge to wetland
//    **********************************************************************/
//
//    // Based on the above initial estimate of lake area, the lake
//    // will either inundate some of the wetland or will recede and
//    // expose new wetland.  In the case of inundation, we will
//    // take all above-ground moisture storage (snowpack, etc) and
//    // give it to the lake, but at the same time we will take water
//    // from the lake and fill the inundated soil to saturation.
//    // In the case of a receding lake, newly-exposed wetland is
//    // assumed to have saturated soil and 0 above-ground storage.
//
//    // The redistribution of moisture within the wetland will happen
//    // during the final step of this function.
//
//    // Note:
//    // This does not account for phase changes and temperature
//    // changes, so will generate some energy balance errors.
//    // In addition, the moisture exchange with the wetland
//    // will make our initial estimate of the new lake area
//    // somewhat inaccurate (the initial estimate will be an
//    // upper bound on the new lake area, resulting in some of
//    // the wetland being "splashed" by the lake, causing the
//    // snow to run off and the soil to moisten, but not being
//    // inundated).  However, lake dimensions will be recalculated
//    // after runoff and baseflow are subtracted from the lake.
//
//    lake->recharge = 0.0;
//    for (j = 0; j < options.Nlayer; j++) {
//        delta_moist[j] = 0; // mm over (1-lakefrac)
//    }
//
//    if (max_newfraction > lakefrac) {
//        // Lake must fill soil to saturation in the newly-flooded area
//        for (j = 0; j < options.Nlayer; j++) {
//            delta_moist[j] +=
//                (soil_con.max_moist[j] -
//                 cell[iveg][band].layer[j].moist) *
//                (max_newfraction - lakefrac) / (1 - lakefrac);                                                           // mm over (1-lakefrac)
//        }
//        for (j = 0; j < options.Nlayer; j++) {
//            lake->recharge += (delta_moist[j]) / MM_PER_M *
//                              (1 - lakefrac) * lake_con.basin[0];                    // m^3
//        }
//
//        // Above-ground storage in newly-flooded area is liberated and goes to lake
//        abovegrnd_storage =
//            (veg_var[iveg][band].Wdew / MM_PER_M +
//             snow[iveg][band].snow_canopy +
//             snow[iveg][band].swq) *
//            (max_newfraction - lakefrac) * lake_con.basin[0];
//        lake->recharge -= abovegrnd_storage;
//
//        // Fill the soil to saturation if possible in inundated area
//        // Subtract the recharge (which may be negative) from the lake
//        if (lake->volume - lake->ice_water_eq > lake->recharge) { // enough liquid water to support recharge
//            lake->volume -= lake->recharge;
//        }
//        else { // not enough liquid water to support recharge; fill soil as much as allowed by available liquid water in lake and above-ground storage in newly-flooded area; lake will recede back from this point after recharge is taken out of it
//            lake->recharge = lake->volume - lake->ice_water_eq;
//            lake->volume = lake->ice_water_eq;
//
//            Recharge = MM_PER_M * lake->recharge /
//                       ((max_newfraction -
//                         lakefrac) *
//                        lake_con.basin[0]) +
//                       (veg_var[iveg][band].Wdew +
//                        snow[iveg][band].snow_canopy * MM_PER_M +
//                        snow[iveg][band].swq * MM_PER_M);                                                                                                                               // mm over area that has been flooded
//
//            for (j = 0; j < options.Nlayer; j++) {
//                if (Recharge >
//                    (soil_con.max_moist[j] - cell[iveg][band].layer[j].moist)) {
//                    Recharge -=
//                        (soil_con.max_moist[j] -
//                         cell[iveg][band].layer[j].moist);
//                    delta_moist[j] =
//                        (soil_con.max_moist[j] -
//                         cell[iveg][band].layer[j].moist) *
//                        (max_newfraction - lakefrac) / (1 - lakefrac);                                                      // mm over (1-lakefrac)
//                }
//                else {
//                    delta_moist[j] = Recharge *
//                                     (max_newfraction -
//                                      lakefrac) / (1 - lakefrac);            // mm over (1-lakefrac)
//                    Recharge = 0.0;
//                }
//            }
//        }
//    }
//
//    /**********************************************************************
//    * 4. Calculate outflow from lake.  Runoff estimate is based on the
//    *    equation for flow over a broad crested weir.  Baseflow estimate
//    *    is the ARNO formulation, using the ice content of the adjacent
//    *    wetland.  Outgoing runoff and baseflow are in m3.
//    **********************************************************************/
//
//    Dsmax = soil_con.Dsmax / global_param.model_steps_per_day;
//    lindex = options.Nlayer - 1;
//    liq = 0;
//    for (frost_area = 0; frost_area < options.Nfrost; frost_area++) {
//        liq +=
//            (soil_con.max_moist[lindex] -
//             cell[iveg][band].layer[lindex].ice[frost_area]) *
//            frost_fract[frost_area];
//    }
//    resid_moist = soil_con.resid_moist[lindex] * soil_con.depth[lindex] *
//                  MM_PER_M;
//
//    /** Compute relative moisture **/
//    rel_moist =
//        (liq - resid_moist) / (soil_con.max_moist[lindex] - resid_moist);
//
//    /** Compute baseflow as function of relative moisture **/
//    frac = Dsmax * soil_con.Ds / soil_con.Ws;
//    baseflow_out_mm = frac * rel_moist;
//    if (rel_moist > soil_con.Ws) {
//        frac = (rel_moist - soil_con.Ws) / (1 - soil_con.Ws);
//        baseflow_out_mm += Dsmax * (1 - soil_con.Ds / soil_con.Ws) * pow(frac,
//                                                                         soil_con.c);
//    }
//    if (baseflow_out_mm < 0) {
//        baseflow_out_mm = 0;
//    }
//
//    // extract baseflow volume from the lake m^3
//    // baseflow will only come from under the liquid portion of the lake
//    ErrorFlag = get_depth(lake_con, lake->volume - lake->ice_water_eq, &ldepth);
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating depth: volume = %f, depth = %e",
//                lake->volume, ldepth);
//    }
//    ErrorFlag = get_sarea(lake_con, ldepth, &surfacearea);
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating area: depth = %f, sarea = %e",
//                ldepth, surfacearea);
//    }
//    lake->baseflow_out = baseflow_out_mm * surfacearea / MM_PER_M;
//    if (lake->volume - lake->ice_water_eq >= lake->baseflow_out) {
//        lake->volume -= lake->baseflow_out;
//    }
//    else {
//        lake->baseflow_out = lake->volume - lake->ice_water_eq;
//        lake->volume -= lake->baseflow_out;
//    }
//
//    // Find new lake depth for runoff calculations
//    ErrorFlag = get_depth(lake_con, lake->volume - lake->ice_water_eq, &ldepth);
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating depth: volume = %f, depth = %e",
//                lake->volume, ldepth);
//    }
//
//    // Compute runoff volume in m^3 and extract runoff volume from lake
//    if (ldepth <= lake_con.mindepth) {
//        lake->runoff_out = 0.0;
//    }
//    else {
//        circum = 2 * CONST_PI * pow(surfacearea / CONST_PI, 0.5);
//        lake->runoff_out = lake_con.wfrac * circum * dt *
//                           1.6 * pow(ldepth - lake_con.mindepth, 1.5);
//        if ((lake->volume - lake->ice_water_eq) >= lake->runoff_out) {
//            /*liquid water is available */
//            if ((lake->volume - lake->runoff_out) < lake_con.minvolume) {
//                lake->runoff_out = lake->volume - lake_con.minvolume;
//            }
//            lake->volume -= lake->runoff_out;
//        }
//        else {
//            lake->runoff_out = lake->volume - lake->ice_water_eq;
//            if ((lake->volume - lake->runoff_out) < lake_con.minvolume) {
//                lake->runoff_out = lake->volume - lake_con.minvolume;
//            }
//            lake->volume -= lake->runoff_out;
//        }
//    }
//
//    // Check that lake volume does not exceed our earlier estimate.
//    // This will prevent runaway lake growth for the case in which
//    // the lake recharge to wetland is negative and large enough to
//    // more than compensate for runoff and baseflow out of the lake.
//    if (lake->volume > volume_save) {
//        lake->runoff_out += lake->volume - volume_save;
//        lake->volume = volume_save;
//    }
//
//    // check that lake volume does not exceed its maximum
//    if (lake->volume - lake_con.maxvolume > DBL_EPSILON) {
//        if (lake->ice_water_eq > lake_con.maxvolume) {
//            lake->runoff_out += (lake->volume - lake->ice_water_eq);
//            lake->volume = lake->ice_water_eq;
//        }
//        else {
//            lake->runoff_out += (lake->volume - lake_con.maxvolume);
//            lake->volume = lake_con.maxvolume;
//        }
//    }
//    else if (lake->volume < DBL_EPSILON) {
//        lake->volume = 0.0;
//    }
//
//    /**********************************************************************/
//    /* End of runoff calculation */
//    /**********************************************************************/
//
//    // Recalculate lake depth to define surface[] for next time step
//    // Here, we only want depth of liquid water (below ice bottom), since surface[] array only applies to liquid water
//    ErrorFlag =
//        get_depth(lake_con, lake->volume - lake->ice_water_eq, &(lake->ldepth));
//    if (ErrorFlag == ERROR) {
//        log_err("Error calculating depth: volume = %f, depth = %e",
//                lake->volume, lake->ldepth);
//    }
//
//    /**********************************************************************
//    *  4. Adjust the activenodes and lake area array.
//    **********************************************************************/
//
//    compute_derived_lake_dimensions(lake, lake_con);
//
//    // Final lakefraction
//    if (lake->new_ice_area > lake->surface[0]) {
//        lake->sarea = lake->new_ice_area;
//    }
//    else {
//        lake->sarea = lake->surface[0];
//    }
//    newfraction = lake->sarea / lake_con.basin[0];
//
//    /*******************************************************************/
//
//    /* Adjust temperature distribution if number of nodes has changed.
//       Note:  This approach (and the lake model in general) does not preserve
//       the thermal energy of the water column. */
//
//    index = 0.;
//    if (lake->activenod != isave_n) {
//        for (k = 0; k < lake->activenod; k++) {
//            Tnew[k] = 0.0;
//            for (i_dbl = 0; i_dbl < isave_n; i_dbl++) {
//                index += (1. / lake->activenod);
//                Tnew[k] += lake->temp[(int) floor(index)];
//            }
//        }
//        for (k = 0; k < lake->activenod; k++) {
//            if (isave_n > 0) {
//                lake->temp[k] = Tnew[k] / isave_n;
//            }
//            else {
//                lake->temp[k] = all_vars->energy[iveg][band].Tsurf;
//            }
//        }
//    }
//
//    if (lake->activenod == isave_n && isave_n == 0) {
//        lake->temp[k] = all_vars->energy[iveg][band].Tsurf;
//    }
//
//    /**********************************************************************
//        5. Rescale the fluxes in the lake and the wetland by the change in lake area;
//           Advect the storages.
//           Final units of storages and fluxes should be in mm/final lake or wetland area.
//    **********************************************************************/
//    // Wetland
//    if (newfraction < 1.0) { // wetland exists at end of time step
//        advect_soil_veg_storage(lakefrac, max_newfraction, newfraction,
//                                delta_moist, &soil_con, &veg_con,
//                                &(cell[iveg][band]), &(veg_var[iveg][band]),
//                                lake_con);
//        rescale_soil_veg_fluxes((1 - lakefrac), (1 - newfraction),
//                                &(cell[iveg][band]), &(veg_var[iveg][band]));
//        advect_snow_storage(lakefrac, max_newfraction, newfraction,
//                            &(snow[iveg][band]));
//        rescale_snow_energy_fluxes((1 - lakefrac), (1 - newfraction),
//                                   &(snow[iveg][band]), &(energy[iveg][band]));
//        for (j = 0; j < options.Nlayer; j++) {
//            moist[j] = cell[iveg][band].layer[j].moist;
//        }
//        ErrorFlag = distribute_node_moisture_properties(
//            energy[iveg][band].moist, energy[iveg][band].ice,
//            energy[iveg][band].kappa_node,
//            energy[iveg][band].Cs_node,
//            soil_con.Zsum_node,
//            energy[iveg][band].T,
//            soil_con.max_moist_node,
//            soil_con.expt_node,
//            soil_con.bubble_node,
//            moist, soil_con.depth,
//            soil_con.soil_dens_min,
//            soil_con.bulk_dens_min,
//            soil_con.quartz,
//            soil_con.soil_density,
//            soil_con.bulk_density,
//            soil_con.organic, options.Nnode,
//            options.Nlayer,
//            soil_con.FS_ACTIVE);
//        if (ErrorFlag == ERROR) {
//            return (ERROR);
//        }
//    }
//    else if (lakefrac < 1.0) { // wetland is gone at end of time step, but existed at beginning of step
//        if (lakefrac > 0.0) { // lake also existed at beginning of step
//            for (j = 0; j < options.Nlayer; j++) {
//                lake->evapw += cell[iveg][band].layer[j].evap / MM_PER_M *
//                               (1. - lakefrac) * lake_con.basin[0];
//            }
//            lake->evapw += veg_var[iveg][band].canopyevap / MM_PER_M *
//                           (1. - lakefrac) * lake_con.basin[0];
//            lake->evapw += snow[iveg][band].canopy_vapor_flux *
//                           (1. - lakefrac) * lake_con.basin[0];
//            lake->evapw += snow[iveg][band].vapor_flux *
//                           (1. - lakefrac) * lake_con.basin[0];
//        }
//    }
//
//    // Lake
//    if (newfraction > 0.0) { // lake exists at end of time step
//        // Copy moisture fluxes into lake->soil structure, mm over end-of-step lake area
//        lake->soil.runoff = lake->runoff_out * MM_PER_M /
//                            (newfraction * lake_con.basin[0]);
//        lake->soil.baseflow = lake->baseflow_out * MM_PER_M /
//                              (newfraction * lake_con.basin[0]);
//        lake->soil.inflow = lake->baseflow_out * MM_PER_M /
//                            (newfraction * lake_con.basin[0]);
//        for (lindex = 0; lindex < options.Nlayer; lindex++) {
//            lake->soil.layer[lindex].evap = 0;
//        }
//        lake->soil.layer[0].evap += lake->evapw * MM_PER_M /
//                                    (newfraction * lake_con.basin[0]);
//        // Rescale other fluxes and storages to mm over end-of-step lake area
//        if (lakefrac > 0.0) { // lake existed at beginning of time step
//            rescale_snow_storage(lakefrac, newfraction, &(lake->snow));
//            rescale_snow_energy_fluxes(lakefrac, newfraction, &(lake->snow),
//                                       &(lake->energy));
//            if (lake->snow.swq > 0) {
//                lake->snow.coverage = lake->new_ice_area / lake->sarea;
//            }
//            else {
//                lake->snow.coverage = 0;
//            }
//        }
//        else { // lake didn't exist at beginning of time step; create new lake
//               // Reset all non-essential lake variables
//            initialize_lake(lake, lake_con, &soil_con, &(cell[iveg][band]),
//                            true);
//
//            // Compute lake dimensions from current lake depth
//            compute_derived_lake_dimensions(lake, lake_con);
//
//            // Assign initial temperatures
//            for (k = 0; k < lake->activenod; k++) {
//                lake->temp[k] = all_vars->energy[iveg][band].T[0];
//            }
//            lake->tempavg = lake->temp[0];
//            lake->energy.Tsurf = all_vars->energy[iveg][band].Tsurf;
//            for (k = 0; k < options.Nnode; k++) {
//                lake->energy.T[k] = all_vars->energy[iveg][band].T[k];
//            }
//            for (k = 0; k < options.Nlayer; k++) {
//                lake->soil.layer[k].T = all_vars->cell[iveg][band].layer[k].T;
//            }
//        }
//    }
//    else if (lakefrac > 0.0) { // lake is gone at end of time step, but existed at beginning of step
//        if (lakefrac < 1.0) { // wetland also existed at beginning of step
//            cell[iveg][band].layer[0].evap += MM_PER_M * lake->evapw /
//                                              ((1. -
//                                                newfraction) *
//                                               lake_con.basin[0]);
//            cell[iveg][band].runoff += MM_PER_M * lake->runoff_out /
//                                       ((1. - newfraction) * lake_con.basin[0]);
//            cell[iveg][band].baseflow += MM_PER_M * lake->baseflow_out /
//                                         ((1. -
//                                           newfraction) * lake_con.basin[0]);
//            cell[iveg][band].inflow += MM_PER_M * lake->baseflow_out /
//                                       ((1. - newfraction) * lake_con.basin[0]);
//        }
//    }
//
//    if (options.CARBON) {
//        advect_carbon_storage(lakefrac, newfraction, lake, &(cell[iveg][band]));
//    }

    free((char*) delta_moist);
    free((char*) moist);

    return(0);
}
