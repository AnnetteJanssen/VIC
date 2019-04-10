#ifndef PLUGIN_GENERAL_H
#define PLUGIN_GENERAL_H

#include <vic_driver_shared_image.h>
#include <routing.h>
#include <support.h>
#include <wateruse.h>

enum {
    RANDOM_DECOMPOSITION,
    BASIN_DECOMPOSITION,
    FILE_DECOMPOSITION,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_DECOMPOSITION                   /**< used as a loop counter*/
};

enum {
    // routing
    STATE_DISCHARGE_DT,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_STATE_VARS                   /**< used as a loop counter*/
};

enum {
    // routing
    OUT_DISCHARGE,                      /**< river discharge [m3 s-1]) */
    OUT_STREAM_MOIST,
    // water-use
    OUT_AVAILABLE_GW,
    OUT_AVAILABLE_SURF,
    OUT_AVAILABLE_DAM,
    OUT_AVAILABLE_REMOTE,
    OUT_DEMAND_GW,
    OUT_DEMAND_SURF,
    OUT_DEMAND_REMOTE,
    OUT_WITHDRAWN_GW,
    OUT_WITHDRAWN_SURF,
    OUT_WITHDRAWN_DAM,
    OUT_WITHDRAWN_REMOTE,
    OUT_AVAILABLE,
    OUT_DEMAND,
    OUT_WITHDRAWN,
    OUT_RETURNED,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_OUTVAR_TYPES                /**< used as a loop counter*/
};

enum {
    // routing
    FORCING_DISCHARGE,
    // water-use
    FORCING_IRR_DEMAND,
    FORCING_IRR_GROUNDWATER,
    FORCING_IRR_CONSUMPTION,
    FORCING_MUN_DEMAND,
    FORCING_MUN_GROUNDWATER,
    FORCING_MUN_CONSUMPTION,
    FORCING_LIV_DEMAND,
    FORCING_LIV_GROUNDWATER,
    FORCING_LIV_CONSUMPTION,
    FORCING_MAN_DEMAND,
    FORCING_MAN_GROUNDWATER,
    FORCING_MAN_CONSUMPTION,
    FORCING_ENG_DEMAND,
    FORCING_ENG_GROUNDWATER,
    FORCING_ENG_CONSUMPTION,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_FORCING_TYPES                /**< used as a loop counter*/
};

typedef struct {
    // simulation options
    short unsigned int DECOMPOSITION;
    bool ROUTING;
    bool WATERUSE;

    // module options
    short unsigned int UH_LENGTH;
    bool FORCE_ROUTING;
    short unsigned int NWUTYPES;
    short unsigned int NWURECEIVING;
    int WU_INPUT[WU_NSECTORS];
} plugin_option_struct;

typedef struct {
    size_t rout_steps_per_day;
    double rout_dt;
} plugin_global_param_struct;

typedef struct {
} plugin_parameters_struct;

typedef struct {
    // parameters
    nameid_struct routing;  /**< routing parameter file */
    nameid_struct decomposition;   /**< basin parameter file */
    nameid_struct wateruse;        /**< water-use parameter file */

    // forcing
    nameid_struct forcing[PLUGIN_N_FORCING_TYPES]; /**< forcing files */
    char f_path_pfx[PLUGIN_N_FORCING_TYPES][MAXSTRING]; /**< path and prefix for forcing files */
    char f_varname[PLUGIN_N_FORCING_TYPES][MAXSTRING]; /**< variable name for forcing files */
} plugin_filenames_struct;

// Setup
void plugin_initialize_global_structures(void);
bool plugin_get_global_param(char *cmdstr);
void plugin_set_force_type(char *cmdstr);
void plugin_validate_global_param(void);
bool plugin_get_parameters(char *cmdstr);
void plugin_validate_parameters(void);

void plugin_initialize_mpi(void);
void plugin_broadcast_filenames(void);
void plugin_broadcast_options(void);
void plugin_broadcast_global_params(void);
void plugin_broadcast_params(void);
void plugin_mpi_map_decomp_domain(size_t, size_t, int **, int **, size_t **);

void plugin_start(void);
void plugin_alloc(void);
void plugin_init(void);
void plugin_finalize(void);

void plugin_generate_default_state(void);
void plugin_compute_derived_state_vars(void);
void plugin_check_init_state_file(void);
void plugin_restore(void);

void plugin_print_decomposition(size_t, int **);
void plugin_print_global_param(plugin_global_param_struct *);
void plugin_print_options(plugin_option_struct *);
void plugin_print_parameters(plugin_parameters_struct *);

// Output
void plugin_set_output_met_data_info(void);
void plugin_initialize_nc_file(nc_file_struct *nc_file);
void plugin_add_hist_dim(nc_file_struct *nc, stream_struct  *stream);
void plugin_set_nc_var_info(unsigned int varid, unsigned short int dtype,
                            nc_file_struct    *nc_hist_file,
                            nc_var_struct     *nc_var);
void plugin_set_nc_var_dimids(unsigned int varid, nc_file_struct *nc_hist_file,
                              nc_var_struct  *nc_var);
void plugin_get_default_outvar_aggtype(unsigned int varid, unsigned int *);
void plugin_set_state_meta_data_info(void);
void plugin_set_nc_state_file_info(nc_file_struct *nc_state_file);
void plugin_add_state_dim(char *filename, nc_file_struct *nc_state_file);
void plugin_add_state_dim_var(char *filename, nc_file_struct *nc_state_file);
void plugin_add_state_dim_var_data(char           *filename,
                                   nc_file_struct *nc_state_file);
void plugin_set_nc_state_var_info(nc_file_struct *nc);
void plugin_store(nc_file_struct *);

// Simulation
void plugin_force(void);
void plugin_run(void);
void plugin_put_data(void);

MPI_Datatype               plugin_mpi_global_struct_type;
MPI_Datatype               plugin_mpi_filenames_struct_type;
MPI_Datatype               plugin_mpi_option_struct_type;
MPI_Datatype               plugin_mpi_param_struct_type;

plugin_option_struct       plugin_options;
plugin_global_param_struct plugin_global_param;
plugin_filenames_struct    plugin_filenames;
plugin_parameters_struct   plugin_param;

#endif /* PLUGIN_GENERAL_H */
