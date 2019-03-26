#ifndef IRRIGATION_H
#define IRRIGATION_H

typedef struct {
    size_t ni_types;
    size_t ni_active;

    int *vidx;
    int *iidx;
} irr_con_map_struct;

typedef struct {
    int irr_class;
    int veg_class;
    int veg_index;

    bool paddy;
} irr_con_struct;

typedef struct {
    double leftover;
    
    double requirement;
    double shortage;
    double prev_req;
    double prev_short;

    double need;
    double deficit;
} irr_var_struct;

bool irr_get_global_param(char *);
void irr_validate_global_param(void);
bool irr_get_parameters(char *);
void irr_validate_parameters(void);

void irr_mpi_map_decomp_domain(size_t, size_t, int **, int **, size_t **);

void irr_start(void);
void irr_late_alloc(void);
void irr_initialize_local_structures(void);
void irr_init(void);

void irr_set_output_met_data_info(void);
void irr_history(int, unsigned int *);
void irr_put_data(size_t);
void irr_run_requirement(size_t);
void irr_run_shortage(size_t);
void irr_set_demand(size_t);
void irr_get_withdrawn(size_t);
void irr_finalize(void);

irr_var_struct   ***irr_var;
irr_con_struct    **irr_con;
irr_con_map_struct *irr_con_map;

#endif

