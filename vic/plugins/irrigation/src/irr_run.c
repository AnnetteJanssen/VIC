#include <vic_driver_image.h>
#include <plugin.h>

void
irr_run_requirement(size_t iCell)
{
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern plugin_parameters_struct plugin_param;
    
    double moist[MAX_LAYERS];
    double total_moist;
    double total_wcr;
    double area_fract;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    veg_var_struct *cveg_var;
    cell_data_struct *ccell_var;
    
    csoil_con = &soil_con[iCell];
    
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
            cveg_var = &all_vars[iCell].veg_var[cirr_con->veg_index][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){

                // Reset values
                cirr_var->need = 0.0;
                ccell_var->layer[0].Ksat = csoil_con->Ksat[0];
                
                if(cveg_var->fcanopy > MIN_FCANOPY){
                    
                    /**********************************************************************
                    * Initialize
                    **********************************************************************/
                    // Reduce Ksat
                    if (cirr_con->paddy){
                        ccell_var->layer[0].Ksat = pow(csoil_con->Ksat[0], plugin_param.Ksat_expt);
                    }
                    
                    // Get moisture content and critical moisture content of every layer
                    total_moist = 0.0;
                    total_wcr = 0.0;        
                    for(k = 0; k < options.Nlayer; k++){
                        moist[k] = 0.0;
                    } 

                    for(k = 0; k < options.Nlayer; k++){                
                        for (l = 0; l < options.Nfrost; l++) {
                            moist[k] += (ccell_var->layer[k].moist - 
                                         ccell_var->layer[k].ice[l]) *
                                         csoil_con->frost_fract[l];
                        }

                        if(cveg_con->root[k] > 0.){
                            total_moist += moist[k];
                            total_wcr += csoil_con->Wcr[k];
                        }
                    }
                    
                    /**********************************************************************
                    * Requirement
                    **********************************************************************/
                    // Calculate requirment - suboptimal evapotranspiration
                    // (based on field capacity)
                    if(cirr_con->paddy){
                        // With ponding the moisture of the top layer should
                        // always be saturated
                        if(moist[0] + cirr_var->leftover < csoil_con->max_moist[0]){
                            cirr_var->requirement = 
                                    csoil_con->max_moist[0] - 
                                    (moist[0] + cirr_var->leftover);
                        }
                    }         
                    else if(options.SHARE_LAYER_MOIST){
                        // In the SHARE_LAYER_MOIST option the moisture and critical
                        // moisture point of all layers with roots are combined
                        if(total_moist + cirr_var->leftover < total_wcr){
                            cirr_var->requirement = 
                                    (total_wcr / plugin_param.Wfc_fract) - 
                                    (total_moist + cirr_var->leftover);
                        }
                    }else{
                        // Without the SHARE_LAYER_MOIST option the moisture and 
                        // critical moisture point of all layers with roots are assessed individually
                        bool calc_req = false;
                        for(k = 0; k < options.Nlayer; k++){
                            if(cveg_con->root[k] > 0.){
                                if(moist[k] + cirr_var->leftover < csoil_con->Wcr[k]){
                                    calc_req = true;
                                    break;
                                }
                            }
                        }

                        if(calc_req){
                            cirr_var->requirement = 
                                    (total_wcr / plugin_param.Wfc_fract) - 
                                    (total_moist + cirr_var->leftover);
                        }
                    }

                    if(cirr_var->requirement < 0.0){
                        log_err("Irrigation requirement < 0.0 [%.16f]?",
                                cirr_var->requirement);
                    }

                    // Calculate need - newly added requirement
                    cirr_var->need = 
                            cirr_var->requirement - cirr_var->prev_req;
                    if(cirr_var->need < 0.0){
                        cirr_var->need = 0.0;
                    }
                    cirr_var->prev_req = cirr_var->requirement;
                }
            }
        }   
    }
}

void
irr_run_shortage(size_t iCell)
{
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern plugin_parameters_struct plugin_param;
    
    double moist[MAX_LAYERS];
    double total_moist;
    double total_wcr;
    double area_fract;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    veg_var_struct *cveg_var;
    cell_data_struct *ccell_var;
    
    csoil_con = &soil_con[iCell];
    
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
            cveg_var = &all_vars[iCell].veg_var[cirr_con->veg_index][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){ 

                // Reset values
                cirr_var->deficit = 0.0;
                
                if(cveg_var->fcanopy > MIN_FCANOPY){
                    
                    /**********************************************************************
                    * Initialize
                    **********************************************************************/
                    // Get moisture content and critical moisture content of every layer
                    total_moist = 0.0;
                    total_wcr = 0.0;        
                    for(k = 0; k < options.Nlayer; k++){
                        moist[k] = 0.0;
                    } 

                    for(k = 0; k < options.Nlayer; k++){                
                        for (l = 0; l < options.Nfrost; l++) {
                            moist[k] += (ccell_var->layer[k].moist - 
                                         ccell_var->layer[k].ice[l]) *
                                         csoil_con->frost_fract[l];
                        }

                        if(cveg_con->root[k] > 0.){
                            total_moist += moist[k];
                            total_wcr += csoil_con->Wcr[k];
                        }
                    }
                    
                    /**********************************************************************
                    * Shortage
                    **********************************************************************/
                    // Calculate shortage - suboptimal evapotranspiration
                    // (based on VIC equations for evapotranspiration)
                    if(cirr_con->paddy){
                        // With ponding the moisture of the top layer should
                        // always be saturated
                        if(moist[0] < csoil_con->max_moist[0]){
                            cirr_var->shortage = csoil_con->max_moist[0] - moist[0];
                        }
                    } 
                    else if(options.SHARE_LAYER_MOIST){
                        // In the SHARE_LAYER_MOIST option the moisture and critical
                        // moisture point of all layers with roots are combined
                        if(total_moist < total_wcr){
                            cirr_var->shortage = total_wcr - total_moist;
                        }
                    }
                    else{
                        // Without the SHARE_LAYER_MOIST option the moisture and 
                        // critical moisture point of all layers with roots are assessed individually
                        for(k = 0; k < options.Nlayer; k++){
                            if(cveg_con->root[k] > 0.){
                                if(moist[k] < csoil_con->Wcr[k]){
                                    cirr_var->shortage = csoil_con->Wcr[k] - moist[k];
                                }
                            }
                        }
                    }
                    
                    
                    if(cirr_var->shortage < 0.0){
                        log_err("Irrigation shortage < 0.0 [%.16f]?",
                                cirr_var->shortage);
                    }
                    
                    // Calculate deficit - newly added shortage
                    cirr_var->deficit = 
                            cirr_var->shortage - cirr_var->prev_short;
                    if(cirr_var->deficit < 0.0){
                        cirr_var->deficit = 0.0;
                    }
                    cirr_var->prev_short = cirr_var->shortage;
                }
            }
        }
    }
}
