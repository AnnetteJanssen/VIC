#ifndef DAMS_H
#define DAMS_H

#include <vic_physical_constants.h>
#include <stdbool.h>
#define DAM_HIST_YEARS 3

#define DAM_AMP_STEP_POS 0.05
#define DAM_AMP_STEP_NEG -0.05

#define DAM_SAMP_FLO 0.75
#define DAM_SAMP_HYD 0
#define DAM_SAMP_SUP 1
#define DAM_EAMP_FLO 1
#define DAM_EAMP_HYD 1
#define DAM_EAMP_SUP 0

#define DAM_PREF_VOL_HYD 0.85
#define DAM_PREF_VOL_FLO 0.5
#define DAM_PREF_VOL_SUP 0.85
#define DAM_MIN_VOL_HYD 0.1
#define DAM_MIN_VOL_FLO 0
#define DAM_MIN_VOL_SUP 0.1

#define DAM_PREF_DEM_SUP 0.2

#define DAM_DIS_MOD_FRAC 0.75
#define DAM_DIS_MOD_SHAPE 6

#define DAM_FUN_HYD 1
#define DAM_FUN_IRR 2
#define DAM_FUN_WAS 3
#define DAM_FUN_FLO 4
#define DAM_FUN_OTH 5

#define DAYS_PER_MONTH_AVG 30.42
#define DAYS_PER_WEEK 7

typedef struct {
    size_t nd_active;
} dam_con_map_struct;

typedef struct {
    int id;
    int year;
    int function;
    double max_volume;
    size_t nservice;
    size_t *service;
    double *serve_factor;
} dam_con_struct;

typedef struct {
    double volume;
    double discharge;
    double inflow;
    double demand;

    double history_flow[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    double history_demand[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    double op_discharge[MONTHS_PER_YEAR];
    double op_discharge_irr[MONTHS_PER_YEAR];
    double op_volume[MONTHS_PER_YEAR];

    double total_flow;
    double total_demand;
    size_t total_steps;

    int op_year;
    size_t months_running;
} dam_var_struct;

bool dam_get_global_parameters(char *cmdstr);
void dam_validate_global_parameters(void);
void dam_start(void);
void dam_validate_domain(void);
void dam_alloc(void);
void initialize_dam_local_structures(void);
void dam_init(void);
void dam_set_output_meta_data_info(void);
void dam_set_state_meta_data_info(void);
void dam_generate_default_state(void);
void dam_history(size_t cur_cell);
void dam_run(size_t cur_cell);
double dam_area(double volume, double max_volume, double max_area,
                double max_height);
double dam_height(double area, double max_height);
void dam_put_data(size_t);
void dam_finalize(void);
void dam_add_types(void);

dam_var_struct    **dam_var;
dam_con_struct    **dam_con;
dam_con_map_struct *dam_con_map;

#endif /* DAMS_H */
