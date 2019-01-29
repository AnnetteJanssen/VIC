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
    extern parameters_struct param;
    
    double moist[MAX_LAYERS];
    double total_moist;
    double total_wcr;
    double season_day;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    bool pond;
    
    irr_con_struct *cur_irr_con;
    irr_var_struct *cur_irr_var;
    soil_con_struct *cur_soil_con;
    veg_con_struct *cur_veg_con;
    cell_data_struct *cur_cell_var;
    
    cur_soil_con = &soil_con[cur_cell];
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_irr_con = &irr_con[cur_cell][i];
        cur_veg = cur_irr_con->veg_index;
        cur_veg_con = &veg_con[cur_cell][cur_veg];
        
        /**********************************************************************
        * 0. Reset values
        **********************************************************************/
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){  
            cur_irr_var = &irr_var[cur_cell][i][j];  
            
            cur_irr_var->need = 0.0;
            cur_irr_var->shortage = 0.0;
        }
                
        /**********************************************************************
        * 1. Check irrigation season
        **********************************************************************/
        for(j = 0; j < cur_irr_con->nseasons; j++){
            season_day = between_jday(cur_irr_con->season_start[j],
                cur_irr_con->season_end[j],dmy[current].day_in_year);
            
            if(season_day > 0){
                break;
            }
        }
        
        if(season_day <= 0.0){
            // Outside of irrigation season            
            for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
                cur_irr_var = &irr_var[cur_cell][i][j];
                cur_cell_var = &all_vars[cur_cell].cell[cur_veg][j];
                
                cur_irr_var->requirement = 0.0;
                cur_irr_var->prev_req = 0.0; 
                if (cur_irr_con->ponding){
                    cur_cell_var->layer[0].Ksat = cur_soil_con->Ksat[0];
                }
            }

            continue;
        }
        
        // Run irrigated vegetation
        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
            cur_irr_var = &irr_var[cur_cell][i][j];
            cur_cell_var = &all_vars[cur_cell].cell[cur_veg][j]; 
            
            if (cur_irr_con->ponding){
                cur_cell_var->layer[0].Ksat = pow(cur_soil_con->Ksat[0], param.IRR_KPUDDLE);
            }
                            
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
                    moist[k] += (cur_cell_var->layer[k].moist - 
                                 cur_cell_var->layer[k].ice[l]) *
                                 cur_soil_con->frost_fract[l];
                }
                
                if(cur_veg_con->root[k] > 0.){
                    total_moist += moist[k];
                    total_wcr += cur_soil_con->Wcr[k];
                }
            }
            
            /**********************************************************************
            * 3. Get shortage/deficit
            **********************************************************************/
            // Calculate shortage - suboptimal evapotranspiration
            // (based on VIC equations for evapotranspiration)
            if(cur_irr_con->ponding){
                // With ponding the moisture of the top layer should
                // always be saturated
                if(moist[0] < cur_soil_con->max_moist[0]){
                    irr_var[cur_cell][i][j].shortage +=
                            soil_con[cur_cell].max_moist[0] - moist[0];
                }
            } 
            else if(options.SHARE_LAYER_MOIST){
                // In the SHARE_LAYER_MOIST option the moisture and critical
                // moisture point of all layers with roots are combined
                if(total_moist < total_wcr){
                    irr_var[cur_cell][i][j].shortage +=
                            total_wcr - total_moist;
                }
            }
            else{
                // Without the SHARE_LAYER_MOIST option the moisture and 
                // critical moisture point of all layers with roots are
                // assessed individually
                for(k = 0; k < options.Nlayer; k++){
                    if(veg_con[cur_cell][cur_veg].root[k] > 0.){
                        if(moist[k] < soil_con[cur_cell].Wcr[k]){
                            irr_var[cur_cell][i][j].shortage +=
                                    soil_con[cur_cell].Wcr[k] - moist[k];
                        }
                    }
                }
            }
            
            if(irr_var[cur_cell][i][j].shortage < 0.0){
                log_err("Irrigation shortage < 0.0 [%.16f]?",
                        irr_var[cur_cell][i][j].shortage);
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
            if(irr_con[cur_cell][i].ponding){
                // With ponding the moisture of the top layer should
                // always be saturated
                if((moist[0] + irr_var[cur_cell][i][j].leftover)
                        < soil_con[cur_cell].max_moist[0]){
                    // moisture content is below critical   
                    irr_var[cur_cell][i][j].requirement = 
                            soil_con[cur_cell].max_moist[0] - 
                            (moist[0] + irr_var[cur_cell][i][j].leftover);
                }
            }         
            else if(options.SHARE_LAYER_MOIST){
                // In the SHARE_LAYER_MOIST option the moisture and critical
                // moisture point of all layers with roots are combined
                if((total_moist + irr_var[cur_cell][i][j].leftover)
                        < (total_wcr / param.IRR_WIRR)){
                    // moisture content is below critical   
                    irr_var[cur_cell][i][j].requirement = 
                            (total_wcr / param.IRR_WFC) - 
                            (total_moist + irr_var[cur_cell][i][j].leftover);
                }
            }else{
                // Without the SHARE_LAYER_MOIST option the moisture and 
                // critical moisture point of all layers with roots are
                // assessed individually
                bool calc_req = false;
                for(k = 0; k < options.Nlayer; k++){
                    if(veg_con[cur_cell][cur_veg].root[k] > 0.){
                        if(moist[k] < 
                                soil_con[cur_cell].Wcr[k] / param.IRR_WIRR){
                            calc_req = true;
                            break;
                        }
                    }
                }

                if(calc_req){
                    // moisture content is below critical
                    irr_var[cur_cell][i][j].requirement = 
                            (total_wcr / param.IRR_WFC) - 
                            (total_moist + irr_var[cur_cell][i][j].leftover);
                }
            }
            
            if(irr_var[cur_cell][i][j].requirement < 0.0){
                log_err("Irrigation requirement < 0.0 [%.16f]?",
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
                                        
            /**********************************************************************
            * 5. Handle potential irrigation
            **********************************************************************/
            if(options.IRR_POTENTIAL && 
                    irr_var[cur_cell][i][j].requirement > 0.0){
                all_vars[cur_cell].cell[cur_veg][j].layer[0].moist +=
                        irr_var[cur_cell][i][j].requirement;
                if(all_vars[cur_cell].cell[cur_veg][j].layer[0].moist >
                        soil_con[cur_cell].max_moist[0]){
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                            soil_con[cur_cell].max_moist[0];
                }
            }
        }        
    }
}
     
void
irr_set_demand(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern elev_con_map_struct *elev_con_map;
    extern wu_force_struct **wu_force;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern size_t NR;
    
    double demand;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    
    demand = 0.0;            
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;

        for(j = 0; j < elev_con_map[cur_cell].ne_active; j++){
            demand += irr_var[cur_cell][i][j].requirement *
                    soil_con[cur_cell].AreaFract[j] * 
                    veg_con[cur_cell][cur_veg].Cv;
        }
    }
    wu_force[cur_cell][WU_IRRIGATION].demand[NR] = demand / 
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
            (wu_var[cur_cell][WU_IRRIGATION].withdrawn - 
            wu_var[cur_cell][WU_IRRIGATION].returned) / 
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
    
    if(total_available > 0){
        // Received water for irrigation
        
        // Calculate available as fraction of requirement
        fraction =  total_available / total_requirement;
    
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
