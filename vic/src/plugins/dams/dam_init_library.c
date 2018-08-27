#include <vic.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{
    size_t i;

    dam_var->volume = 0.0;
    dam_var->discharge = 0.0;
    dam_var->inflow = 0.0;
    dam_var->demand = 0.0;
    dam_var->total_flow = 0.0;
    dam_var->total_demand = 0.0;
    dam_var->total_steps = 0;
    dam_var->months_running = 0;

    for (i = 0; i < DAM_HIST_YEARS * MONTHS_PER_YEAR; i++) {
        dam_var->history_flow[i] = 0.0;
        dam_var->history_demand[i] = 0.0;
    }
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_volume[i] = 0.0;
        dam_var->op_discharge[i] = 0.0;
    }

    dam_var->op_year = 0;
}

void
initialize_dam_con(dam_con_struct *dam_con)
{
    size_t i;
    
    dam_con->id = 0;
    dam_con->year = 0;
    dam_con->function = DAM_FUN_OTH;
    dam_con->max_volume = 0;
    
    for (i = 0; i < dam_con->nservice; i++) {
        dam_con->service[i] = MISSING_USI;
        dam_con->serve_factor[i] = 0.0;
    }    
}

void
initialize_dam_local_structures(void)
{
    extern domain_struct       local_domain;
    extern dam_var_struct    **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
            initialize_dam_con(&dam_con[i][j]);
            initialize_dam_var(&dam_var[i][j]);
        }
    }
}
