#ifndef IRRIGATION_H
#define IRRIGATION_H

#include <stdbool.h>

#define FIELD_CAP_FRAC 0.7
#define IRRIGATION_FRAC 0.9

typedef struct{
    size_t ni_active;
    int *vidx;
}irr_con_map_struct;

typedef struct{
    bool ponding;
}irr_con_struct;

typedef struct{
    double pond_storage;
    
    double leftover;
    double prev_demand;
    double prev_root_moist;
}irr_var_struct;

bool irr_get_global_parameters(char *cmdstr);
void irr_validate_global_parameters(void);
void irr_start(void);
void irr_alloc(void);
void initialize_irr_local_structures(void);
void irr_init(void);
void irr_generate_default_state(void);
void irr_run1(size_t cur_cell);
void irr_run2(size_t cur_cell);
void irr_finalize(void);

#endif /* IRRIGATION_H */

