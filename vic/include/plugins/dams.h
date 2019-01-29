#ifndef DAMS_H
#define DAMS_H

#include <vic_physical_constants.h>
#include <stdbool.h>

#define DAM_HIST_YEARS 5

typedef struct {
    bool run;
    
    int year;
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

    int op_year;
    size_t months_running;
} dam_var_struct;

bool dam_get_global_parameters(char *cmdstr);
void dam_validate_global_param(void);
void dam_start(void);
void dam_validate_domain(void);
void dam_alloc(void);
void initialize_dam_local_structures(void);
void dam_init(void);
void dam_set_output_meta_data_info(void);
void dam_set_state_meta_data_info(void);
void dam_generate_default_state(void);

void local_dam_history(dam_con_struct *, dam_var_struct *, size_t cur_cell);
void global_dam_history(dam_con_struct *, dam_var_struct *, size_t cur_cell);
void local_dam_run(size_t cur_cell);
void global_dam_run(size_t cur_cell);

void dam_calc_opt_release(double *, double *, double *, double *, size_t);
void dam_corr_opt_release(double *, double *, size_t, double, double);
double dam_corr_release(double, double, double);
double dam_calc_k_factor(double, double);
double dam_calc_c_factor(double *, double, size_t);
void dam_calc_opt_storage(double *, double *, double *, size_t, double);
double dam_area(double, double, double, double);
double dam_height(double, double);

void dam_put_data(size_t);
void dam_finalize(void);
void dam_add_types(void);

dam_var_struct     *local_dam_var;
dam_var_struct     *global_dam_var;
dam_con_struct     *local_dam_con;
dam_con_struct     *global_dam_con;
#endif /* DAMS_H */
