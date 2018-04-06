#ifndef WATER_USE_H
#define WATER_USE_H

#include <stdbool.h>

#define MAX_RETURN_STEPS 50
#define MAX_COMPENSATION_STEPS 100

enum{
    WU_RETURN_GROUNDWATER,
    WU_RETURN_SURFACEWATER,
    WU_RETURN_NLOCATIONS
};

enum{
    WU_INPUT_YEARLY,
    WU_INPUT_MONTHLY,
    WU_INPUT_DAILY,
    WU_INPUT_NFREQUENCIES
};

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
    size_t link_id;
    int nsending;
    int nreceiving;
    size_t *sending;
    size_t *receiving;
}wu_con_struct;

typedef struct{
    double demand;
    double consumption_fraction;
}wu_hist_struct;

typedef struct{
    double demand;
    double withdrawn;
    double consumed;    
    double returned;
}sector_var_struct;

typedef struct{
    sector_var_struct *sector;
    size_t last_run;
}wu_var_struct;

bool wu_get_global_parameters(char *cmdstr);
void wu_validate_global_parameters(void);
void wu_start(void);
void wu_alloc(void);
void initialize_wu_local_structures(void);
void wu_init(void);
void wu_set_output_meta_data_info(void);
void wu_set_state_meta_data_info(void);
bool wu_history(int, unsigned int *);
void wu_force(void);
void wu_run(size_t cur_cell);
void wu_put_data(void);
void wu_finalize(void);
void wu_add_types(void);

wu_var_struct *wu_var;
wu_hist_struct **wu_hist;
wu_con_struct *wu_con;

#endif