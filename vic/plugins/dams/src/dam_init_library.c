#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{
    size_t               i;
    
    dam_var->active = false;
    dam_var->inflow = 0.0;
    dam_var->demand = 0.0;
    dam_var->efr = 0.0;
    dam_var->release = 0.0;
    dam_var->storage = 0.0;
    
    dam_var->total_inflow = 0.0;
    dam_var->demand = 0.0;
    dam_var->efr = 0.0;
    
    for(i = 0; i < MONTHS_PER_YEAR * DAM_HIST_YEARS; i++) {
        dam_var->history_inflow[i] = 0.0;
        dam_var->history_demand[i] = 0.0;
        dam_var->history_efr[i] = 0.0;
    }
    for(i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_release[i] = 0.0;
        dam_var->op_storage[i] = 0.0;
    }
    
    dam_var->op_year = 0;
    dam_var->months_running = 0;
}

void
initialize_dam_con(dam_con_struct *dam_con)
{
    size_t i;
    
    dam_con->year = 0;
    dam_con->capacity = 0;
    dam_con->inflow_frac = 0.0;
    
    for (i = 0; i < dam_con->nservice; i++) {
        dam_con->service[i] = MISSING_USI;
        dam_con->service_frac[i] = 0.0;
    }    
}

void
dam_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_con_struct     **local_dam_con;
    extern dam_var_struct     **local_dam_var;
    extern dam_con_map_struct  *global_dam_con_map;
    extern dam_con_struct     **global_dam_con;
    extern dam_var_struct     **global_dam_var;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NDAMTYPES; j++){
            if(local_dam_con_map[i].didx[j] != NODATA_DAM){
                initialize_dam_con(&local_dam_con[i][j]);
                initialize_dam_var(&local_dam_var[i][j]);
            }
            if(global_dam_con_map[i].didx[j] != NODATA_DAM){
                initialize_dam_con(&global_dam_con[i][j]);
                initialize_dam_var(&global_dam_var[i][j]);
            }
        }
    }
}
