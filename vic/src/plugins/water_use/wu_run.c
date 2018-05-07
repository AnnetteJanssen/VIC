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
    
    double fraction;
    double **moist;
    double resid_moist;
    double ice;
    
    double withdrawn_local_sec[WU_NSECTORS];
    double demand_local[3];     /* [0] surface water [1] groundwater [2] total */
    double available_local[3];  /* [0] surface water [1] groundwater [2] total */
    double withdrawn_local[3];  /* [0] surface water [1] groundwater [2] total */
    double returned;
    
    double withdrawn_remote_sec[WU_NSECTORS];
    double demand_remote;
    double available_remote;
    double withdrawn_remote;
    
    double withdrawn_dam_sec[WU_NSECTORS];
    double demand_dam;
    double available_dam;
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
                moist[i] = malloc(veg_con_map[cur_cell].nv_active * sizeof(*moist[i]));
                check_alloc_status(moist[i], "Memory allocation error.");
            }
        }
    }
    
    // Initialize    
    for(i = 0; i < 3; i++){
        demand_local[i] = 0.0;
        available_local[i] = 0.0;
        withdrawn_local[i] = 0.0;
    }    
    returned = 0.0;
    
    demand_remote = 0.0;
    available_remote = 0.0;
    withdrawn_remote = 0.0;
    
    demand_dam = 0.0;
    available_dam = 0.0;
    withdrawn_dam = 0.0;
    
    for(i = 0; i < WU_NSECTORS; i++){
        withdrawn_local_sec[i] = 0.0;
        withdrawn_remote_sec[i] = 0.0;
        withdrawn_dam_sec[i] = 0.0;
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
    * 1. Local abstractions
    **********************************************************************/
    // Get surface water availability
    for(i = 0; i < options.RIRF_NSTEPS; i++){
        available_local[0] += rout_var[cur_cell].discharge[i] * 
            global_param.dt;
    }
    
    // Get groundwater availability
    if (options.WU_GW) {
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
    }
    
    // Total water availability
    available_local[2] = available_local[0] + available_local[1];
    
    // Get surface- and groundwater demand
    for(i = 0; i < WU_NSECTORS; i++){
        demand_local[0] += wu_var[cur_cell][i].demand * 
                (1 - wu_hist[cur_cell][i].gw_fraction);
        demand_local[1] += wu_var[cur_cell][i].demand * 
                wu_hist[cur_cell][i].gw_fraction;
    }
    
    // Total water demand
    demand_local[2] = demand_local[0] + demand_local[1];
    
    // Calculate withdrawal
    if(available_local[2] > 0 && demand_local[2] > 0){
        if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
            
            // Calculate fraction
            fraction = available_local[2] / demand_local[2];
            if(fraction >= 1){
                fraction = 1.0;
            }
            
            for(i = 0; i < WU_NSECTORS; i++){
                withdrawn_local_sec[i] = wu_var[cur_cell][i].demand * 
                        fraction;
                
                wu_var[cur_cell][i].demand *= (1 - fraction);
                
                withdrawn_local[2] = demand_local[2] * fraction;
            }
        } 
        else {
            log_err("WU_STRATEGY PRIORITY has not been implemented yet");
        }
        
        // Separate withdrawal by surface- and groundwater
        if (withdrawn_local[2] >= available_local[2]) {         
            withdrawn_local[0] = available_local[0];
            withdrawn_local[1] = available_local[1];
        }
        else {
            if (available_local[0] < demand_local[0]) {
                withdrawn_local[0] = available_local[0];
                withdrawn_local[1] = withdrawn_local[2] - withdrawn_local[0];
            }
            else if (available_local[1] < demand_local[1]) {
                withdrawn_local[1] = available_local[1];
                withdrawn_local[0] = withdrawn_local[2] - withdrawn_local[1];
            }
            else{
                withdrawn_local[0] = demand_local[0];
                withdrawn_local[1] = demand_local[1];
            }
        }
        
                        
        // Calculate fraction
        fraction = withdrawn_local[2] / (withdrawn_local[0] + withdrawn_local[1]);
        if(fraction > 0){
            if(fabs(fraction - 1.0) > DBL_EPSILON){
                log_err("fraction != 1.0 [%.3f]?", fraction);
            }
        }
    }
    
    /**********************************************************************
    * 2. Remote abstractions
    **********************************************************************/
    if(options.WU_REMOTE && demand_local[2] > withdrawn_local[2]){
        
        // Get availability
        for(i = 0; i < wu_con[cur_cell].nreceiving; i++){
            rec_cell = wu_con[cur_cell].receiving[i];
                 
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                available_remote += rout_var[rec_cell].discharge[j] * 
                        global_param.dt;
            }
        }

        // Get demand
        for(i = 0; i < WU_NSECTORS; i++){
            demand_remote += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(available_remote > 0 && demand_remote > 0){
            if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
                
                // Calculate fraction
                fraction = available_remote / demand_remote;
                if(fraction >= 1){
                    fraction = 1.0;
                }
                
                // Calculate withdrawal
                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn_remote_sec[i] = wu_var[cur_cell][i].demand * 
                            fraction;
                    
                    wu_var[cur_cell][i].demand *= (1 - fraction);

                    withdrawn_remote += withdrawn_remote_sec[i];
                }
            } else {
                log_err("WU_STRATEGY PRIORITY has not been implemented yet");
            }
        }
    }
    
    /**********************************************************************
    * 3. Dam abstractions
    **********************************************************************/
    if(options.WU_DAM && 
            ((demand_local[2] > withdrawn_local[2] &&
            demand_remote > withdrawn_remote) ||            
            (demand_local[2] > withdrawn_local[2] &&
            demand_remote == 0.0))){
        
        // Get availability
        for(i = 0; i < wu_con[cur_cell].nservice; i++){
            ser_cell = wu_con[cur_cell].service[i];
            ser_idx = wu_con[cur_cell].service_idx[i];
                             
            available_dam += dam_var[ser_cell][ser_idx].volume;
        }

        // Get demand
        for(i = 0; i < WU_NSECTORS; i++){
            demand_dam += wu_var[cur_cell][i].demand;
        }
    
        // Calculate withdrawal
        if(available_dam > 0 && demand_dam > 0){
            if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
                
                // Calculate fraction
                fraction = available_dam / demand_dam;
                if(fraction >= 1){
                    fraction = 1.0;
                }
                
                // Calculate withdrawal
                for(i = 0; i < WU_NSECTORS; i++){
                    withdrawn_dam_sec[i] = wu_var[cur_cell][i].demand * 
                            fraction;

                    wu_var[cur_cell][i].demand *= (1 - fraction);

                    withdrawn_dam += withdrawn_dam_sec[i];
                }
            } else {
                log_err("WU_STRATEGY PRIORITY has not been implemented yet");
            }
        }
    }
    
    /**********************************************************************
    * 4. Consumption and return
    **********************************************************************/    
    // Calculate consumption and return flow
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].withdrawn +=
                withdrawn_local_sec[i] + 
                withdrawn_remote_sec[i] + 
                withdrawn_dam_sec[i];
        wu_var[cur_cell][i].consumed +=
                wu_var[cur_cell][i].withdrawn * 
                wu_hist[cur_cell][i].consumption_fraction;
        wu_var[cur_cell][i].returned +=
                wu_var[cur_cell][i].withdrawn * 
                (1 - wu_hist[cur_cell][i].consumption_fraction);
        
        returned += wu_var[cur_cell][i].returned;
    }
    
    /**********************************************************************
    * 5. Actual withdrawal
    **********************************************************************/
    // Calculate reduction and return flow
    if(withdrawn_local[2] > 0){
        
        if (withdrawn_local[0] > 0.0) {
            // Calculate fraction
            fraction = (withdrawn_local[0] - returned) / available_local[0];
            if(fraction > 1.0){
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                    log_err("fraction > 1.0 [%.3f]?", fraction);
                }
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
                if(fabs(fraction - 1.0) > DBL_EPSILON){
                    log_err("fraction > 1.0 [%.3f]?", fraction);
                }
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
    }
    
    if(withdrawn_remote > 0){
        
        // Calculate fraction
        fraction = withdrawn_remote / available_remote;
        if(fraction > 1.0){
            if(fabs(fraction - 1.0) > DBL_EPSILON){
                log_err("fraction > 1.0 [%.3f]?", fraction);
            }
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
            if(fabs(fraction - 1.0) > DBL_EPSILON){
                log_err("fraction > 1.0 [%.3f]?", fraction);
            }
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