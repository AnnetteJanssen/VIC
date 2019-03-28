#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_efr_force(efr_force_struct *efr_force, veg_con_map_struct *veg_con_map)
{
    extern option_struct options;
    extern size_t NF;

    size_t        i;
    size_t        j;

    for (i = 0; i < NF; i++) {
        efr_force->discharge[i] = 0.0;
        efr_force->baseflow[i] = 0.0;
    }
    
    for (i = 0; i < veg_con_map->nv_active; i++) {
        for(j = 0; j < options.SNOW_BAND; j++){
            efr_force->moist[i][j] = 0.0;
        }
    }
}

void
efr_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern efr_force_struct   *efr_force;
    extern veg_con_map_struct *veg_con_map;

    size_t                      i;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_efr_force(&(efr_force[i]), &(veg_con_map[i]));
    }
}
