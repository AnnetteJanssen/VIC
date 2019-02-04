#ifndef DAMS_H
#define DAMS_H

#define NODATA_DAM    -1
#define DAM_HIST_YEARS 5

typedef struct {
    size_t nd_types;
    size_t nd_active;
    size_t *didx;
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

    int op_year;
    size_t months_running;
} dam_var_struct;

dam_con_map_struct *global_dam_con_map;
dam_var_struct    **global_dam_var;
dam_con_struct    **global_dam_con;
dam_con_map_struct *local_dam_con_map;
dam_var_struct    **local_dam_var;
dam_con_struct    **local_dam_con;

#endif
