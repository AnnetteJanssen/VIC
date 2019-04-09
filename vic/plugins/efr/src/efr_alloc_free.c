#include <vic_driver_image.h>
#include <plugin.h>

void
efr_alloc(void)
{
    extern domain_struct              local_domain;
    extern option_struct options;
    extern size_t                     NF;
    extern veg_con_map_struct       *veg_con_map;
    extern efr_force_struct         *efr_force;

    size_t                            i;
    size_t                            j;
    
    efr_force = malloc(local_domain.ncells_active * sizeof(*efr_force));
    check_alloc_status(efr_force, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        efr_force[i].discharge =
            malloc(NF * sizeof(*efr_force[i].discharge));
        check_alloc_status(efr_force[i].discharge,
                           "Memory allocation error");
        
        efr_force[i].baseflow =
            malloc(NF * sizeof(*efr_force[i].discharge));
        check_alloc_status(efr_force[i].discharge,
                           "Memory allocation error");
        
        efr_force[i].moist =
            malloc(veg_con_map[i].nv_active * sizeof(*efr_force[i].moist));
        check_alloc_status(efr_force[i].moist,
                           "Memory allocation error");
        
        for(j = 0; j < veg_con_map[i].nv_active; j++){
            efr_force[i].moist[j] =
                malloc(options.SNOW_BAND * sizeof(*efr_force[i].moist[j]));
            check_alloc_status(efr_force[i].moist[j],
                               "Memory allocation error");
        }
    }
    
    efr_initialize_local_structures();
}

void
efr_finalize(void)
{
    extern domain_struct        local_domain;
    extern veg_con_map_struct       *veg_con_map;
    extern efr_force_struct   *efr_force;

    size_t                      i;
    size_t                      j;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < veg_con_map[i].nv_active; j++){
            free(efr_force[i].moist[j]);
        }
        free(efr_force[i].discharge);
        free(efr_force[i].baseflow);
        free(efr_force[i].moist);
    }
    free(efr_force);
}
