#ifndef WATER_USE_H
#define WATER_USE_H

#include <stdbool.h>

enum {
    WU_INPUT_FROM_FILE,
    WU_INPUT_CALCULATE,
    WU_INPUT_NONE,
    WU_INPUT_NLOCATIONS
};

enum {
    WU_IRRIGATION,
    WU_NSECTORS
};

typedef struct {    
    size_t nreceiving;
    size_t *receiving;
    
    size_t nservice;
    size_t *service;
    size_t *service_idx;
    
    double cons_fraction[WU_NSECTORS];
    double gw_fraction[WU_NSECTORS];
} wu_con_struct;

typedef struct {
    double *demand;
} wu_force_struct;

typedef struct {
    double demand;
    double withdrawn;
    double returned;
} wu_var_struct;

bool wu_get_global_parameters(char *cmdstr);
void wu_validate_global_param(void);
void wu_start(void);
void wu_validate_domain(void);
void wu_alloc(void);
void wu_late_alloc(void);
void initialize_wu_local_structures(void);
void wu_init(void);
void wu_set_output_meta_data_info(void);
void wu_set_state_meta_data_info(void);
bool wu_history(int, unsigned int *);
void wu_forcing(void);
void wu_run(size_t cur_cell);
void wu_put_data(size_t);
void wu_finalize(void);
void wu_add_types(void);

wu_var_struct   **wu_var;
wu_force_struct **wu_force;
wu_con_struct    *wu_con;

#endif