#ifndef EFR_H
#define EFR_H

#define EFR_HIST_YEARS 1
#define EFR_FRAC_STEP 0.05
#define VFM_LOW_FLOW_FRAC 0.4
#define VFM_LOW_DEMAND_FRAC 0.6
#define VFM_HIGH_FLOW_FRAC 0.8
#define VFM_HIGH_DEMAND_FRAC 0.3

typedef struct {
    double *req_discharge;
    double *requirement_baseflow;
} efr_force_struct;

typedef struct {
    double **requirement_moist;
} efr_var_struct;

bool efr_get_global_parameters(char *cmdstr);
void efr_validate_global_param(void);
void efr_validate_domain(void);
void efr_set_output_meta_data_info(void);
void efr_set_state_meta_data_info(void);
void efr_alloc(void);
void initialize_efr_local_structures(void);
void efr_forcing(void);
void efr_run(size_t cur_cell);
void efr_put_data(size_t);
void efr_finalize(void);
void efr_add_types(void);

efr_var_struct *efr_var;
efr_force_struct *efr_force;

#endif /* EFR_H */
