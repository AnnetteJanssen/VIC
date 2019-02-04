#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{
    extern plugin_option_struct plugin_options;
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;

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
    
    dam_var->op_year = 0;
    dam_var->months_running = 0;
}

void
initialize_dam_con(dam_con_struct *dam_con)
{
    extern plugin_option_struct plugin_options;

    size_t               i;

    dam_con->downstream = MISSING_USI;
    dam_con->Nupstream = 0;
    dam_con->upstream = NULL;

    for (i = 0; i < plugin_options.UH_LENGTH; i++) {
        dam_con->inflow_uh[i] = 0.0;
        dam_con->runoff_uh[i] = 0.0;
    }
}

void
dam_initialize_local_structures(void)
{
    extern domain_struct    local_domain;
    extern dam_var_struct *dam_var;
    extern dam_con_struct *dam_con;
    extern dam_force_struct *dam_force;
    extern plugin_option_struct plugin_options;

    size_t                  i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_dam_con(&dam_con[i]);
        initialize_dam_var(&(dam_var[i]));
    }
        
    if(plugin_options.FORCE_ROUTING){
        for (i = 0; i < local_domain.ncells_active; i++) {
            initialize_dam_force(&(dam_force[i]));
        }
    }
}
