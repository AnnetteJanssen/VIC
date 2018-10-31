/******************************************************************************
 * @section DESCRIPTION
 *
 * Stand-alone image mode driver of the VIC model
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

size_t              NF, NR;
size_t              current;
size_t             *filter_active_cells = NULL;
size_t             *mpi_map_mapping_array = NULL;
all_vars_struct    *all_vars = NULL;
force_data_struct  *force = NULL;
dmy_struct         *dmy = NULL;
dmy_struct          dmy_state;
filenames_struct    filenames;
filep_struct        filep;
domain_struct       global_domain;
global_param_struct global_param;
lake_con_struct    *lake_con = NULL;
domain_struct       local_domain;
MPI_Comm            MPI_COMM_VIC = MPI_COMM_WORLD;
MPI_Datatype        mpi_global_struct_type;
MPI_Datatype        mpi_filenames_struct_type;
MPI_Datatype        mpi_location_struct_type;
MPI_Datatype        mpi_alarm_struct_type;
MPI_Datatype        mpi_option_struct_type;
MPI_Datatype        mpi_param_struct_type;
int                *mpi_map_local_array_sizes = NULL;
int                *mpi_map_global_array_offsets = NULL;
int                 mpi_rank;
int                 mpi_size;
int                 mpi_decomposition;
option_struct       options;
parameters_struct   param;
soil_con_struct    *soil_con = NULL;
veg_con_map_struct *veg_con_map = NULL;
veg_con_struct    **veg_con = NULL;
veg_hist_struct   **veg_hist = NULL;
veg_lib_struct    **veg_lib = NULL;
elev_con_map_struct *elev_con_map = NULL;
save_data_struct   *save_data;  // [ncells]
double           ***out_data = NULL;  // [ncells, nvars, nelem]
stream_struct      *output_streams = NULL;  // [nstreams]
nc_file_struct     *nc_hist_files = NULL;  // [nstreams]
metadata_struct    *state_metadata = NULL;
metadata_struct    *out_metadata = NULL;
node               *outvar_types = NULL;
node               *state_vars = NULL;

/******************************************************************************
 * @brief   Stand-alone image mode driver of the VIC model
 * @details The image mode driver runs VIC for a single timestep for all grid
 *          cells before moving on to the next timestep.
 *
 * @param argc Argument count
 * @param argv Argument vector
 *****************************************************************************/
int
main(int    argc,
     char **argv)
{
    int          status;
    timer_struct global_timers[N_TIMERS];
    char         state_filename[MAXSTRING];

    // start vic all timer
    timer_start(&(global_timers[TIMER_VIC_ALL]));
    // start vic init timer
    timer_start(&(global_timers[TIMER_VIC_INIT]));

    // Initialize MPI - note: logging not yet initialized
    status = MPI_Init(&argc, &argv);
    check_mpi_status(status, "MPI error.");

    // Initialize Log Destination
    initialize_log();

    // initialize mpi
    initialize_mpi();

    // process command line arguments
    if (mpi_rank == VIC_MPI_ROOT) {
        cmd_proc(argc, argv, filenames.global);
    }

    // read global parameters
    vic_start();
    
    // decompose domains
    vic_domain();

    // allocate memory
    vic_alloc();

    // initialize model parameters from parameter files
    vic_init();

    // populate model state, either using a cold start or from a restart file
    vic_populate_model_state(&(dmy[0]));

    // initialize output structures
    vic_init_output(&(dmy[0]));

    // Initialization is complete, print settings
    if (mpi_rank == VIC_MPI_ROOT) {
        log_info(
            "Initialization is complete; print global param, parameters and options structures");
        print_global_param(&global_param);
        print_option(&options);
        print_parameters(&param);
    }

    // stop init timer
    timer_stop(&(global_timers[TIMER_VIC_INIT]));
    // start vic run timer
    timer_start(&(global_timers[TIMER_VIC_RUN]));
    // initialize vic force timer
    timer_init(&(global_timers[TIMER_VIC_FORCE]));
    // initialize vic write timer
    timer_init(&(global_timers[TIMER_VIC_WRITE]));
    
    // loop over all timesteps
    for (current = 0; current < global_param.nrecs; current++) {
        // read forcing data
        timer_continue(&(global_timers[TIMER_VIC_FORCE]));
        vic_force();
        timer_stop(&(global_timers[TIMER_VIC_FORCE]));

        // run vic over the domain
        vic_run(&(dmy[current]));

        // Write history files
        timer_continue(&(global_timers[TIMER_VIC_WRITE]));
        vic_write_output(&(dmy[current]));
        timer_stop(&(global_timers[TIMER_VIC_WRITE]));

        // Write state file
        if (check_save_state_flag(current, &dmy_state)) {
            vic_store(&dmy_state, state_filename);
        }
    }
    // stop vic run timer
    timer_stop(&(global_timers[TIMER_VIC_RUN]));
    // start vic final timer
    timer_start(&(global_timers[TIMER_VIC_FINAL]));

    // clean up
    vic_finalize();

    // finalize MPI
    status = MPI_Finalize();
    check_mpi_status(status, "MPI error.");

    log_info("Completed running VIC %s", VIC_DRIVER);

    // stop vic final timer
    timer_stop(&(global_timers[TIMER_VIC_FINAL]));
    // stop vic all timer
    timer_stop(&(global_timers[TIMER_VIC_ALL]));

    if (mpi_rank == VIC_MPI_ROOT) {
        // write timing info
        write_vic_timing_table(global_timers, VIC_DRIVER);
    }

    return EXIT_SUCCESS;
}