/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine converts data units, and stores finalized values in an array
 * for later output to the output files.
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

#include <vic_driver_shared_all.h>

/******************************************************************************
 * @brief    This routine converts data units, and stores finalized values in
 *           an array for later output to the output files.
 *****************************************************************************/
void
put_data_tlake(all_vars_struct   *all_vars,
               soil_con_struct   *soil_con,
               veg_con_struct    *veg_con,
               veg_lib_struct    *veg_lib,
               lake_con_struct   *lake_con,
               double           **out_data,
               timer_struct      *timer)
{
    extern option_struct       options;

    size_t                     veg;
    int                        lake_class;
    int                        nidx;
    size_t                     band;
    size_t                     Nbands;
    bool                       overstory;
    bool                       HasVeg;
    bool                       IsWet;
    bool                      *AboveTreeLine;
    double                    *AreaFract;
    double                    *frost_fract;
    double                     frost_slope;
    double                     Cv;
    double                     Clake;
    double                     cv_baresoil;
    double                     cv_veg;
    double                     cv_overstory;
    double                     cv_snow;
    double                    *TreeAdjustFactor;
    double                     ThisAreaFract;
    double                     ThisTreeAdjust;
    size_t                     i;

    cell_data_struct         **cell;
    energy_bal_struct        **energy;
    lake_var_struct          **lake_var;
    snow_data_struct         **snow;
    veg_var_struct           **veg_var;

    cell = all_vars->cell;
    energy = all_vars->energy;
    lake_var = all_vars->lake_var;
    snow = all_vars->snow;
    veg_var = all_vars->veg_var;

    AboveTreeLine = soil_con->AboveTreeLine;
    AreaFract = soil_con->AreaFract;
    frost_fract = soil_con->frost_fract;
    frost_slope = soil_con->frost_slope;

    // Compute treeline adjustment factors
    TreeAdjustFactor = calloc(options.SNOW_BAND, sizeof(*TreeAdjustFactor));
    check_alloc_status(TreeAdjustFactor, "Memory allocation error.");
    for (band = 0; band < options.SNOW_BAND; band++) {
        if (AboveTreeLine[band]) {
            Cv = 0;
            for (veg = 0; veg < veg_con[0].vegetat_type_num; veg++) {
                if (veg_lib[veg_con[veg].veg_class].overstory) {
                    if (options.LAKES && veg_con[veg].LAKE) {
                        lake_class = veg_con[veg].lake_class;
                        
                        if (band == 0) {
                            // Fraction of tile that is flooded
                            Clake = lake_var[veg]->sarea / lake_con[lake_class].basin[0];
                            Cv += veg_con[veg].Cv * (1 - Clake);
                        }
                    }
                    else {
                        Cv += veg_con[veg].Cv;
                    }
                }
            }
            TreeAdjustFactor[band] = 1. / (1. - Cv);
        }
        else {
            TreeAdjustFactor[band] = 1.;
        }
        if (TreeAdjustFactor[band] != 1) {
            log_warn("Tree adjust factor for band %zu is equal to %f.",
                     band, TreeAdjustFactor[band]);
        }
    }

    cv_baresoil = 0;
    cv_veg = 0;
    cv_overstory = 0;
    cv_snow = 0;

    // Initialize output data to zero
    zero_output_list(out_data);

    /****************************************
       Store Output for all Vegetation Types (except lakes)
    ****************************************/
    for (veg = 0; veg <= veg_con[0].vegetat_type_num; veg++) {
        Cv = veg_con[veg].Cv;
        Clake = 0;
        Nbands = options.SNOW_BAND;
        IsWet = false;

        if (veg < veg_con[0].vegetat_type_num) {
            HasVeg = true;
        }
        else {
            HasVeg = false;
        }

        if (Cv > 0) {
            // Check if this is lake/wetland tile
            if (options.LAKES && veg_con[veg].LAKE) {
                lake_class = veg_con[veg].lake_class;
                
                Clake = lake_var[veg]->sarea / lake_con[lake_class].basin[0];
                Nbands = 1;
                IsWet = true;
            }

            overstory = veg_lib[veg_con[veg].veg_class].overstory;

            /*********************************
               Store Output for all Bands
            *********************************/
            for (band = 0; band < Nbands; band++) {
                ThisAreaFract = AreaFract[band];
                ThisTreeAdjust = TreeAdjustFactor[band];
                if (IsWet) {
                    ThisAreaFract = 1;
                    ThisTreeAdjust = 1;
                }

                if (ThisAreaFract > 0. &&
                    (veg == veg_con[0].vegetat_type_num ||
                     (!AboveTreeLine[band] ||
                      (AboveTreeLine[band] &&
                       !overstory)))) {
                    /** compute running totals of various landcovers **/
                    if (HasVeg) {
                        cv_veg += Cv * ThisAreaFract * ThisTreeAdjust;
                    }
                    else {
                        cv_baresoil += Cv * ThisAreaFract * ThisTreeAdjust;
                    }
                    if (overstory) {
                        cv_overstory += Cv * ThisAreaFract * ThisTreeAdjust;
                    }
                    if (snow[veg][band].swq > 0.0) {
                        cv_snow += Cv * ThisAreaFract * ThisTreeAdjust;
                    }

                    // Store Wetland-Specific Variables
                    if (IsWet) {
                        // Wetland soil temperatures
                        for (i = 0; i < options.Nnode; i++) {
                            out_data[OUT_SOIL_TNODE_WL][i] =
                                energy[veg][band].T[i];
                        }
                    }

                    /**********************************
                       Record Lake Variables
                    **********************************/
                    if (IsWet) {
                        // Override some variables of soil under lake with those of wetland
                        // This is for those variables whose lake values shouldn't be included
                        // in grid cell average
                        // Note: doing this for eb terms will lead to reporting of eb errors
                        // this should be fixed when we implement full thermal solution beneath lake
                        for (i = 0; i < MAX_FRONTS; i++) {
                            lake_var[veg]->energy.fdepth[i] =
                                energy[veg][band].fdepth[i];
                            lake_var[veg]->energy.tdepth[i] =
                                energy[veg][band].fdepth[i];
                        }
                        for (i = 0; i < options.Nnode; i++) {
                            lake_var[veg]->energy.ice[i] = energy[veg][band].ice[i];
                            lake_var[veg]->energy.T[i] = energy[veg][band].T[i];
                        }
                        lake_var[veg]->soil.pot_evap =
                            cell[veg][band].pot_evap;
                        lake_var[veg]->soil.rootmoist = cell[veg][band].rootmoist;
                        lake_var[veg]->energy.deltaH = energy[veg][band].deltaH;
                        lake_var[veg]->energy.fusion = energy[veg][band].fusion;
                        lake_var[veg]->energy.grnd_flux = energy[veg][band].grnd_flux;


                        /*********************************
                           Record Water Balance Terms
                        *********************************/
                        collect_wb_terms(lake_var[veg]->soil,
                                         veg_var[0][0],
                                         lake_var[veg]->snow,
                                         Cv,
                                         ThisAreaFract,
                                         ThisTreeAdjust,
                                         0,
                                         Clake,
                                         overstory,
                                         frost_fract,
                                         out_data);

                        /**********************************
                           Record Energy Balance Terms
                        **********************************/
                        collect_eb_terms(lake_var[veg]->energy,
                                         lake_var[veg]->snow,
                                         lake_var[veg]->soil,
                                         Cv,
                                         ThisAreaFract,
                                         ThisTreeAdjust,
                                         0,
                                         1,
                                         Clake,
                                         overstory,
                                         band,
                                         frost_fract,
                                         frost_slope,
                                         out_data);

                        // Store Lake-Specific Variables

                        // Lake ice
                        if (lake_var[veg]->new_ice_area > 0.0) {
                            out_data[OUT_LAKE_ICE][0] =
                                (lake_var[veg]->ice_water_eq /
                                 lake_var[veg]->new_ice_area) * CONST_RHOICE /
                                CONST_RHOFW;
                            out_data[OUT_LAKE_ICE_TEMP][0] =
                                lake_var[veg]->tempi;
                            out_data[OUT_LAKE_ICE_HEIGHT][0] =
                                lake_var[veg]->hice;
                            out_data[OUT_LAKE_SWE][0] = lake_var[veg]->swe /
                                                        lake_var[veg]->areai;       // m over lake ice
                            out_data[OUT_LAKE_SWE_V][0] = lake_var[veg]->swe;  // m3
                        }
                        else {
                            out_data[OUT_LAKE_ICE][0] = 0.0;
                            out_data[OUT_LAKE_ICE_TEMP][0] = 0.0;
                            out_data[OUT_LAKE_ICE_HEIGHT][0] = 0.0;
                            out_data[OUT_LAKE_SWE][0] = 0.0;
                            out_data[OUT_LAKE_SWE_V][0] = 0.0;
                        }
                        out_data[OUT_LAKE_DSWE_V][0] = lake_var[veg]->swe -
                                                       lake_var[veg]->swe_save;       // m3
                        // same as OUT_LAKE_MOIST
                        out_data[OUT_LAKE_DSWE][0] =
                            (lake_var[veg]->swe - lake_var[veg]->swe_save) * MM_PER_M /
                            soil_con->cell_area;

                        // Lake dimensions
                        out_data[OUT_LAKE_AREA_FRAC][0] = Cv * Clake;
                        out_data[OUT_LAKE_DEPTH][0] = lake_var[veg]->ldepth;
                        out_data[OUT_LAKE_SURF_AREA][0] = lake_var[veg]->sarea;
                        if (out_data[OUT_LAKE_SURF_AREA][0] > 0) {
                            out_data[OUT_LAKE_ICE_FRACT][0] =
                                lake_var[veg]->new_ice_area /
                                out_data[OUT_LAKE_SURF_AREA][0];
                        }
                        else {
                            out_data[OUT_LAKE_ICE_FRACT][0] = 0.;
                        }
                        out_data[OUT_LAKE_VOLUME][0] = lake_var[veg]->volume;
                        out_data[OUT_LAKE_DSTOR_V][0] = lake_var[veg]->volume -
                                                        lake_var[veg]->
                                                        volume_save;
                        // mm over gridcell
                        out_data[OUT_LAKE_DSTOR][0] =
                            (lake_var[veg]->volume - lake_var[veg]->volume_save) *
                            MM_PER_M /
                            soil_con->cell_area;

                        // Other lake characteristics
                        out_data[OUT_LAKE_SURF_TEMP][0] = lake_var[veg]->temp[0];
                        for(nidx = 0; nidx < lake_var[veg]->activenod; nidx++){
                            out_data[OUT_LAKE_NODE_TEMP][nidx] = lake_var[veg]->temp[nidx];
                        }
                        
                        if (out_data[OUT_LAKE_SURF_AREA][0] > 0) {
                            // mm over gridcell
                            out_data[OUT_LAKE_MOIST][0] =
                                (lake_var[veg]->volume / soil_con->cell_area) *
                                MM_PER_M;
                            // same as OUT_LAKE_MOIST
                            out_data[OUT_SURFSTOR][0] =
                                (lake_var[veg]->volume / soil_con->cell_area) *
                                MM_PER_M;
                        }
                        else {
                            out_data[OUT_LAKE_MOIST][0] = 0;
                            out_data[OUT_SURFSTOR][0] = 0;
                        }

                        // Lake moisture fluxes
                        out_data[OUT_LAKE_BF_IN_V][0] =
                            lake_var[veg]->baseflow_in;  // m3
                        out_data[OUT_LAKE_BF_OUT_V][0] =
                            lake_var[veg]->baseflow_out;  // m3
                        out_data[OUT_LAKE_CHAN_IN_V][0] =
                            lake_var[veg]->channel_in;  // m3
                        out_data[OUT_LAKE_CHAN_OUT_V][0] =
                            lake_var[veg]->runoff_out;  // m3
                        out_data[OUT_LAKE_EVAP_V][0] = lake_var[veg]->evapw;  // m3
                        out_data[OUT_LAKE_PREC_V][0] = lake_var[veg]->prec;  // m3
                        out_data[OUT_LAKE_RCHRG_V][0] = lake_var[veg]->recharge;  // m3
                        out_data[OUT_LAKE_RO_IN_V][0] = lake_var[veg]->runoff_in;  // m3
                        out_data[OUT_LAKE_VAPFLX_V][0] =
                            lake_var[veg]->vapor_flux;  // m3
                        out_data[OUT_LAKE_BF_IN][0] =
                            lake_var[veg]->baseflow_in * MM_PER_M /
                            soil_con->cell_area;  // mm over gridcell
                        out_data[OUT_LAKE_BF_OUT][0] =
                            lake_var[veg]->baseflow_out * MM_PER_M /
                            soil_con->cell_area;  // mm over gridcell
                        out_data[OUT_LAKE_CHAN_OUT][0] =
                            lake_var[veg]->runoff_out * MM_PER_M /
                            soil_con->cell_area;  // mm over gridcell
                        // mm over gridcell
                        out_data[OUT_LAKE_EVAP][0] = lake_var[veg]->evapw *
                                                     MM_PER_M /
                                                     soil_con->cell_area;
                        // mm over gridcell
                        out_data[OUT_LAKE_RCHRG][0] = lake_var[veg]->recharge *
                                                      MM_PER_M /
                                                      soil_con->cell_area;
                        // mm over gridcell
                        out_data[OUT_LAKE_RO_IN][0] = lake_var[veg]->runoff_in *
                                                      MM_PER_M /
                                                      soil_con->cell_area;
                        out_data[OUT_LAKE_VAPFLX][0] =
                            lake_var[veg]->vapor_flux * MM_PER_M /
                            soil_con->cell_area;  // mm over gridcell
                    } // End if options.LAKES etc.
                } // End if ThisAreaFract etc.
            } // End loop over bands
        } // End if Cv > 0
    } // End loop over veg
    
    free((char *) (TreeAdjustFactor));

    // vic_run run time
    out_data[OUT_TIME_VICRUN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_VICRUN_CPU][0] = timer->delta_cpu;
}