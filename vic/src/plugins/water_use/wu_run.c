#include <vic.h>

void
wu_run(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern wu_hist_struct **wu_hist;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern rout_var_struct *rout_var;
    extern global_param_struct global_param;
    extern soil_con_struct *soil_con;
    extern all_vars_struct *all_vars;
    extern elev_con_map_struct *elev_con_map;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct **veg_con;
    
    double **moist;
    double resid_moist;
    double ice;    
    
    double fraction;
    double fraction2;
    double demand;
    double returned;
    
    bool satisfaction;
    bool satisfaction_local[2]; /* [0] surface water [1] groundwater */
    
    double available_local[2];  /* [0] surface water [1] groundwater */
    double available_subs;
    double available_remote;
    double available_dam;
    
    double withdrawn;
    double withdrawn_sec[WU_NSECTORS];
    double withdrawn_local[2];  /* [0] surface water [1] groundwater */
    double withdrawn_remote;
    double withdrawn_dam;
    
    cell_data_struct **cell;
    
    size_t rec_cell;
    size_t ser_cell;
    size_t ser_idx;
    
    size_t i;    
    size_t j;
    size_t k;
    size_t l;
    
    /**********************************************************************
    * 0. Allocation and Initialization
    **********************************************************************/
    // Allocate    
    if (options.WU_GW) {
        if(!options.GROUNDWATER){
            moist = malloc(veg_con_map[cur_cell].nv_active * sizeof(*moist));
            check_alloc_status(moist, "Memory allocation error.");
            for(i = 0; i < veg_con_map[cur_cell].nv_active; i++){
                moist[i] = malloc(elev_con_map[cur_cell].ne_active * sizeof(*moist[i]));
                check_alloc_status(moist[i], "Memory allocation error.");
            }
        }
    }
    
    // Initialize withdrawal since this determines the abstraction
    for(i = 0; i < WU_NSECTORS; i++){
        withdrawn_sec[i] = 0.0; 
    }
    for(i = 0; i < 2; i++){
        withdrawn_local[i] = 0.0;        
        satisfaction_local[i] = false;
    }    
    withdrawn_remote = 0.0;
    withdrawn_dam = 0.0;
    
    // Reset values
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].demand = wu_hist[cur_cell][i].demand;
        wu_var[cur_cell][i].withdrawn = 0.0;
        wu_var[cur_cell][i].consumed = 0.0;
        wu_var[cur_cell][i].returned = 0.0;
    }    
    
    cell = all_vars[cur_cell].cell;
    
    /**********************************************************************
    * 1. Local surface water abstractions
    **********************************************************************/
    // Get surface water availability
    available_local[0] = 0.0;
    for(i = 0; i < options.RIRF_NSTEPS; i++){
        available_local[0] += rout_var[cur_cell].discharge[i] * 
            global_param.dt;
    }
    
    // Get surface water demand
    demand = 0.0;
    for(i = 0; i < WU_NSECTORS; i++){
        demand += wu_hist[cur_cell][i].demand * 
                (1 - wu_hist[cur_cell][i].gw_fraction);
    }
    
    // Calculate surface water withdrawal
    if(available_local[0] > 0 && demand > 0){
        // Calculate fraction
        fraction = available_local[0] / demand;
        if(fraction >= 1){
            fraction = 1.0;
            satisfaction_local[0] = true;
        }

        for(i = 0; i < WU_NSECTORS; i++){
            withdrawn = wu_hist[cur_cell][i].demand *
                    (1 - wu_hist[cur_cell][i].gw_fraction) *
                    fraction;

            wu_var[cur_cell][i].demand -= withdrawn;            
            withdrawn_sec[i] += withdrawn;
            withdrawn_local[0] += withdrawn;
        }
    
        fraction = withdrawn_local[0] / available_local[0];
        if(fraction > 1){
            if(fabs(fraction - 1.0) > DBL_EPSILON){
                    log_err("fraction > 1.0 [%.3f]?", fraction);
            }
        }
    }
    
    /**********************************************************************
    * 2. Local groundwater abstractions
    **********************************************************************/    
    // Get groundwater availability
    if (options.WU_GW) {
        available_local[1] = 0.0;
    
        if(options.GROUNDWATER){
            log_err("GROUNDWATER WATER USE is not yet implemented for GROUNDWATER");
        }
        else {
            l = options.Nlayer - 1;
            resid_moist = soil_con[cur_cell].resid_moist[l] * 
                          soil_con[cur_cell].depth[l] * 
                          MM_PER_M;

            for(i = 0; i < veg_con_map[cur_cell].nv_active; i++){
                for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                    moist[i][j] = cell[i][j].layer[l].moist;

                    if(moist[i][j] > resid_moist){
                        ice = 0.0;            
                        for(k = 0; k < options.Nfrost; k++){
                            ice += cell[i][j].layer[l].ice[k] *
                                   soil_con[cur_cell].frost_fract[k];
                        }

                        moist[i][j] = min(moist[i][j] - ice, 
                                          moist[i][j] - resid_moist);
                    }
                    else {
                        moist[i][j] = 0.0;
                    }

                    available_local[1] += moist[i][j]  * 
                                          soil_con[cur_cell].AreaFract[j] * 
                                          veg_con[cur_cell][i].Cv;
                }
            }
            
            available_local[1] = available_local[1] / MM_PER_M * 
                    local_domain.locations[i].area;
        }
    
        // Get groundwater demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_hist[cur_cell][i].demand * 
                    wu_hist[cur_cell][i].gw_fraction;
        }

        // Calculate groundwater withdrawal
        if(available_local[1] > 0 && demand > 0){
            // Calculate fraction
            fraction = available_local[1] / demand;
            if(fraction >= 1){
                fraction = 1.0;
                satisfaction_local[1] = true;
            }

            for(i = 0; i < WU_NSECTORS; i++){
                withdrawn = wu_hist[cur_cell][i].demand *
                        wu_hist[cur_cell][i].gw_fraction *
                        fraction;

                wu_var[cur_cell][i].demand -= withdrawn;
                withdrawn_sec[i] += withdrawn;
                withdrawn_local[1] += withdrawn;
            }

            fraction = withdrawn_local[1] / available_local[1];
            if(fraction > 1){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                        log_err("fraction > 1.0 [%.3f]?", fraction);
                }
            }
        }
    }
    
    // Total satisfaction
    satisfaction = false;
    if(satisfaction_local[0] && satisfaction_local[1]){
        satisfaction = true;
    }
    
    /**********************************************************************
    * 3. Remote abstractions
    **********************************************************************/
    if(options.WU_REMOTE && !satisfaction){
        
        // Get availability
        available_remote = 0.0;
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
                 
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                available_remote += rout_var[rec_cell].discharge[j] * 
                        global_param.dt;
            }
        }

        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(available_remote > 0 && demand > 0){
            if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
                
                // Calculate fraction
                fraction = available_remote / demand;
                if(fraction >= 1){
                    fraction = 1.0;
                    satisfaction = true;
                }
                
                // Calculate withdrawal
                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn = wu_var[cur_cell][i].demand * fraction;
                    
                    wu_var[cur_cell][i].demand -= withdrawn;
                    withdrawn_sec[i] += withdrawn;         
                    withdrawn_remote += withdrawn;
                }
            } else {
                log_err("WU_STRATEGY PRIORITY has not been implemented yet");
            }
    
            fraction = withdrawn_remote / available_remote;
            if(fraction > 1.0){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                        log_err("fraction > 1.0 [%.3f]?", fraction);
                }
            }
        }
    }
    
    /**********************************************************************
    * 4. Substitute abstractions
    **********************************************************************/
    if(options.WU_GW && !satisfaction){ 
        
        // Get availability
        available_subs = (available_local[0] - withdrawn_local[0])  + 
                (available_local[1] - withdrawn_local[1]);
        
        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(available_subs > 0 && demand > 0){
            if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
                
                // Calculate fraction
                fraction = available_subs / demand;
                if(fraction >= 1){
                    fraction = 1.0;
                    satisfaction = true;
                }
                
                fraction2 = (available_local[0] - withdrawn_local[0]) /
                             available_subs;
                
                // Calculate withdrawal
                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn = wu_var[cur_cell][i].demand * fraction;
                    
                    wu_var[cur_cell][i].demand -= withdrawn;
                    withdrawn_sec[i] += withdrawn;
                    
                    // Dived substitute irrigation over surface- and groundwater
                    withdrawn_local[0] += withdrawn * fraction2;                 
                    withdrawn_local[1] += withdrawn * (1 - fraction2);
                }
            } else {
                log_err("WU_STRATEGY PRIORITY has not been implemented yet");
            }
    
            fraction = withdrawn_local[0] / available_local[0];
            if(fraction > 1){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                        log_err("fraction > 1.0 [%.3f]?", fraction);
                }
            }    
            fraction = withdrawn_local[1] / available_local[1];
            if(fraction > 1){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                        log_err("fraction > 1.0 [%.3f]?", fraction);
                }
            }
        }
    }
    
    /**********************************************************************
    * 5. Dam abstractions
    **********************************************************************/
    if(options.WU_DAM && !satisfaction){
        
        // Get availability
        available_dam = 0.0;
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
                             
            available_dam += dam_var[ser_cell][ser_idx].volume;
        }

        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(available_dam > 0 && demand > 0){
            if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
                
                // Calculate fraction
                fraction = available_dam / demand;
                if(fraction >= 1){
                    fraction = 1.0;
                }
                
                // Calculate withdrawal
                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn = wu_var[cur_cell][i].demand * fraction;
                    
                    wu_var[cur_cell][i].demand -= withdrawn;
                    withdrawn_sec[i] += withdrawn;
                    withdrawn_dam += withdrawn;
                }
            } else {
                log_err("WU_STRATEGY PRIORITY has not been implemented yet");
            }
    
            fraction = withdrawn_dam / available_dam;
            if(fraction > 1){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                        log_err("fraction > 1.0 [%.3f]?", fraction);
                }
            }
        }
    }
    
    /**********************************************************************
    * 4. Consumption and return
    **********************************************************************/    
    // Calculate consumption and return flow
    returned = 0.0;
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].withdrawn = withdrawn_sec[i];
        wu_var[cur_cell][i].consumed = wu_var[cur_cell][i].withdrawn * 
                wu_hist[cur_cell][i].consumption_fraction;
        wu_var[cur_cell][i].returned = wu_var[cur_cell][i].withdrawn * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
        
        returned += wu_var[cur_cell][i].returned;
    }
    
    /**********************************************************************
    * 5. Actual withdrawal
    **********************************************************************/
    // Calculate reduction and return flow        
    if (withdrawn_local[0] > 0.0) {        
        // Calculate fraction
        fraction = (withdrawn_local[0] - returned) / available_local[0];
        if(fraction > 1.0){
            fraction = 1.0;
        }

        // Modify discharge
        for(i = 0; i < options.RIRF_NSTEPS; i++){
            rout_var[cur_cell].discharge[i] *= 1 - fraction;
        }
    }
        
    if (withdrawn_local[1] > 0.0) {        
        // Calculate fraction
        fraction = withdrawn_local[1] / available_local[1];
        if(fraction > 1.0){
            fraction = 1.0;
        }
        for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
            for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                moist[i][j] *= fraction;                    
            }
        }

        // Modify groundwater
        if (options.GROUNDWATER) {
            log_err("GROUNDWATER WATER USE is not yet implemented for GROUNDWATER");
        }
        else {
            for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
                for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                    // Calculate fraction
                    fraction = (cell[i][j].layer[l].moist - moist[i][j]) / 
                            resid_moist;                        
                    if(fraction < 1.0 && moist[i][j] > 0){
                        if(fabs(fraction - 1.0) > DBL_EPSILON){
                            log_err("fraction < 1.0 [%.3f]?", fraction);
                        }
                        fraction = 1.0;
                    }

                    cell[i][j].layer[l].moist = resid_moist * fraction;
                }    
            }
        }
    }
    
    if(withdrawn_remote > 0){        
        // Calculate fraction
        fraction = withdrawn_remote / available_remote;
        if(fraction > 1.0){
            fraction = 1.0;
        }
        
        // Modify discharge
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
            
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                rout_var[rec_cell].discharge[j] *= 1 - fraction;
            }
        }
    }
    
    if(withdrawn_dam > 0){        
        // Calculate fraction
        fraction = withdrawn_dam / available_dam;
        if(fraction > 1.0){
            fraction = 1.0;
        }
        
        // Modify volume
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
            
            dam_var[ser_cell][ser_idx].volume *= 1 - fraction;
        }
    }
    
    /**********************************************************************
    * 6. Finalize
    **********************************************************************/
    if (options.WU_GW) {
        if (!options.GROUNDWATER) {
            for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
                free(moist[i]);
            }
            free(moist);
        }
    }
}