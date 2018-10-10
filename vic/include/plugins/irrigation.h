#ifndef IRRIGATION_H
#define IRRIGATION_H

#include <stdbool.h>

#define IRR_CAP_FRAC 0.7
#define IRR_CRIT_FRAC 0.99

#define POND_IRR_CRIT_FRAC 0.1
#define POND_CAPACITY 100
#define POND_KSAT_FRAC 0.05

typedef struct {
    size_t ni_types;
    size_t ni_active;

    int *vidx;
    int *iidx;
} irr_con_map_struct;

typedef struct {
    int veg_index;

    bool ponding;
    double pond_capacity;

    size_t nseasons;
    double *season_start;
    double *season_end;
    double season_offset;
} irr_con_struct;

typedef struct {
    double pond_storage;
    double leftover;
    
    double requirement;
    double shortage;
    double prev_req;
    double prev_short;

    double need;
    double deficit;
} irr_var_struct;

bool irr_get_global_parameters(char *cmdstr);
void irr_validate_global_parameters(void);
void irr_start(void);
void irr_validate_domain(void);
void irr_set_output_meta_data_info(void);
void irr_set_state_meta_data_info(void);
void irr_late_alloc(void);
void initialize_irr_local_structures(void);
void irr_init(void);
bool irr_history(int, unsigned int *);
void irr_run(size_t cur_cell);
void irr_set_demand(size_t cur_cell);
void irr_get_withdrawn(size_t cur_cell);
void irr_run_ponding_leftover(size_t cur_cell);
void irr_put_data(size_t);
void irr_finalize(void);
void irr_add_types(void);

irr_var_struct   ***irr_var;
irr_con_struct    **irr_con;
irr_con_map_struct *irr_con_map;

#endif /* IRRIGATION_H */
