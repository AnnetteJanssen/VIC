#include <vic.h>

void
efr_alloc(void)
{
    extern domain_struct   local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern efr_var_struct *efr_var;
    
    size_t i;
    size_t j;
    
    efr_var = malloc(local_domain.ncells_active * sizeof(*efr_var));
    check_alloc_status(efr_var, "Memory allocation error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        
        efr_var[i].requirement_moist = malloc(veg_con_map[i].nv_active * sizeof(*efr_var[i].requirement_moist));
        check_alloc_status(efr_var[i].requirement_moist, "Memory allocation error");
        
        for (j = 0; j < veg_con_map[i].nv_active; j++) {

            efr_var[i].requirement_moist[j] = malloc(elev_con_map[i].ne_active * sizeof(*efr_var[i].requirement_moist[j]));
            check_alloc_status(efr_var[i].requirement_moist[j], "Memory allocation error");
        }
    }
}

void
efr_finalize(void)
{
    extern domain_struct   local_domain;
    extern efr_var_struct *efr_var;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;

    size_t i;
    size_t j;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            free(efr_var[i].requirement_moist[j]);
        }
        free(efr_var[i].requirement_moist);
    }
    free(efr_var);
}
