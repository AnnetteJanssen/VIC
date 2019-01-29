#include <vic.h>

void
efr_alloc(void)
{
    extern domain_struct   local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern efr_var_struct *efr_var;
    extern efr_force_struct *efr_force;
    extern size_t NF;
    
    size_t i;
    size_t j;
    
    efr_var = malloc(local_domain.ncells_active * sizeof(*efr_var));
    check_alloc_status(efr_var, "Memory allocation error");
    
    efr_force = malloc(local_domain.ncells_active * sizeof(*efr_force));
    check_alloc_status(efr_force, "Memory allocation error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        
        efr_force[i].requirement_baseflow = malloc(NF * sizeof(*efr_force[i].requirement_baseflow));
        check_alloc_status(efr_force[i].requirement_baseflow, "Memory allocation error");
        
        efr_force[i].req_discharge = malloc(NF * sizeof(*efr_force[i].req_discharge));
        check_alloc_status(efr_force[i].req_discharge, "Memory allocation error");
        
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
    extern efr_force_struct *efr_force;

    size_t i;
    size_t j;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            free(efr_var[i].requirement_moist[j]);
        }
        free(efr_var[i].requirement_moist);
        free(efr_force[i].requirement_baseflow);
        free(efr_force[i].req_discharge);
    }
    free(efr_var);
    free(efr_force);
}
