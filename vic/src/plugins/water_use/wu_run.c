#include <vic.h>

void
wu_run(size_t cur_cell)
{
    extern option_struct options;
    extern wu_hist_struct **wu_hist;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern rout_var_struct *rout_var;
    extern global_param_struct global_param;
    
    double withdrawn_fraction;
    
    double *withdrawn_local;
    double total_demand_local;
    double total_available_local;
    double total_withdrawn_local;
    double total_return_local;
    
    double *withdrawn_remote;
    double total_demand_remote;
    double total_available_remote;
    double total_withdrawn_remote;
    double total_return_remote;
    
    double *withdrawn_dam;
    double total_demand_dam;
    double total_available_dam;
    double total_withdrawn_dam;
    double total_return_dam;
    
    size_t rec_cell;
    size_t ser_cell;
    size_t ser_idx;
    
    size_t i;    
    size_t j;
    
    /**********************************************************************
    * 0. Allocation and Initialization
    **********************************************************************/
    // Allocate
    withdrawn_local = malloc(WU_NSECTORS * sizeof(*withdrawn_local));
    check_alloc_status(withdrawn_local, "Memory allocation error.");
    withdrawn_remote = malloc(WU_NSECTORS * sizeof(*withdrawn_remote));
    check_alloc_status(withdrawn_remote, "Memory allocation error.");   
    withdrawn_dam = malloc(WU_NSECTORS * sizeof(*withdrawn_dam));
    check_alloc_status(withdrawn_dam, "Memory allocation error.");
        
    // Initialize
    total_demand_local = 0.0;
    total_available_local = 0.0;
    total_withdrawn_local = 0.0;
    total_return_local = 0.0;
    total_demand_remote = 0.0;
    total_available_remote = 0.0;
    total_withdrawn_remote = 0.0;
    total_return_remote = 0.0;
    total_demand_dam = 0.0;
    total_available_dam = 0.0;
    total_withdrawn_dam = 0.0;
    total_return_dam = 0.0;
    
    for(i = 0; i < WU_NSECTORS; i++){
        withdrawn_local[i] = 0.0;
        withdrawn_remote[i] = 0.0;
        withdrawn_dam[i] = 0.0;
    }
    
    // Reset values
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].demand = wu_hist[cur_cell][i].demand;
        wu_var[cur_cell][i].withdrawn = 0.0;
        wu_var[cur_cell][i].consumed = 0.0;
        wu_var[cur_cell][i].returned = 0.0;
    }    
    
    /**********************************************************************
    * 1. Local abstractions
    **********************************************************************/
    // Get availability
    for(i = 0; i < options.RIRF_NSTEPS; i++){
        total_available_local += rout_var[cur_cell].discharge[i] * 
            global_param.dt;
    }
    
    // Get demand
    for(i = 0; i < WU_NSECTORS; i++){
        total_demand_local += wu_var[cur_cell][i].demand;
    }
    
    // Calculate withdrawal
    if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
        
        withdrawn_fraction = 0.0;
        if(total_available_local > 0 && total_demand_local > 0){
            withdrawn_fraction = total_available_local / total_demand_local;
            if(withdrawn_fraction > 1){
                withdrawn_fraction = 1.0;
            }

            for(i = 0; i < WU_NSECTORS; i++){
                withdrawn_local[i] = wu_var[cur_cell][i].demand * 
                        withdrawn_fraction;
                
                wu_var[cur_cell][i].demand *= (1 - withdrawn_fraction);
                
                total_withdrawn_local += withdrawn_local[i];
            }
        }
    } else {
        log_err("WU_STRATEGY PRIORITY has not been implemented yet");
    }
    
    /**********************************************************************
    * 2. Remote abstractions
    **********************************************************************/
    if(options.WU_REMOTE && 
            total_demand_local > total_withdrawn_local){
        
        // Get availability
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
                 
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                total_available_remote += rout_var[rec_cell].discharge[j] * 
                        global_param.dt;
            }
        }

        // Get demand
        for(i = 0; i < WU_NSECTORS; i++){
            total_demand_remote += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){

            withdrawn_fraction = 0.0;
            if(total_available_remote > 0 && total_demand_remote > 0){
                withdrawn_fraction = total_available_remote / total_demand_remote;
                if(withdrawn_fraction > 1){
                    withdrawn_fraction = 1.0;
                }

                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn_remote[i] = wu_var[cur_cell][i].demand * 
                            withdrawn_fraction;
                    
                    wu_var[cur_cell][i].demand *= (1 - withdrawn_fraction);

                    total_withdrawn_remote += withdrawn_remote[i];
                }
            }
        } else {
            log_err("WU_STRATEGY PRIORITY has not been implemented yet");
        }
    }
    
    /**********************************************************************
    * 3. Dam abstractions
    **********************************************************************/
    if(options.WU_REMOTE && options.DAMS && 
            ((total_demand_local > total_withdrawn_local &&
            total_demand_remote > total_withdrawn_remote) ||            
            (total_demand_local > total_withdrawn_local &&
            total_demand_remote == 0.0))){
        // Get availability
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
                             
            total_available_dam += dam_var[ser_cell][ser_idx].volume;
        }

        // Get demand
        for(i = 0; i < WU_NSECTORS; i++){
            total_demand_dam += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){

            withdrawn_fraction = 0.0;
            if(total_available_dam > 0 && total_demand_dam > 0){
                withdrawn_fraction = total_available_dam / total_demand_dam;
                if(withdrawn_fraction > 1){
                    withdrawn_fraction = 1.0;
                }

                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn_dam[i] = wu_var[cur_cell][i].demand * 
                            withdrawn_fraction;
                    
                    wu_var[cur_cell][i].demand *= (1 - withdrawn_fraction);

                    total_withdrawn_dam += withdrawn_dam[i];
                }
            }
        } else {
            log_err("WU_STRATEGY PRIORITY has not been implemented yet");
        }
    }
    
    /**********************************************************************
    * 4. Consumption and return
    **********************************************************************/    
    // Calculate consumption and return flow
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].withdrawn +=
                withdrawn_local[i] + withdrawn_remote[i] + withdrawn_dam[i];
        wu_var[cur_cell][i].consumed +=
                wu_var[cur_cell][i].withdrawn * 
                wu_hist[cur_cell][i].consumption_fraction;
        wu_var[cur_cell][i].returned +=
                wu_var[cur_cell][i].withdrawn * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
        
        total_return_local += withdrawn_local[i] * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
        total_return_remote += withdrawn_remote[i] * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
        total_return_dam += withdrawn_dam[i] * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
    }
    
    /**********************************************************************
    * 5. Actual withdrawal
    **********************************************************************/
    // Calculate reduction and return flow
    if(total_withdrawn_local > 0){
        for(i = 0; i < options.RIRF_NSTEPS; i++){
            rout_var[cur_cell].discharge[i] -=
                    (total_withdrawn_local - total_return_local - 
                    total_return_remote - total_return_dam) * 
                    ((rout_var[cur_cell].discharge[i] * global_param.dt) / 
                    total_available_local) / 
                    global_param.dt;

            if(rout_var[cur_cell].discharge[i] < 0){
                if(abs(rout_var[cur_cell].discharge[i]) > DBL_EPSILON){
                    log_err("Routing discharge reduction due to water use "
                            "is abnormally large [%.4f]", 
                            rout_var[cur_cell].discharge[i]);
                } else {
                    rout_var[cur_cell].discharge[i] = 0.0;
                }
            }    
        }
    }
    
    if(total_withdrawn_remote > 0){
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
            
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                rout_var[rec_cell].discharge[j] -=
                    (total_withdrawn_remote) * 
                    ((rout_var[rec_cell].discharge[j] * global_param.dt) / 
                        total_available_remote) / 
                    global_param.dt;

                if(rout_var[rec_cell].discharge[j] < 0){
                    if(abs(rout_var[rec_cell].discharge[j]) > DBL_EPSILON){
                        log_err("Remote routing discharge reduction due to water use "
                                "is abnormally large [%.4f]", 
                                rout_var[rec_cell].discharge[j]);
                    } else {
                        rout_var[rec_cell].discharge[j] = 0.0;
                    }
                }
            }
        }
    }
    
    if(total_withdrawn_dam > 0){
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
            
            dam_var[ser_cell][ser_idx].volume -=
                (total_withdrawn_dam) * 
                (dam_var[ser_cell][ser_idx].volume / total_available_dam);

            if(dam_var[ser_cell][ser_idx].volume < 0){
                if(abs(dam_var[ser_cell][ser_idx].volume) > DBL_EPSILON){
                    log_err("Dam volume reduction due to water use "
                            "is abnormally large [%.4f]", 
                            dam_var[ser_cell][ser_idx].volume);
                } else {
                    dam_var[ser_cell][ser_idx].volume = 0.0;
                }
            }
        }
    }
    
    /**********************************************************************
    * 6. Potential irrigation
    **********************************************************************/    
    if(options.IRR_POTENTIAL){
        wu_var[cur_cell][WU_IRRIGATION].withdrawn = 
                wu_var[cur_cell][WU_IRRIGATION].demand;
        wu_var[cur_cell][WU_IRRIGATION].consumed = 
                wu_var[cur_cell][WU_IRRIGATION].withdrawn * 
                wu_hist[cur_cell][WU_IRRIGATION].consumption_fraction;
        wu_var[cur_cell][WU_IRRIGATION].returned = 
                wu_var[cur_cell][WU_IRRIGATION].withdrawn * 
                (1 - wu_hist[cur_cell][WU_IRRIGATION].consumption_fraction);
    }
    
    
    /**********************************************************************
    * 7. Finalization
    **********************************************************************/  
    free(withdrawn_local);
    free(withdrawn_remote);
    free(withdrawn_dam);
}