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
    WU_MUNICIPAL,
    WU_MANUFACTURING,
    WU_ENERGY,
    WU_IRRIGATION,
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
    double demand;
    double withdrawn;
    double returned;
} wu_var_struct;

wu_con_map_struct *wu_con_map;
wu_con_struct **wu_con;
wu_force_struct **wu_force;
wu_var_struct **wu_var;

#endif /* WATERUSE_H */

