#ifndef WATER_USE_H
#define WATER_USE_H

#include <stdbool.h>

#define MAX_RETURN_STEPS 50
#define MAX_COMPENSATION_STEPS 100

enum{
    WU_INPUT_FROM_FILE,
    WU_INPUT_CALCULATE,
    WU_INPUT_NONE,
    WU_INPUT_NLOCATIONS
};

enum{
    WU_STRATEGY_EQUAL,
    WU_STRATEGY_PRIORITY,
    WU_NSTRATEGY
};

typedef struct{    
    size_t nreceiving;
    size_t *receiving;
    
    size_t nservice;
    size_t *service;
    size_t *service_idx;
}wu_con_struct;

typedef struct{
    double *demand;
    double *consumption_fraction;
}wu_force_struct;

typedef struct{
    double demand;
    double consumption_fraction;
}wu_hist_struct;

typedef struct{
    double demand;
    double withdrawn;
    double consumed;    
    double returned;
}wu_var_struct;

bool wu_get_global_parameters(char *cmdstr);
void wu_validate_global_parameters(void);
void wu_start(void);
void wu_alloc(void);
void wu_late_alloc(void);
void initialize_wu_local_structures(void);
void wu_init(void);
void wu_set_output_meta_data_info(void);
void wu_set_state_meta_data_info(void);
bool wu_history(int, unsigned int *);
void wu_forcing(void);
void wu_run(size_t cur_cell);
void wu_put_data(void);
void wu_finalize(void);
void wu_add_types(void);

wu_var_struct **wu_var;
wu_hist_struct **wu_hist;
wu_force_struct **wu_force;
wu_con_struct *wu_con;

#endif