#include <vic.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{
    size_t i;

    dam_var->active = false;
    
    dam_var->inflow = 0.0;
    dam_var->demand = 0.0;
    dam_var->efr = 0.0;
    
    dam_var->storage = 0.0;
    dam_var->release = 0.0;
    
    dam_var->total_inflow = 0.0;
    dam_var->total_demand = 0.0;
    dam_var->total_efr = 0.0;

    for (i = 0; i < DAM_HIST_YEARS * MONTHS_PER_YEAR; i++) {
        dam_var->history_inflow[i] = 0.0;
        dam_var->history_demand[i] = 0.0;
        dam_var->history_efr[i] = 0.0;
    }
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_storage[i] = 0.0;
        dam_var->op_release[i] = 0.0;
    }

    dam_var->op_year = 0;
    dam_var->months_running = 0;
}

void
initialize_dam_con(dam_con_struct *dam_con)
{
    size_t i;
    
    dam_con->run = false;
    
    dam_con->year = 0;
    dam_con->capacity = 0;
    dam_con->inflow_frac = 0.0;
    
    for (i = 0; i < dam_con->nservice; i++) {
        dam_con->service[i] = MISSING_USI;
        dam_con->service_frac[i] = 0.0;
    }    
}

void
initialize_dam_local_structures(void)
{
    extern domain_struct       local_domain;
    extern dam_con_struct     *local_dam_con;
    extern dam_var_struct     *local_dam_var;
    extern dam_con_struct     *global_dam_con;
    extern dam_var_struct     *global_dam_var;

    size_t                     i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_dam_con(&local_dam_con[i]);
        initialize_dam_var(&local_dam_var[i]);
        initialize_dam_con(&global_dam_con[i]);
        initialize_dam_var(&global_dam_var[i]);
    }
}
