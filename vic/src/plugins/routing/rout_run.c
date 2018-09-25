#include <vic.h>
void
rout_random_run()
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern rout_var_struct    *rout_var;
    extern rout_con_struct    *rout_con;
    extern rout_hist_struct   *rout_hist;
    extern double           ***out_data;
    extern size_t             *routing_order;
    extern int                 mpi_rank;

    size_t                    *nup_global;
    size_t                   **up_global;
    double                   **ruh_global;
    double                   **iuh_global;
    double                    *run_global;
    double                   **dt_dis_global;
    double                    *dis_global;
    double                    *hist_global;

    size_t                    *nup_local;
    size_t                   **up_local;
    double                   **ruh_local;
    double                   **iuh_local;
    double                    *run_local;
    double                   **dt_dis_local;
    double                    *dis_local;
    double                    *hist_local;

    size_t                     cur_cell;
    double                     inflow;
    double                     dt_inflow;
    double                     runoff;
    double                     dt_runoff;
    size_t                     rout_steps_per_dt;

    size_t                     i;
    size_t                     j;
        
    rout_steps_per_dt = global_param.rout_steps_per_day /
                          global_param.model_steps_per_day;

    // Alloc
    nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
    check_alloc_status(nup_global, "Memory allocation error");
    up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
    check_alloc_status(up_global, "Memory allocation error");
    ruh_global = malloc(global_domain.ncells_active * sizeof(*ruh_global));
    check_alloc_status(ruh_global, "Memory allocation error");
    iuh_global = malloc(global_domain.ncells_active * sizeof(*iuh_global));
    check_alloc_status(iuh_global, "Memory allocation error");
    run_global = malloc(global_domain.ncells_active * sizeof(*run_global));
    check_alloc_status(run_global, "Memory allocation error");
    dt_dis_global = malloc(global_domain.ncells_active * sizeof(*dt_dis_global));
    check_alloc_status(dt_dis_global, "Memory allocation error");
    dis_global = malloc(global_domain.ncells_active * sizeof(*dis_global));
    check_alloc_status(dis_global, "Memory allocation error");

    for (i = 0; i < global_domain.ncells_active; i++) {
        up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
        check_alloc_status(up_global[i], "Memory allocation error");
        ruh_global[i] = malloc(options.RUH_NSTEPS * sizeof(*ruh_global[i]));
        check_alloc_status(ruh_global[i], "Memory allocation error");
        iuh_global[i] = malloc(options.IUH_NSTEPS * sizeof(*iuh_global[i]));
        check_alloc_status(iuh_global[i], "Memory allocation error");
        dt_dis_global[i] = malloc((options.IUH_NSTEPS + rout_steps_per_dt) * sizeof(*dt_dis_global[i]));
        check_alloc_status(dt_dis_global[i], "Memory allocation error");
    }

    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    ruh_local = malloc(local_domain.ncells_active * sizeof(*ruh_local));
    check_alloc_status(ruh_local, "Memory allocation error");
    iuh_local = malloc(local_domain.ncells_active * sizeof(*iuh_local));
    check_alloc_status(iuh_local, "Memory allocation error");
    run_local = malloc(local_domain.ncells_active * sizeof(*run_local));
    check_alloc_status(run_local, "Memory allocation error");
    dt_dis_local = malloc(local_domain.ncells_active * sizeof(*dt_dis_local));
    check_alloc_status(dt_dis_local, "Memory allocation error");
    dis_local = malloc(local_domain.ncells_active * sizeof(*dis_local));
    check_alloc_status(dis_local, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
        ruh_local[i] = malloc(options.RUH_NSTEPS * sizeof(*ruh_local[i]));
        check_alloc_status(ruh_local[i], "Memory allocation error");
        iuh_local[i] = malloc(options.IUH_NSTEPS * sizeof(*iuh_local[i]));
        check_alloc_status(iuh_local[i], "Memory allocation error");
        dt_dis_local[i] = malloc((options.IUH_NSTEPS + rout_steps_per_dt) * sizeof(*dt_dis_local[i]));
        check_alloc_status(dt_dis_local[i], "Memory allocation error");
    }
    
    if (options.ROUTING_FORCE) {
        hist_global = malloc(global_domain.ncells_active * sizeof(*hist_global));
        check_alloc_status(hist_global, "Memory allocation error");
        hist_local = malloc(local_domain.ncells_active * sizeof(*hist_local));
        check_alloc_status(hist_local, "Memory allocation error");
    }

    // Get
    for (i = 0; i < local_domain.ncells_active; i++) {
        nup_local[i] = rout_con[i].Nupstream;
        for (j = 0; j < rout_con[i].Nupstream; j++) {
            up_local[i][j] = rout_con[i].upstream[j];
        }
        for (j = 0; j < options.RUH_NSTEPS; j++) {
            ruh_local[i][j] = rout_con[i].runoff_uh[j];
        }
        for (j = 0; j < options.IUH_NSTEPS; j++) {
            iuh_local[i][j] = rout_con[i].inflow_uh[j];
        }
        run_local[i] =
            (out_data[i][OUT_RUNOFF][0] +
            out_data[i][OUT_BASEFLOW][0]) *
            local_domain.locations[i].area /
            (global_param.dt * MM_PER_M);
        for (j = 0; j < options.IUH_NSTEPS + rout_steps_per_dt; j++) {
            dt_dis_local[i][j] = rout_var[i].dt_discharge[j];
        }
        
        dis_local[i] = rout_var[i].discharge;
        if (options.ROUTING_FORCE) {
            hist_local[i] = rout_hist[i].discharge;
        }
    }

    // Gather
    gather_sizet(nup_global, nup_local);
    gather_sizet_2d(up_global, up_local, MAX_UPSTREAM);
    gather_double_2d(ruh_global, ruh_local, options.RUH_NSTEPS);
    gather_double_2d(iuh_global, iuh_local, options.IUH_NSTEPS);
    gather_double(run_global, run_local);
    gather_double_2d(dt_dis_global, dt_dis_local, options.IUH_NSTEPS + rout_steps_per_dt);
    gather_double(dis_global, dis_local);
    if (options.ROUTING_FORCE) {
        gather_double(hist_global, hist_local);
    }

    // Calculate
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            cur_cell = routing_order[i];
            
            for(j = 0; j < rout_steps_per_dt; j++){
                dt_dis_global[cur_cell][0] = 0.0;
                cshift(dt_dis_global[cur_cell], options.IUH_NSTEPS + rout_steps_per_dt, 1, 0, 1);
            }

            inflow = 0;
            for (j = 0; j < nup_global[cur_cell]; j++) {
                inflow += dis_global[up_global[cur_cell][j]];
            }
            if (options.ROUTING_FORCE) {
                inflow += hist_global[cur_cell];
            }
            dt_inflow = inflow / rout_steps_per_dt;

            runoff = run_global[cur_cell];
            dt_runoff = runoff / rout_steps_per_dt;
            
            dis_global[cur_cell] = 0.0;
            for(j = 0; j < rout_steps_per_dt; j++){
                rout(dt_inflow, iuh_global[cur_cell], dt_dis_global[cur_cell],
                     options.IUH_NSTEPS, j);
                rout(dt_runoff, ruh_global[cur_cell], dt_dis_global[cur_cell],
                     options.RUH_NSTEPS, j);
                
                dis_global[cur_cell] += dt_dis_global[cur_cell][0];
                cshift(dt_dis_global[cur_cell], options.IUH_NSTEPS + rout_steps_per_dt, 1, 0, 1);
            }
            for(j = 0; j < rout_steps_per_dt; j++){
                cshift(dt_dis_global[cur_cell], 1, options.IUH_NSTEPS + rout_steps_per_dt, 1, -1);
            }
        }
    }

    // Scatter discharge
    scatter_double_2d(dt_dis_global, dt_dis_local, options.IUH_NSTEPS + rout_steps_per_dt);
    scatter_double(dis_global, dis_local);

    // Set discharge
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < options.IUH_NSTEPS + rout_steps_per_dt; j++) {
            rout_var[i].dt_discharge[j] = dt_dis_local[i][j];
        }
        rout_var[i].discharge = dis_local[i];
        
//        rout_var[i].moist = 0.0;
//        for(j = 0; j < options.IUH_NSTEPS + rout_steps_per_dt; j++){
//            rout_var[i].moist += rout_var[i].dt_discharge[j] *
//                                          global_param.dt /
//                                          local_domain.locations[i].area *
//                                          MM_PER_M;
//        }
    }

    // Free
    for (i = 0; i < global_domain.ncells_active; i++) {
        free(up_global[i]);
        free(ruh_global[i]);
        free(iuh_global[i]);
        free(dt_dis_global[i]);
    }
    free(nup_global);
    free(up_global);
    free(ruh_global);
    free(iuh_global);
    free(run_global);
    free(dt_dis_global);
    free(dis_global);

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
        free(ruh_local[i]);
        free(iuh_local[i]);
        free(dt_dis_local[i]);
    }
    free(nup_local);
    free(up_local);
    free(ruh_local);
    free(iuh_local);
    free(run_local);
    free(dt_dis_local);
    free(dis_local);
    
    if (options.ROUTING_FORCE){
        free(hist_global);
        free(hist_local);
    }
}

void
rout_basin_run(size_t cur_cell)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern rout_var_struct    *rout_var;
    extern rout_con_struct    *rout_con;
    extern rout_hist_struct    *rout_hist;
    extern double           ***out_data;

    double                     inflow;
    double                     dt_inflow;
    double                     runoff;
    double                     dt_runoff;
    size_t                     rout_steps_per_dt;

    size_t                     i;

    rout_steps_per_dt = global_param.rout_steps_per_day /
                          global_param.model_steps_per_day;
    
    for(i = 0; i < rout_steps_per_dt; i++){
        rout_var[cur_cell].dt_discharge[0] = 0.0;
        cshift(rout_var[cur_cell].dt_discharge, options.IUH_NSTEPS + rout_steps_per_dt, 1, 0, 1);
    }
    
    inflow = 0;
    for (i = 0; i < rout_con[cur_cell].Nupstream; i++) {
        inflow += rout_var[rout_con[cur_cell].upstream[i]].discharge;
    }
    if(options.ROUTING_FORCE){
        inflow += rout_hist[cur_cell].discharge;
    }
    dt_inflow = inflow / rout_steps_per_dt;

    runoff =
        (out_data[cur_cell][OUT_RUNOFF][0] +
         out_data[cur_cell][OUT_BASEFLOW][0]) *
        local_domain.locations[cur_cell].area /
        (global_param.dt * MM_PER_M);
    dt_runoff = runoff / rout_steps_per_dt;
    
    rout_var[cur_cell].discharge = 0;
    for(i = 0; i < rout_steps_per_dt; i++){
        rout(dt_inflow, rout_con[cur_cell].inflow_uh, rout_var[cur_cell].dt_discharge,
             options.IUH_NSTEPS, i);
        rout(dt_runoff, rout_con[cur_cell].runoff_uh, rout_var[cur_cell].dt_discharge,
             options.RUH_NSTEPS, i);
        
        rout_var[cur_cell].discharge += rout_var[cur_cell].dt_discharge[0];
        cshift(rout_var[cur_cell].dt_discharge, options.IUH_NSTEPS + rout_steps_per_dt, 1, 0, 1);
    }
    for(i = 0; i < rout_steps_per_dt; i++){
        cshift(rout_var[cur_cell].dt_discharge, 1, options.IUH_NSTEPS + rout_steps_per_dt, 1, -1);
    }
    
//    rout_var[cur_cell].moist = 0.0;
//    for(i = 0; i < options.IUH_NSTEPS + rout_steps_per_dt; i++){
//        rout_var[cur_cell].moist += rout_var[cur_cell].dt_discharge[i] *
//                                      global_param.dt /
//                                      local_domain.locations[cur_cell].area *
//                                      MM_PER_M;
//    }
}
