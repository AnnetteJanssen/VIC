#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_irr_var(irr_var_struct *irr_var)
{
    irr_var->deficit = 0.0;
    irr_var->leftover = 0.0;
    irr_var->need = 0.0;
    irr_var->prev_req = 0.0;
    irr_var->prev_short = 0.0;
    irr_var->requirement = 0.0;
    irr_var->shortage = 0.0;
}

void
initialize_irr_con(irr_con_struct *irr_con)
{
    irr_con->paddy = false;
    irr_con->irr_class = MISSING_USI;
    irr_con->veg_class = MISSING_USI;
    irr_con->veg_index = MISSING_USI;
}

void
irr_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern irr_var_struct          ***irr_var;
    extern irr_con_struct           **irr_con;
    extern irr_con_map_struct        *irr_con_map;
    extern option_struct options;

    size_t                      i;
    size_t                      j;
    size_t                      k;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            initialize_irr_con(&irr_con[i][j]);
            for(k = 0; k < options.SNOW_BAND; k++){
                initialize_irr_var(&(irr_var[i][j][k]));
            }
        }
    }
}
