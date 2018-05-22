#include <vic.h>

void
irr_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    
    double *moist;
    double total_moist;
    double total_wcr;
    double season_day;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    /**********************************************************************
    * 0. Allocation
    **********************************************************************/
    // Allocate
    moist = malloc(options.Nlayer * sizeof(*moist));
    check_alloc_status(moist, "Memory allocation error.");
    
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;
        
        // Reset values
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){     
            irr_var[cur_cell][i][j].need = 0.0;
            irr_var[cur_cell][i][j].shortage = 0.0;
        }
                
        /**********************************************************************
        * 1. Check irrigation season
        **********************************************************************/
        for(j = 0; j < irr_con[cur_cell][i].nseasons; j++){
            season_day = between_jday(irr_con[cur_cell][i].season_start[j],
                irr_con[cur_cell][i].season_end[j],dmy[current].day_in_year);
            
            if(season_day > 0){
                break;
            }
        }
        
        if(season_day <= 0.0){
            // Outside of irrigation season            
            for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                irr_var[cur_cell][i][j].requirement = 0.0;
                irr_var[cur_cell][i][j].prev_req = 0.0; 
            }
            
            if (irr_con[cur_cell][i].ponding){
                for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){  
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat = 
                            soil_con[cur_cell].Ksat[0];
                }
            }

            continue;
        } else {
            // Inside of irrigation season              
            if (irr_con[cur_cell][i].ponding){
                for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat = 
                            soil_con[cur_cell].Ksat[0] * POND_KSAT_FRAC;
                }
            }
        }
        
        // Run irrigated vegetation
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                            
            /**********************************************************************
            * 2. Get moisture content
            **********************************************************************/
            // Initialize     
            total_moist = 0.0;
            total_wcr = 0.0;        
            for(k = 0; k < options.Nlayer; k++){
                moist[k] = 0.0;
            } 
            
            // Get moisture content and critical moisture content of every layer
            for(k = 0; k < options.Nlayer; k++){                
                for (l = 0; l < options.Nfrost; l++) {
                    moist[k] += ((all_vars[cur_cell].cell[cur_veg][j].layer[k].moist -
                      all_vars[cur_cell].cell[cur_veg][j].layer[k].ice[l])
                            * soil_con[cur_cell].frost_fract[l]);
                }
                
                if(veg_con[cur_cell][cur_veg].root[k] > 0.){
                    total_moist += moist[k];
                    total_wcr += soil_con[cur_cell].Wcr[k];
                }
            }
            
            /**********************************************************************
            * 3. Get shortage/deficit
            **********************************************************************/
            // Calculate shortage - suboptimal evapotranspiration
            // (based on VIC equations for evapotranspiration)
            if(options.SHARE_LAYER_MOIST){
                if(total_moist < total_wcr &&
                    season_day > irr_con[cur_cell][i].season_offset){
                    irr_var[cur_cell][i][j].shortage +=
                            total_wcr - total_moist;
                }
            }else{
                for(k = 0; k < options.Nlayer; k++){
                    if(veg_con[cur_cell][cur_veg].root[k] > 0.){
                        if(moist[k] < soil_con[cur_cell].Wcr[k] && 
                                season_day > irr_con[cur_cell][i].season_offset){
                            irr_var[cur_cell][i][j].shortage +=
                                    soil_con[cur_cell].Wcr[k] - moist[k];
                        }
                    }
                }
            }
            
            // Calculate deficit - newly added shortage
            irr_var[cur_cell][i][j].deficit =
                    irr_var[cur_cell][i][j].shortage - 
                    irr_var[cur_cell][i][j].prev_short;
            if(irr_var[cur_cell][i][j].deficit < 0.0){
                irr_var[cur_cell][i][j].deficit = 0.0;
            }
            irr_var[cur_cell][i][j].prev_short = 
                irr_var[cur_cell][i][j].shortage;
                                        
            /**********************************************************************
            * 4. Check requirement/need
            **********************************************************************/
            // Get requirement              
            if(options.SHARE_LAYER_MOIST){
                // In the SHARE_LAYER_MOIST option the moisture and critical
                // moisture point of all layers with roots are combined
                
                if((total_moist + irr_var[cur_cell][i][j].leftover)
                        < (total_wcr / IRR_CRIT_FRAC)){
                    // moisture content is below critical   
                    irr_var[cur_cell][i][j].requirement = 
                            (total_wcr / FIELD_CAP_FRAC) - 
                            (total_moist + irr_var[cur_cell][i][j].leftover);
                } else if (irr_con[cur_cell][i].ponding &&
                        (irr_var[cur_cell][i][j].pond_storage + 
                            irr_var[cur_cell][i][j].leftover) < 
                        irr_con[cur_cell][i].pond_capacity * POND_IRR_CRIT_FRAC){
                    // pond storage is below critical    
                    irr_var[cur_cell][i][j].requirement = 
                            irr_con[cur_cell][i].pond_capacity - 
                            (irr_var[cur_cell][i][j].pond_storage + 
                            irr_var[cur_cell][i][j].leftover);
                }
            }else{
                // Without the SHARE_LAYER_MOIST option the moisture and 
                // critical moisture point of all layers with roots are
                // assessed individually
                
                bool calc_req = false;
                for(k = 0; k < options.Nlayer; k++){
                    if(veg_con[cur_cell][cur_veg].root[k] > 0.){
                        if(moist[k] < 
                                soil_con[cur_cell].Wcr[k] / IRR_CRIT_FRAC){
                            calc_req = true;
                            break;
                        }
                    }
                }

                if(calc_req){
                    // moisture content is below critical
                    irr_var[cur_cell][i][j].requirement = 
                            (total_wcr / FIELD_CAP_FRAC) - 
                            (total_moist + irr_var[cur_cell][i][j].leftover);
                } else if (irr_con[cur_cell][i].ponding &&
                        (irr_var[cur_cell][i][j].pond_storage + 
                            irr_var[cur_cell][i][j].leftover) < 
                        irr_con[cur_cell][i].pond_capacity * POND_IRR_CRIT_FRAC){
                    // pond storage is below critical 
                    irr_var[cur_cell][i][j].requirement = 
                            irr_con[cur_cell][i].pond_capacity - 
                            (irr_var[cur_cell][i][j].pond_storage + 
                            irr_var[cur_cell][i][j].leftover);
                }
            }
            
            if(irr_var[cur_cell][i][j].requirement < 0.0){
                log_err("Irrigation requirement < 0.0 [%.3f]?",
                        irr_var[cur_cell][i][j].requirement);
            }
            
            // Calculate need - newly added requirement
            irr_var[cur_cell][i][j].need = 
                    irr_var[cur_cell][i][j].requirement - 
                    irr_var[cur_cell][i][j].prev_req;
            if(irr_var[cur_cell][i][j].need < 0.0){
                irr_var[cur_cell][i][j].need = 0.0;
            }
            irr_var[cur_cell][i][j].prev_req = 
                irr_var[cur_cell][i][j].requirement;
            }
        }
    
    /**********************************************************************
    * 5. Finalization
    **********************************************************************/  
    free(moist);
}
     
void
irr_set_demand(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern elev_con_map_struct *elev_con_map;
    extern wu_hist_struct **wu_hist;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    
    double total_demand;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    
    total_demand = 0.0;            
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;

        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
            total_demand += irr_var[cur_cell][i][j].requirement *
                    soil_con[cur_cell].AreaFract[j] * 
                    veg_con[cur_cell][cur_veg].Cv;
        }
    }

    wu_hist[cur_cell][WU_IRRIGATION].consumption_fraction = irr_con[cur_cell][0].WUE;
    wu_hist[cur_cell][WU_IRRIGATION].gw_fraction = irr_con[cur_cell][0].gw_fraction;
    wu_hist[cur_cell][WU_IRRIGATION].demand = total_demand / 
                wu_hist[cur_cell][WU_IRRIGATION].consumption_fraction /
                MM_PER_M * local_domain.locations[cur_cell].area; 
}

void
irr_get_withdrawn(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern all_vars_struct *all_vars;
    extern option_struct options;
    extern global_param_struct global_param;
    extern irr_con_map_struct *irr_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern wu_var_struct **wu_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern all_vars_struct *all_vars;
    extern option_struct options;
    
    double total_available;
    double total_requirement;
    double fraction;
    double available;
    double need;  
    double max_infil;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    
    total_available = 
            wu_var[cur_cell][WU_IRRIGATION].consumed / 
            local_domain.locations[cur_cell].area * 
            MM_PER_M;

    total_requirement = 0.0;    
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;
        
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){  
            total_requirement +=
                    irr_var[cur_cell][i][j].requirement * 
                    soil_con[cur_cell].AreaFract[j] * 
                    veg_con[cur_cell][cur_veg].Cv;
        }
    }
    
    if(options.IRR_POTENTIAL){
        total_available = total_requirement;
    }
    
    if(total_available > 0){
        // Received water for irrigation
        
        // Calculate available as fraction of requirement
        fraction =  total_available / total_requirement;        
        if(fraction > 1.0){
            if (fabs(fraction - 1.0) > DBL_EPSILON && 
                    total_requirement > DBL_EPSILON) {
                log_err("Fraction is > 1.0 [%.3f]?", fraction);
            }
            fraction = 1.0;
        }
    
        for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
            cur_veg = irr_con[cur_cell][i].veg_index;

            for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                
                // Availability is equally divided;
                available = irr_var[cur_cell][i][j].requirement * fraction;
                
                // Reduce requirement
                irr_var[cur_cell][i][j].requirement -= available;
                if(irr_var[cur_cell][i][j].requirement < 0.0){
                    irr_var[cur_cell][i][j].requirement = 0.0;
                }
                
                // Calculate maximum infiltration
                max_infil = all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat / 
                        global_param.model_steps_per_day;
                
                if(irr_con[cur_cell][i].ponding){
                    
                    // Fill pond
                    need = irr_con[cur_cell][i].pond_capacity - 
                           irr_var[cur_cell][i][j].pond_storage;

                    if(available > need){
                        // Received too much
                        irr_var[cur_cell][i][j].pond_storage = 
                                irr_con[cur_cell][i].pond_capacity;
                        irr_var[cur_cell][i][j].leftover =
                                available -
                                need;
                    }else{
                        // Received too little
                        irr_var[cur_cell][i][j].pond_storage += 
                                available;
                    }
                }else{
                    
                    // Fill first soil layer
                    need = soil_con[cur_cell].max_moist[0] - 
                           all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
                    if(need > max_infil){
                        need = max_infil;
                    }

                    if(available > need){
                        // Received too much
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                                soil_con[cur_cell].max_moist[0];
                        irr_var[cur_cell][i][j].leftover =
                                available -
                                need;
                    }else{
                        // Received too little
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                                available;
                        available = 0.0;
                    }
                }
            }
        }
    }
}            
  
void
irr_run_ponding_leftover(size_t cur_cell)
{
    extern all_vars_struct *all_vars;
    extern option_struct options;
    extern global_param_struct global_param;
    extern irr_con_map_struct *irr_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern all_vars_struct *all_vars;
    extern option_struct options;
    
    double need;  
    double max_infil;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;
        
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){  
            
            // Calculate maximum infiltration
            max_infil = all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat / 
                    global_param.model_steps_per_day;
            
            // Handle leftovers
            if(irr_var[cur_cell][i][j].leftover > 0){                
                if(irr_con[cur_cell][i].ponding){
                    
                    // Fill pond
                    need = irr_con[cur_cell][i].pond_capacity - 
                           irr_var[cur_cell][i][j].pond_storage;

                    if(irr_var[cur_cell][i][j].leftover >
                            need){
                        // Leftover too much
                        irr_var[cur_cell][i][j].pond_storage = 
                                irr_con[cur_cell][i].pond_capacity;
                        irr_var[cur_cell][i][j].leftover -=
                                need;
                    }else{
                        // Leftover too little
                        irr_var[cur_cell][i][j].pond_storage += 
                                irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0;
                    }                      
                }else{
                    
                    // Fill first soil layer
                    need = soil_con[cur_cell].max_moist[0] - 
                           all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
                    
                    if(need > max_infil){
                        need = max_infil;
                    }

                    if(irr_var[cur_cell][i][j].leftover >
                            need){
                        // Leftover too much
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                                soil_con[cur_cell].max_moist[0];
                        irr_var[cur_cell][i][j].leftover -=
                                need;
                    }else{
                        // Leftover too little
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                                irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0.0;
                    }         
                }
            }
            
            // Handle pond infiltration
            if(irr_var[cur_cell][i][j].pond_storage > 0){
                
                // Fill first soil layer
                need = soil_con[cur_cell].max_moist[0] - 
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
                if(need > max_infil){
                    need = max_infil;
                }

                 if(irr_var[cur_cell][i][j].pond_storage >
                         need){
                     // Pond storage too much
                     all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                             soil_con[cur_cell].max_moist[0];
                     irr_var[cur_cell][i][j].pond_storage -=
                             need;
                 }else{
                     // Pond storage too little
                     all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                             irr_var[cur_cell][i][j].pond_storage;
                     irr_var[cur_cell][i][j].pond_storage = 0.0;
                 }
            }
        }
    }
}
