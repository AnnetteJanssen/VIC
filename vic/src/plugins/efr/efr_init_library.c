#include <vic.h>

#include "efr.h"

void
initialize_efr_force(efr_force_struct *efr_force)
{
    extern size_t NF;
    
    size_t i;
    
    for(i = 0; i < NF; i++){
        efr_force->ay_discharge[i] = 0.0;
        efr_force->ay_baseflow[i] = 0.0;
        efr_force->baseflow[i] = 0.0;
        efr_force->discharge[i] = 0.0;
    }
}

void
initialize_efr_hist(efr_hist_struct *efr_hist)
{
    efr_hist->ay_discharge = 0.0;
    efr_hist->ay_baseflow = 0.0;
    efr_hist->baseflow = 0.0;
    efr_hist->discharge = 0.0;
}

void
initialize_efr_var(efr_var_struct *efr_var, size_t nveg, size_t nelev)
{    
    size_t i;
    size_t j;
    
    for (i = 0; i < nveg; i++) {
        for (j = 0; j < nelev; j++) {
            efr_var->requirement_moist[i][j] = 0.0;
        }
    }
    
    efr_var->requirement_flow = 0.0;
}

void
initialize_efr_local_structures(void)
{
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern domain_struct   local_domain;
    extern efr_var_struct *efr_var;
    extern efr_force_struct *efr_force;
    extern efr_hist_struct *efr_hist;

    size_t                 i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_efr_var(&efr_var[i], veg_con_map[i].nv_active, elev_con_map[i].ne_active);
        initialize_efr_force(&efr_force[i]);
        initialize_efr_hist(&efr_hist[i]);
    }
}
