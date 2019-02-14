#ifndef DAMS_H
#define DAMS_H

#include <plugin.h>

#define NODATA_DAM    -1
#define DAM_HIST_YEARS 5

typedef struct {
    size_t nd_types;
    size_t nd_active;
    int *didx;
} dam_con_map_struct;

typedef struct {
    unsigned short int year;
    double capacity;
    double inflow_frac;
    
    size_t nservice;
    size_t *service;
    double *service_frac;
} dam_con_struct;

typedef struct {
    bool active;
    
    double inflow;
    double demand;
    double efr;
    double release;
    double storage;

    double history_inflow[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    double history_demand[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    double history_efr[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    double op_release[MONTHS_PER_YEAR];
    double op_storage[MONTHS_PER_YEAR];

    double total_inflow;
    double total_demand;
    double total_efr;
    size_t register_steps;

    int op_month;
    size_t months_running;
} dam_var_struct;

bool dam_get_global_param(char *);
void dam_validate_global_param(void);
bool dam_get_parameters(char *);
void dam_validate_parameters(void);
void dam_start(void);

void dam_alloc(void);
void dam_initialize_local_structures(void);

void dam_init(void);
void dam_generate_default_state(void);

void dam_set_output_met_data_info(void);
void dam_initialize_nc_file(nc_file_struct *);
void dam_add_hist_dim(nc_file_struct *, stream_struct *);
void dam_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void dam_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void dam_history(unsigned int, unsigned int *);
void dam_put_data(size_t);


void local_dam_register(dam_con_struct *, dam_var_struct *, size_t);
void global_dam_register(dam_con_struct *, dam_var_struct *, size_t);
void dam_calc_opt_release(double *, double *, double *, double *, size_t);
void dam_corr_opt_release(double *, double *, size_t, double, double);
double dam_corr_release(double, double, double, double);
double dam_calc_k_factor(double, double);
double dam_calc_c_factor(double *, double, size_t, size_t *);
void dam_calc_opt_storage(double *, double *, double *, size_t, size_t*, double);
double dam_area(double, double, double, double);
double dam_height(double, double);
void local_dam_run(size_t);
void global_dam_run(size_t);

void dam_finalize(void);

dam_con_map_struct *global_dam_con_map;
dam_var_struct    **global_dam_var;
dam_con_struct    **global_dam_con;
dam_con_map_struct *local_dam_con_map;
dam_var_struct    **local_dam_var;
dam_con_struct    **local_dam_con;

#endif
