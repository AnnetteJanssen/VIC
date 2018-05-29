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
    extern efr_var_struct *efr_var;
    extern efr_hist_struct *efr_hist;
    
    double liq;
    double moist;
    double resid_moist;
    double ice;    
    
    double fraction;
    double fraction2;
    double demand;
    double returned;
    
    bool satisfaction;
    bool satisfaction_local[2];  /* [0] surface water [1] groundwater */
    
    double **available_moist;    /* Per vegetation type, per elevation band */
    double available_local[2];   /* [0] surface water [1] groundwater */
    double available_subs;
    double *available_receiving; /* Per receiving cell */
    double available_remote;
    double *available_servicing; /* Per servicing dam */
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
            available_moist = malloc(veg_con_map[cur_cell].nv_active * sizeof(*available_moist));
            check_alloc_status(available_moist, "Memory allocation error.");
            for(i = 0; i < veg_con_map[cur_cell].nv_active; i++){
                available_moist[i] = malloc(elev_con_map[cur_cell].ne_active * sizeof(*available_moist[i]));
                check_alloc_status(available_moist[i], "Memory allocation error.");
            }
        }
    }
    if (options.WU_REMOTE) {
        available_receiving = malloc(wu_con[cur_cell].nreceiving * sizeof(*available_receiving));
        check_alloc_status(available_receiving, "Memory allocation error.");
    }
    if (options.WU_DAM) {
        available_servicing = malloc(wu_con[cur_cell].nservice * sizeof(*available_servicing));
        check_alloc_status(available_servicing, "Memory allocation error.");
    }
    
    // Initialize withdrawal since this determines the abstraction
    for(i = 0; i < WU_NSECTORS; i++){
        withdrawn_sec[i] = 0.0; 
    }
    for(i = 0; i < 2; i++){
        withdrawn_local[i] = 0.0;      
    }    
    withdrawn_remote = 0.0;
    withdrawn_dam = 0.0;
    
    // Initialize satisfaction
    for(i = 0; i < 2; i++){      
        satisfaction_local[i] = false;
    }
    satisfaction = false;
    
    // Initialize groundwater
    if (!options.WU_GW) {
        for(i = 0; i < WU_NSECTORS; i++){
            wu_hist[cur_cell][i].gw_fraction = 0.0;
        }
    }
    
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
    // Get surface water demand
    demand = 0.0;
    for(i = 0; i < WU_NSECTORS; i++){
        demand += wu_hist[cur_cell][i].demand * 
                (1 - wu_hist[cur_cell][i].gw_fraction);
    }
    
    // Get surface water availability
    available_local[0] = 0.0;
    if (demand > 0) {
        available_local[0] = rout_var[cur_cell].discharge[0] * global_param.dt; 

        if(options.EFR){
            available_local[0] -= efr_hist[cur_cell].requirement_discharge 
                    * global_param.dt;
            if(available_local[0] < 0.0){
                available_local[0] = 0.0;
            }
        }
    }
    
    // Calculate surface water withdrawal
    if(available_local[0] > 0){
        if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
            // Calculate fraction
            fraction = available_local[0] / demand;
            if(fraction >= 1){
                fraction = 1.0;
                satisfaction_local[0] = true;
            }

            // Withdrawal as fraction of demand
            for(i = 0; i < WU_NSECTORS; i++){
                withdrawn = wu_hist[cur_cell][i].demand *
                        (1 - wu_hist[cur_cell][i].gw_fraction) *
                        fraction;

                wu_var[cur_cell][i].demand -= withdrawn;            
                withdrawn_sec[i] += withdrawn;
                withdrawn_local[0] += withdrawn;
            }
        }
        else {
            log_err("WU_STRATEGY PRIORITY has not been implemented yet");
        }

        // Calculate fraction
        fraction = withdrawn_local[0] / available_local[0];
        if(fraction > 1.0){
            if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS){
                log_err("fraction > 1.0 [%.16f]?", fraction);
            }
            withdrawn_local[0] = available_local[0];
        }
    }
    
    /**********************************************************************
    * 2. Local groundwater abstractions
    **********************************************************************/
    // Get groundwater demand
    demand = 0.0;
    for(i = 0; i < WU_NSECTORS; i++){
        demand += wu_hist[cur_cell][i].demand * 
                wu_hist[cur_cell][i].gw_fraction;
    }
    
    // Get groundwater availability
    available_local[1] = 0.0;
    if (demand > 0) {
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
                    moist = cell[i][j].layer[l].moist;

                    ice = 0.0;            
                    for(k = 0; k < options.Nfrost; k++){
                        ice += cell[i][j].layer[l].ice[k] *
                               soil_con[cur_cell].frost_fract[k];
                    }
                    liq = moist - ice;

                    available_moist[i][j] = liq;                    
                    if(options.EFR){
                        available_moist[i][j] -= efr_var[cur_cell].requirement_moist[i][j];
                    }

                    available_moist[i][j] = min(available_moist[i][j], moist - resid_moist);
                    if(available_moist[i][j] < 0.0){
                        available_moist[i][j] = 0.0;
                    }

                    available_moist[i][j] *= (local_domain.locations[i].area * 
                                        soil_con[cur_cell].AreaFract[j] * 
                                        veg_con[cur_cell][i].Cv) / 
                                        MM_PER_M;                    
                    available_local[1] += available_moist[i][j];
                }
            }
        }
    }

    // Calculate groundwater withdrawal
    if(available_local[1] > 0){
        if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
            // Calculate fraction
            fraction = available_local[1] / demand;
            if(fraction >= 1){
                fraction = 1.0;
                satisfaction_local[1] = true;
            }

            // Withdrawal as fraction of demand
            for(i = 0; i < WU_NSECTORS; i++){
                withdrawn = wu_hist[cur_cell][i].demand *
                        wu_hist[cur_cell][i].gw_fraction *
                        fraction;

                wu_var[cur_cell][i].demand -= withdrawn;
                withdrawn_sec[i] += withdrawn;
                withdrawn_local[1] += withdrawn;
            }
        }
        else {
            log_err("WU_STRATEGY PRIORITY has not been implemented yet");
        }

        fraction = withdrawn_local[1] / available_local[1];
        if(fraction > 1.0){
            if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS){
                log_err("fraction > 1.0 [%.16f]?", fraction);
            }
            withdrawn_local[1] = available_local[1];
        }
    }
    
    // Local satisfaction
    satisfaction = false;
    if(satisfaction_local[0] && satisfaction_local[1]){
        satisfaction = true;
    }
    
    /**********************************************************************
    * 3. Remote abstractions
    **********************************************************************/
    if(options.WU_REMOTE && !satisfaction){
        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
        
        // Get availability
        available_remote = 0.0;
        if (demand > 0) {
            for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
                rec_cell = wu_con[cur_cell].receiving[i];

                available_receiving[i] = rout_var[rec_cell].discharge[0] * 
                        global_param.dt;

                if(options.EFR){
                    available_receiving[i] -= efr_hist[rec_cell].requirement_discharge 
                            * global_param.dt;
                    if(available_receiving[i] < 0.0){
                        available_receiving[i] = 0.0;
                    }
                }

                available_remote += available_receiving[i];
            }
        }
    
        // Calculate withdrawal
        if(available_remote > 0){
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
                if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS){
                        log_err("fraction > 1.0 [%.16f]?", fraction);
                }
                withdrawn_remote = available_remote;
            }
        }
    }
    
    /**********************************************************************
    * 4. Substitute abstractions
    **********************************************************************/
    if(options.WU_GW && !satisfaction){
        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
        
        available_subs = 0.0;
        if (demand > 0) {
            // Get availability
            available_subs = (available_local[0] - withdrawn_local[0])  + 
                    (available_local[1] - withdrawn_local[1]);
        }
    
        // Calculate withdrawal
        if(available_subs > 0){
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
                if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS * 2){
                        log_err("fraction > 1.0 [%.16f]?", fraction);
                }
                withdrawn_local[0] = available_local[0];
            }    
            fraction = withdrawn_local[1] / available_local[1];
            if(fraction > 1){
                if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS * 2){
                        log_err("fraction > 1.0 [%.16f]?", fraction);
                }
                withdrawn_local[1] = available_local[1];
            }
        }
    }
    
    /**********************************************************************
    * 5. Dam abstractions
    **********************************************************************/
    if(options.WU_DAM && !satisfaction){
        // Get demand
        demand = 0.0;
        for(i = 0; i < WU_NSECTORS; i++){
            demand += wu_var[cur_cell][i].demand;
        }
        
        // Get availability
        available_dam = 0.0;
        if(demand > 0) {
            for(i = 0; i < wu_con[cur_cell].nservice; i++){
                ser_cell = wu_con[cur_cell].service[i];
                ser_idx = wu_con[cur_cell].service_idx[i];

                available_servicing[i] = dam_var[ser_cell][ser_idx].volume;
                available_dam += available_servicing[i];
            }
        }
    
        // Calculate withdrawal
        if(available_dam > 0){
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
                if(fabs(fraction - 1.0) > DBL_EPSILON * WU_NSECTORS){
                        log_err("fraction > 1.0 [%.16f]?", fraction);
                }
                withdrawn_dam = available_dam;
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
        // Modify discharge
        withdrawn = withdrawn_local[0] - returned;
        withdrawn /= global_param.dt;
        
        rout_var[cur_cell].discharge[0] -= withdrawn;
        if(rout_var[cur_cell].discharge[0] < 0.0){
            rout_var[cur_cell].discharge[0] = 0.0;
        }
    }
        
    if (withdrawn_local[1] > 0.0) {
        // Modify groundwater
        if (options.GROUNDWATER) {
            log_err("GROUNDWATER WATER USE is not yet implemented for GROUNDWATER");
        }
        else {
            for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
                for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                    if(available_moist[i][j] > 0.0){
                        withdrawn = withdrawn_local[1] * 
                                    (available_moist[i][j] / available_local[1]);
                        withdrawn *= MM_PER_M /
                                    (local_domain.locations[i].area *
                                    soil_con[cur_cell].AreaFract[j] *
                                    veg_con[cur_cell][i].Cv);
                        
                        cell[i][j].layer[l].moist -= withdrawn;
                        if(cell[i][j].layer[l].moist < resid_moist){
                            cell[i][j].layer[l].moist = resid_moist;
                        }
                    }
                }    
            }
        }
    }
    
    if(withdrawn_remote > 0){
        // Modify discharge
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
            
            if (available_receiving[i] > 0) {
                withdrawn = withdrawn_remote *
                            (available_receiving[i] / available_remote);
                withdrawn /= global_param.dt;

                rout_var[rec_cell].discharge[0] -= withdrawn;
                if(rout_var[rec_cell].discharge[0] < 0.0){
                    rout_var[rec_cell].discharge[0] = 0.0;
                }
            }
        }
    }
    
    if(withdrawn_dam > 0){
        // Modify volume
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
            
            withdrawn = withdrawn_dam *
                        (available_servicing[i] / available_dam);
            
            dam_var[ser_cell][ser_idx].volume -= withdrawn;
            if(dam_var[ser_cell][ser_idx].volume < 0.0){
                dam_var[ser_cell][ser_idx].volume = 0.0;
            }
        }
    }
    
    /**********************************************************************
    * 6. Finalize
    **********************************************************************/
    if (options.WU_GW) {
        if (!options.GROUNDWATER) {
            for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
                free(available_moist[i]);
            }
            free(available_moist);
        }
    }
    if (options.WU_REMOTE) {
        free(available_receiving);
    }
    if (options.WU_DAM) {
        free(available_servicing);
    }
}