#ifndef EFR_H
#define EFR_H

typedef struct {
    double *discharge;
    double *baseflow;
    double **moist;
} efr_force_struct;


bool efr_get_global_param(char *);
void efr_validate_global_param(void);

void efr_start(void);
void efr_alloc(void);
void efr_initialize_local_structures(void);

void efr_set_output_met_data_info(void);
void efr_history(int, unsigned int *);

void efr_forcing(void);
void efr_put_data(size_t);
void efr_finalize(void);

efr_force_struct *efr_force;

#endif

