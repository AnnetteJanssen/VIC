#include <vic.h>

void
rout_alloc(void)
{
    extern domain_struct    global_domain;
    extern size_t           NF;
    extern domain_struct    local_domain;
    extern option_struct    options;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern rout_hist_struct *rout_hist;
    extern rout_force_struct *rout_force;
    extern size_t          *routing_order;
    extern global_param_struct global_param;
    
    size_t                     rout_steps_per_dt;

    size_t                     i;

    rout_steps_per_dt = global_param.rout_steps_per_day /
                          global_param.model_steps_per_day;

    rout_var = malloc(local_domain.ncells_active * sizeof(*rout_var));
    check_alloc_status(rout_var, "Memory allocation error");

    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con, "Memory allocation error");
    
    if (options.ROUTING_FORCE) {
        rout_hist = malloc(local_domain.ncells_active * sizeof(*rout_hist));
        check_alloc_status(rout_hist, "Memory allocation error");

        rout_force = malloc(local_domain.ncells_active * sizeof(*rout_force));
        check_alloc_status(rout_force, "Memory allocation error");
    }
    
    if (options.ROUTING_TYPE == ROUTING_BASIN) {
        routing_order =
            malloc(local_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }
    else if (options.ROUTING_TYPE == ROUTING_RANDOM) {
        routing_order =
            malloc(global_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].inflow_uh =
            malloc(options.IUH_NSTEPS * sizeof(*rout_con[i].inflow_uh));
        check_alloc_status(rout_con[i].inflow_uh, "Memory allocation error");

        rout_con[i].runoff_uh =
            malloc(options.RUH_NSTEPS * sizeof(*rout_con[i].runoff_uh));
        check_alloc_status(rout_con[i].runoff_uh, "Memory allocation error");

        rout_var[i].dt_discharge =
            malloc((options.IUH_NSTEPS + rout_steps_per_dt) * sizeof(*rout_var[i].dt_discharge));
        check_alloc_status(rout_var[i].dt_discharge, "Memory allocation error");
        
        if (options.ROUTING_FORCE) {
            rout_force[i].discharge = malloc(NF * sizeof(*rout_force[i].discharge));
            check_alloc_status(rout_force[i].discharge, "Memory allocation error");
        }
    }
}

void
rout_finalize(void)
{
    extern domain_struct    local_domain;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;
    extern rout_hist_struct *rout_hist;
    extern rout_force_struct *rout_force;
    extern option_struct    options;

    size_t                  i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(rout_con[i].inflow_uh);
        free(rout_con[i].runoff_uh);
        free(rout_con[i].upstream);
        free(rout_var[i].dt_discharge);
        
        if(options.ROUTING_FORCE){
            free(rout_force[i].discharge);
        }
    }
    free(rout_var);
    free(rout_con);
    free(routing_order);
    
    if(options.ROUTING_FORCE){
        free(rout_force);
        free(rout_hist);
    }
}
