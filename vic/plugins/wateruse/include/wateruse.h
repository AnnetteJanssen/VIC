#include <vic_driver_image.h>

#ifndef WATERUSE_H
#define WATERUSE_H

#define NODATA_WU -1

enum {
    WU_FROM_FILE,
    WU_CALCULATE,
    WU_SKIP,
    WU_NINPUT
};

enum {
    WU_IRRIGATION,
    WU_MUNICIPAL,
    WU_MANUFACTURING,
    WU_ENERGY,
    WU_LIVESTOCK,
    WU_NSECTORS
};

typedef struct {
    size_t ns_types;
    size_t ns_active;
    int *sidx;
} wu_con_map_struct;

typedef struct {
    size_t wu_sector;
    size_t sector_type_num;
    
    size_t nreceiving;
    size_t *receiving;
} wu_con_struct;

typedef struct {
    double *demand;
    double *consumption_frac;
    double *groundwater_frac;
} wu_force_struct;

typedef struct {
    double available_gw;
    double available_surf;
    double available_dam;
    double available_remote;
    double demand_gw;
    double demand_surf;
    double demand_remote;
    double withdrawn_gw;
    double withdrawn_surf;
    double withdrawn_dam;
    double withdrawn_remote;
    double returned;
} wu_var_struct;

bool wu_get_global_param(char *);
void wu_validate_global_param(void);

void wu_start(void);

void wu_alloc(void);
void wu_initialize_local_structures(void);

void wu_init(void);

void wu_set_output_met_data_info(void);
void wu_initialize_nc_file(nc_file_struct *);
void wu_add_hist_dim(nc_file_struct *, stream_struct *);
void wu_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void wu_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void wu_history(unsigned int, unsigned int *);
void wu_put_data(size_t);

void wu_forcing(void);
void wu_run(size_t);

void wu_finalize(void);

wu_con_map_struct *wu_con_map;
wu_con_struct **wu_con;
wu_force_struct **wu_force;
wu_var_struct **wu_var;

#endif /* WATERUSE_H */

