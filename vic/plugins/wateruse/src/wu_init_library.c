#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_wu_force(wu_force_struct *wu_force)
{
    extern size_t NF;

    size_t               i;

    for (i = 0; i < NF; i++) {
        wu_force->consumption_frac[i] = 0.0;
        wu_force->groundwater_frac[i] = 0.0;
        wu_force->demand[i] = 0.0;
    }
}

void
initialize_wu_var(wu_var_struct *wu_var)
{
    wu_var->available_gw = 0.0;
    wu_var->available_surf = 0.0;
    wu_var->available_dam = 0.0;
    wu_var->available_remote = 0.0;
    wu_var->demand_gw = 0.0;
    wu_var->demand_surf = 0.0;
    wu_var->demand_remote = 0.0;
    wu_var->withdrawn_gw = 0.0;
    wu_var->withdrawn_surf = 0.0;
    wu_var->withdrawn_dam = 0.0;
    wu_var->withdrawn_remote = 0.0;
    wu_var->returned = 0.0;
}

void
initialize_wu_con(wu_con_struct *wu_con)
{
    size_t               i;

    for(i = 0; i < wu_con->nreceiving; i++){
        wu_con->receiving[i] = MISSING_USI;
    }
}

void
wu_initialize_local_structures(void)
{
    extern domain_struct    local_domain;
    extern plugin_option_struct plugin_options;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    extern wu_force_struct **wu_force;

    size_t                  i;
    size_t                  j;
    int                  iSector;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NWUTYPES; j++){
            iSector = wu_con_map[i].sidx[j];
            if(iSector != NODATA_WU){
                initialize_wu_con(&wu_con[i][iSector]);
                initialize_wu_var(&(wu_var[i][iSector]));
                initialize_wu_force(&(wu_force[i][iSector]));
            }
        }
    }
}
