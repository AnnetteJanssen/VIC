/******************************************************************************
 * @section DESCRIPTION
 *
 * Run function for image mode driver.
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

#include <vic.h>

/******************************************************************************
 * @brief    Run VIC for one timestep and store output data
 *****************************************************************************/
void
vic_run(dmy_struct *dmy_current)
{
    extern size_t              current;
    extern all_vars_struct    *all_vars;
    extern gw_var_struct    ***gw_var;
    extern force_data_struct  *force;
    extern domain_struct       local_domain;
    extern option_struct       options;
    extern global_param_struct global_param;
    extern lake_con_struct     lake_con;
    extern double           ***out_data;
    extern stream_struct      *output_streams;
    extern save_data_struct   *save_data;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern gw_con_struct      *gw_con;
    extern veg_hist_struct   **veg_hist;
    extern veg_lib_struct    **veg_lib;
    extern int                 mpi_rank;
    
    size_t                     i;
    size_t                     cur_cell;
    
    timer_struct               timer;

    if (mpi_rank == VIC_MPI_ROOT) {
        // Print the current timestep info before running vic_run
        printf("Running timestep: %zu (%02i-%02i-%2i, %02i:00)\n",
               current,
               dmy_current->day,
               dmy_current->month,
               dmy_current->year,
               dmy_current->dayseconds / SEC_PER_HOUR);
    }
    
    /******************************************************************************
      VIC & cell operations
     *****************************************************************************/
    timer_start(&timer);
    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        update_step_vars(&(all_vars[i]), veg_con[i], &soil_con[i], veg_hist[i]);
        
        if (options.GROUNDWATER) {
            run_gw_general(&(force[i]), &(all_vars[i]), gw_var[i],
                       dmy_current, &global_param,
                       &lake_con, &(soil_con[i]), veg_con[i], veg_lib[i],
                       &(gw_con[i]));
        }
        else {
            run_general(&(force[i]), &(all_vars[i]),
                    dmy_current, &global_param,
                    &lake_con, &(soil_con[i]), veg_con[i], veg_lib[i]);
        }

        if (options.IRRIGATION) {
            irr_run(i);
        }
    }
    timer_stop(&timer);

    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        if (options.GROUNDWATER) {
            put_gw_data(&(all_vars[i]), gw_var[i], &(force[i]), &(soil_con[i]), veg_con[i],
                     veg_lib[i], &lake_con, out_data[i], &(save_data[i]),
                     &timer);
        }
        else {
            put_data(&(all_vars[i]), &(force[i]), &(soil_con[i]), veg_con[i],
                     veg_lib[i], &lake_con, out_data[i], &(save_data[i]),
                     &timer);
        }
    }

    /******************************************************************************
     Plugins & ordered operations
     *****************************************************************************/
    timer_start(&timer);
    if (options.ROUTING_TYPE == ROUTING_RVIC) {
        routing_rvic_run();
        if(options.DAMS || options.EFR || options.WATER_USE){
            log_err("Plugins not yet implemented for ROUTING_RVIC");
        }
    }
    else if (options.ROUTING_TYPE == ROUTING_RANDOM) {
        rout_random_run();
        if(options.DAMS || options.EFR || options.WATER_USE){
            log_err("Plugins not yet implemented for ROUTING_RANDOM");
        }
    }
    else if (options.ROUTING_TYPE == ROUTING_BASIN) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            cur_cell = routing_order[i];
                     
            rout_basin_run(cur_cell);
            
            if(options.DAMS || options.EFR || options.WATER_USE){
                if (options.EFR) {
                    efr_run(cur_cell);
                }
                if (options.DAMS) {
                    dam_run(cur_cell);
                }

                if (options.WATER_USE) {
                    if (options.IRRIGATION && 
                            options.WU_INPUT_LOCATION[WU_IRRIGATION] == 
                            WU_INPUT_CALCULATE) {
                        irr_set_demand(cur_cell);
                    }

                    wu_run(cur_cell);

                    if (options.IRRIGATION && 
                            options.WU_INPUT_LOCATION[WU_IRRIGATION] == 
                            WU_INPUT_CALCULATE) {
                        irr_get_withdrawn(cur_cell);
                        irr_run_ponding_leftover(cur_cell);
                    }
                }     
            }
        }
    }
    timer_stop(&timer);
    
    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        plugin_put_data(out_data[i], &timer);

        if (options.ROUTING) {
            rout_put_data(i);
        }
        if (options.EFR) {
            efr_put_data(i);
        }
        if (options.IRRIGATION) {
            irr_put_data(i);
        }
        if (options.WATER_USE) {
            wu_put_data(i);
        }
        if (options.DAMS) {
            dam_put_data(i);
        }
    }
    
    /******************************************************************************
     Aggregate
     *****************************************************************************/
    for (i = 0; i < options.Noutstreams; i++) {
        agg_stream_data(&(output_streams[i]), dmy_current, out_data);
    }
}
