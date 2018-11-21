/******************************************************************************
* @section DESCRIPTION
*
* Calculate infiltration and runoff from the surface, gravity driven drainage
* between all soil layers, and generates baseflow from the bottom layer.
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
******************************************************************************/

#include <vic_run.h>
#include <groundwater.h>

/******************************************************************************
* @brief    Calculate infiltration and runoff from the surface, gravity driven
*           drainage between all soil layers, and generates baseflow from the
*           bottom layer.
******************************************************************************/
int
runoff_gw(cell_data_struct  *cell,
          energy_bal_struct *energy,
          gw_var_struct     *gw_var,
          soil_con_struct   *soil_con,
          gw_con_struct     *gw_con,
          double             ppt,
          double            *frost_fract,
          int                Nnodes)
{
    extern option_struct       options;
    extern parameters_struct   param;
    extern global_param_struct global_param;

    size_t                     lindex;
    size_t                     time_step;
    int                        last_index;
    int                        tmplayer;
    int                        fidx;
    int                        ErrorFlag;
    double                     A;
    double                     tmp_runoff;
    double                     inflow;
    double                     resid_moist[MAX_LAYERS]; // residual moisture (mm)
    double                     org_moist[MAX_LAYERS]; // total soil moisture (liquid and frozen) at beginning of this function (mm)
    double                     avail_liq[MAX_LAYERS][MAX_FROST_AREAS]; // liquid soil moisture available for evap/drainage (mm)
    double                     liq[MAX_LAYERS]; // current liquid soil moisture (mm)
    double                     ice[MAX_LAYERS]; // current frozen soil moisture (mm)
    double                     moist[MAX_LAYERS]; // current total soil moisture (liquid and frozen) (mm)
    double                     max_moist[MAX_LAYERS]; // maximum storable moisture (liquid and frozen) (mm)
    double                     Ksat[MAX_LAYERS];
    double                     Q12[MAX_LAYERS];
    double                     tmp_inflow;
    double                     tmp_moist;
    double                     tmp_moist_for_runoff[MAX_LAYERS];
    double                     tmp_liq;
    double                     dt_inflow;
    double                     dt_runoff;
    double                     runoff[MAX_FROST_AREAS];
    double                     tmp_dt_runoff[MAX_FROST_AREAS];
    double                     baseflow[MAX_FROST_AREAS];
    double                     dt_baseflow;
    double                     evap[MAX_LAYERS][MAX_FROST_AREAS];
    double                     sum_liq;
    double                     evap_fraction;
    double                     evap_sum;
    layer_data_struct         *layer;
    layer_data_struct          tmp_layer;
    unsigned short             runoff_steps_per_dt;

    double                     matric[MAX_LAYERS];
    double                     matric_expt[MAX_LAYERS];
    double                     avg_matric;

    // fluxes
    double                     dt_recharge;
    double                     dt_evaporation;

    // temp variables
    double                     avg_K;
    double                     tmp_z;
    double                     tmp_Wt;

    // states
    int                        lwt;
    double                     Dsmax;
    double                     delta_z;

    double                     Qb[MAX_LAYERS];
    double                     z[MAX_LAYERS];
    double                     eff_porosity[MAX_LAYERS];
    double                     neff_moist;

    // Output
    double                     recharge[MAX_FROST_AREAS];
    double                     zwt[MAX_FROST_AREAS];
    double                     Wa[MAX_FROST_AREAS];
    double                     Wt[MAX_FROST_AREAS];

    /** Set Residual Moisture **/
    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        resid_moist[lindex] = soil_con->resid_moist[lindex] *
                              soil_con->depth[lindex] * MM_PER_M;
    }

    /** Allocate and Set Values for Soil Sublayers **/
    layer = cell->layer;

    cell->runoff = 0;
    cell->baseflow = 0;
    cell->asat = 0;

    runoff_steps_per_dt = global_param.runoff_steps_per_day /
                          global_param.model_steps_per_day;

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        recharge[fidx] = 0;
        baseflow[fidx] = 0;

        zwt[fidx] = gw_var->zwt;
        Wa[fidx] = gw_var->Wa;
        Wt[fidx] = gw_var->Wt;
    }

    gw_var->recharge = 0.0;
    gw_var->zwt = 0.0;
    gw_var->Wa = 0.0;
    gw_var->Wt = 0.0;

    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        evap[lindex][0] = layer[lindex].evap / (double) runoff_steps_per_dt;
        org_moist[lindex] = layer[lindex].moist;
        layer[lindex].moist = 0;
        layer[lindex].eff_saturation = 0;
        if (evap[lindex][0] > 0) { // if there is positive evaporation
            sum_liq = 0;
            // compute available soil moisture for each frost sub area.
            for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
                avail_liq[lindex][fidx] =
                    (org_moist[lindex] - layer[lindex].ice[fidx] -
                     resid_moist[lindex]);
                if (avail_liq[lindex][fidx] < 0) {
                    avail_liq[lindex][fidx] = 0;
                }
                sum_liq += avail_liq[lindex][fidx] *
                           frost_fract[fidx];
            }
            // compute fraction of available soil moisture that is evaporated
            if (sum_liq > 0) {
                evap_fraction = evap[lindex][0] / sum_liq;
            }
            else {
                evap_fraction = 1.0;
            }
            // distribute evaporation between frost sub areas by percentage
            evap_sum = evap[lindex][0];
            for (fidx = (int)options.Nfrost - 1; fidx >= 0; fidx--) {
                evap[lindex][fidx] = avail_liq[lindex][fidx] * evap_fraction;
                avail_liq[lindex][fidx] -= evap[lindex][fidx];
                evap_sum -= evap[lindex][fidx] * frost_fract[fidx];
            }
        }
        else {
            for (fidx = (int)options.Nfrost - 1; fidx > 0; fidx--) {
                evap[lindex][fidx] = evap[lindex][0];
            }
        }
    }

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        /** ppt = amount of liquid water coming to the surface **/
        inflow = ppt;

        /**************************************************
           Initialize Variables
        **************************************************/
        tmp_z = 0.0;
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            Ksat[lindex] = cell->layer[lindex].Ksat /
                           global_param.runoff_steps_per_day;

            /** Set Layer Liquid Moisture Content **/
            liq[lindex] = org_moist[lindex] - layer[lindex].ice[fidx];

            /** Set Layer Frozen Moisture Content **/
            ice[lindex] = layer[lindex].ice[fidx];

            /** Set Layer Maximum Moisture Content **/
            max_moist[lindex] = soil_con->max_moist[lindex];

            if (options.MATRIC) {
                /** Set Matric Potential Exponent (Burdine model 1953) **/
                matric_expt[lindex] = (soil_con->expt[lindex] - 3.0) / 2.0;
            }

            /** Set Effective Porosity **/
            neff_moist = max(ice[lindex], resid_moist[lindex]);
            eff_porosity[lindex] = (max_moist[lindex] - neff_moist) /
                                   soil_con->depth[lindex];

            tmp_z += soil_con->depth[lindex];
            z[lindex] = tmp_z;
        }

        /******************************************************
           Runoff Based on Soil Moisture Level of Upper Layers
        ******************************************************/

        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        }
        compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, inflow, &A,
                                &(runoff[fidx]));

        // save dt_runoff based on initial runoff estimate,
        // since we will modify total runoff below for the case of completely saturated soil
        tmp_dt_runoff[fidx] = runoff[fidx] /
                              (double) runoff_steps_per_dt;

        /**************************************************
           Initialize Frost Area Variables
        **************************************************/

        dt_inflow = inflow / (double) runoff_steps_per_dt;

        Dsmax = soil_con->Dsmax / global_param.runoff_steps_per_day;

        for (time_step = 0; time_step < runoff_steps_per_dt; time_step++) {
            inflow = dt_inflow;

            /**************************************************
               Compute phreatic water level
            **************************************************/
            lwt = -1;
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                if (zwt[fidx] <= z[lindex]) {
                    lwt = lindex;
                    break;
                }
            }

            delta_z = zwt[fidx] - z[options.Nlayer - 1];
            if (delta_z < 0) {
                delta_z = 0;
            }

            /*************************************
               Compute Matric Potential of Sublayers
            *************************************/
            if (options.MATRIC) {
                // Set matric potential (based on moisture content and soil texture)
                for (lindex = 0; lindex < options.Nlayer; lindex++) {
                    tmp_liq = liq[lindex] - evap[lindex][fidx];
                    if (tmp_liq > resid_moist[lindex]) {
                        /** Brooks & Corey relation for matric potential **/
                        matric[lindex] = soil_con->bubble[lindex] *
                                         pow((tmp_liq - resid_moist[lindex]) /
                                             (max_moist[lindex] -
                                              resid_moist[lindex]),
                                             -matric_expt[lindex]);
                    }
                    else {
                        matric[lindex] = param.HUGE_RESIST;
                    }
                }
            }
            
            /*************************************
               Compute Drainage between Sublayers
            *************************************/
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                /** Brooks & Corey relation for hydraulic conductivity **/

                if ((tmp_liq = liq[lindex] - evap[lindex][fidx]) <
                    resid_moist[lindex]) {
                    tmp_liq = resid_moist[lindex];
                }

                if (liq[lindex] > resid_moist[lindex]) {
                    if (options.MATRIC) {
                        if (lindex < options.Nlayer - 1) {
                            avg_matric = pow(10, (soil_con->depth[lindex + 1] *
                                                  log10(fabs(matric[lindex])) +
                                                  soil_con->depth[lindex] *
                                                  log10(fabs(
                                                            matric[lindex +
                                                                   1]))) /
                                             (soil_con->depth[lindex] +
                                              soil_con->depth[lindex + 1]));

                            tmp_liq = resid_moist[lindex] +
                                      (max_moist[lindex] -
                                       resid_moist[lindex]) *
                                      pow(
                                (avg_matric /
                            soil_con->bubble[lindex]), -1 /
                                matric_expt[lindex]);
                        }
                    }

                    Q12[lindex] = Ksat[lindex] *
                                  pow(((tmp_liq -
                                        resid_moist[lindex]) /
                                       (soil_con->max_moist[lindex] -
                                        resid_moist[lindex])),
                                      soil_con->expt[lindex]);
                }
                else {
                    Q12[lindex] = 0.;
                }
            }

            /** Recompute hydraulic conductivity of lowest layer:
             * Exponential decay in drainage if water table is
             * below soil column (Equation 7 - Niu et al., 2007)
             * and no drainage if water table is in soil column **/
            lindex = options.Nlayer - 1;
            
            if (lwt == -1) {
                if(!options.GW_EQUILIBRIUM){
                    Q12[lindex] = Q12[lindex] *
                                  (1 - exp(-gw_con->expt * delta_z)) /
                                  (gw_con->expt * delta_z);
                }
            }
            else {
                Q12[lindex] = 0.;
            }
            
            /**************************************************
               Compute Baseflow
            **************************************************/
            dt_baseflow = Dsmax * exp(-gw_con->expt * zwt[fidx]);
            
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                Qb[lindex] = 0.0;
            }

            if (lwt != -1) {
                /** Calculate average conductivity **/
                avg_K = 0.0;
                for (lindex = lwt; lindex < options.Nlayer; lindex++) {
                    avg_K += Q12[lindex];
                }

                /** Update soil layer baseflow **/
                for (lindex = lwt; lindex < options.Nlayer; lindex++) {
                    if (avg_K == 0) {
                        Qb[lindex] = dt_baseflow / (options.Nlayer - lwt);
                    }
                    else {
                        Qb[lindex] = dt_baseflow * (Q12[lindex] / avg_K);
                    }
                }
            }
            
            /**************************************************
               Solve for Current Soil Layer Moisture, and
               Check Versus Maximum and Minimum Moisture Contents.
            **************************************************/

            last_index = 0;
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                if (lindex == 0) {
                    dt_runoff = tmp_dt_runoff[fidx];
                }
                else {
                    dt_runoff = 0;
                }

                /* transport moisture for all sublayers **/

                tmp_inflow = 0.;

                /** Update soil layer moisture content **/
                liq[lindex] = liq[lindex] +
                              (inflow - dt_runoff) -
                              (Q12[lindex] + evap[lindex][fidx] + Qb[lindex]);

                /** Verify that soil layer moisture is less than maximum **/
                if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
                    tmp_inflow = (liq[lindex] + ice[lindex]) -
                                 max_moist[lindex];
                    liq[lindex] = max_moist[lindex] - ice[lindex];

                    if (lindex == 0) {
                        Q12[lindex] += tmp_inflow;
                        tmp_inflow = 0;
                    }
                    else {
                        tmplayer = lindex;
                        while (tmp_inflow > 0) {
                            tmplayer--;
                            if (tmplayer < 0) {
                                /** If top layer saturated, add to runoff **/
                                runoff[fidx] += tmp_inflow;
                                tmp_inflow = 0;
                            }
                            else {
                                /** else add excess soil moisture to next higher layer **/
                                liq[tmplayer] += tmp_inflow;
                                if ((liq[tmplayer] + ice[tmplayer]) >
                                    max_moist[tmplayer]) {
                                    tmp_inflow =
                                        ((liq[tmplayer] +
                                          ice[tmplayer]) - max_moist[tmplayer]);
                                    liq[tmplayer] = max_moist[tmplayer] -
                                                    ice[tmplayer];
                                }
                                else {
                                    tmp_inflow = 0;
                                }
                            }
                        }
                    } /** end trapped excess moisture **/
                } /** end check if excess moisture in top layer **/

                /** verify that current layer moisture is greater than minimum **/
                if (liq[lindex] < 0) {
                    /** liquid cannot fall below 0 **/
                    Q12[lindex] += liq[lindex];
                    liq[lindex] = 0;
                }
                if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
                    /** moisture cannot fall below minimum **/
                    Q12[lindex] +=
                        (liq[lindex] + ice[lindex]) - resid_moist[lindex];
                    liq[lindex] = resid_moist[lindex] - ice[lindex];
                }

                inflow = (Q12[lindex] + tmp_inflow);
                Q12[lindex] += tmp_inflow;

                last_index++;
            } /* end loop through soil layers */

            /**************************************************
               Compute Aquifer Recharge
            **************************************************/
            dt_recharge = Q12[options.Nlayer - 1];
            
            if (lwt != -1){
                if (lwt == 0) {
                    dt_recharge += Q12[lwt];
                } else {
                    dt_recharge += Q12[lwt - 1];
                }
            }

            /**************************************************
               Compute Aquifer Evaporation
            **************************************************/            
            dt_evaporation = 0.0;
            
            if (lwt != -1){
                if (lwt == 0) {
                    for (lindex = 1; lindex < options.Nlayer; lindex++) {
                        dt_evaporation += evap[lindex][fidx];
                    }
                } else {
                    for (lindex = lwt; lindex < options.Nlayer; lindex++) {
                        dt_evaporation += evap[lindex][fidx];
                    }
                }
            }
            
            /**************************************************
               Compute Groundwater
            **************************************************/            
            /** Update groundwater content **/
            Wt[fidx] = Wt[fidx] +
                       dt_recharge - (dt_baseflow + dt_evaporation);
            
            if(lwt != -1 && dt_recharge < 0){
                // Water level was in soil column:
                // Lack of water caused abstractions from groundwater
                // Reduce groundwater below soil column
                Wt[fidx] = (GW_REF_DEPTH - z[options.Nlayer - 1]) * 
                            gw_con->Sy * MM_PER_M + dt_recharge;
            }
            
            /** Calculate groundwater level **/
            if (Wt[fidx] / gw_con->Sy / MM_PER_M < GW_REF_DEPTH -
                z[options.Nlayer - 1]) {
                // New water level is below soil column
                Wa[fidx] = Wt[fidx];
                zwt[fidx] = GW_REF_DEPTH - Wt[fidx] / gw_con->Sy / MM_PER_M;
            }
            else {
                // New water level is in soil column
                Wa[fidx] = (GW_REF_DEPTH - z[options.Nlayer - 1]) * 
                            gw_con->Sy * MM_PER_M;

                tmp_Wt = (Wt[fidx] - Wa[fidx]) / MM_PER_M;
                for (lindex = options.Nlayer - 1; (int)lindex >= 0; lindex--) {
                    // From bottom to top layer
                    if (tmp_Wt < soil_con->depth[lindex] * eff_porosity[lindex]) {
                        zwt[fidx] = z[lindex] - (tmp_Wt / eff_porosity[lindex]);                        
                        tmp_Wt = 0.0;
                        break;
                    }
                    tmp_Wt -= soil_con->depth[lindex] * eff_porosity[lindex];
                }

                // Remove excess water (above soil column)
                if (tmp_Wt > 0) {
                    zwt[fidx] = 0.0;
                }
                
                if(lwt == -1){
                    // Water level was below soil column:
                    // Water balance was handled in the aquifer
                    // Increase soil column water
                    lindex = options.Nlayer - 1;       
                    liq[lindex] += Wt[fidx] - Wa[fidx];
                    
                    /** Verify that soil layer moisture is less than maximum **/
                    tmp_moist = 0.;
                    if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
                        /* soil moisture above maximum */
                        tmp_moist =
                            ((liq[lindex] + ice[lindex]) - max_moist[lindex]);
                        liq[lindex] = max_moist[lindex] - ice[lindex];

                        tmplayer = lindex;
                        while (tmp_moist > 0) {
                            tmplayer--;
                            if (tmplayer < 0) {
                                /** If top layer saturated, add to runoff **/
                                runoff[fidx] += tmp_moist;
                                tmp_moist = 0;
                            }
                            else {
                                /** else add excess soil moisture to next higher layer **/
                                liq[tmplayer] += tmp_moist;
                                if ((liq[tmplayer] + ice[tmplayer]) >
                                    max_moist[tmplayer]) {
                                    tmp_moist =
                                        ((liq[tmplayer] +
                                          ice[tmplayer]) - max_moist[tmplayer]);
                                    liq[tmplayer] = max_moist[tmplayer] -
                                                    ice[tmplayer];
                                }
                                else {
                                    tmp_moist = 0;
                                }
                            }
                        } /** end trapped excess moisture **/
                    } /** end check if excess moisture in top layer **/
                }
            }

            recharge[fidx] += dt_recharge;
            baseflow[fidx] += dt_baseflow;
        } /* end of sub-dt time step loop */

        /** If negative baseflow, reduce evap accordingly **/
        if (baseflow[fidx] < 0) {
            log_err("Negative baseflow in groundwater mode? Impossible...");
            layer[lindex].evap += baseflow[fidx];
            baseflow[fidx] = 0;
        }

        /** Recompute Asat based on final moisture level of upper layers **/
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        }
        compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, 0, &A,
                                &tmp_runoff);

        /** Store tile-wide values **/
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            layer[lindex].moist +=
                ((liq[lindex] + ice[lindex]) * frost_fract[fidx]);
            layer[lindex].eff_saturation +=
                ((liq[lindex] + ice[lindex]) - resid_moist[lindex]) /
                (max_moist[lindex] - resid_moist[lindex]) *
                frost_fract[fidx];
        }
        cell->asat += A * frost_fract[fidx];
        cell->runoff += runoff[fidx] * frost_fract[fidx];
        cell->baseflow += baseflow[fidx] * frost_fract[fidx];

        gw_var->recharge += recharge[fidx] * frost_fract[fidx];
        gw_var->zwt += zwt[fidx] * frost_fract[fidx];
        gw_var->Wa += Wa[fidx] * frost_fract[fidx];
        gw_var->Wt += Wt[fidx] * frost_fract[fidx];
    }

    /** Compute water table depth **/
    wrap_compute_zwt(soil_con, cell);

    /** Recompute Thermal Parameters Based on New Moisture Distribution **/
    if (options.FULL_ENERGY || options.FROZEN_SOIL) {
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_layer = cell->layer[lindex];
            moist[lindex] = tmp_layer.moist;
        }

        ErrorFlag = distribute_node_moisture_properties(
            energy->moist,
            energy->ice,
            energy->kappa_node,
            energy->Cs_node,
            soil_con->Zsum_node,
            energy->T,
            soil_con->
            max_moist_node,
            soil_con->expt_node,
            soil_con->bubble_node,
            moist, soil_con->depth,
            soil_con->soil_dens_min,
            soil_con->bulk_dens_min,
            soil_con->quartz,
            soil_con->soil_density,
            soil_con->bulk_density,
            soil_con->organic,
            Nnodes,
            options.Nlayer,
            soil_con->FS_ACTIVE);
        if (ErrorFlag == ERROR) {
            return (ERROR);
        }
    }
    return (0);
}
