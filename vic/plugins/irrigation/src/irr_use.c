#include <vic_driver_image.h>
#include <plugin.h>

void
irr_set_demand(size_t iCell)
{
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    
    double demand;
    double area_fract;
    double veg_fract;
    
    size_t i;
    size_t j;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    
    // get demand
    demand = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){
                demand += cirr_var->requirement * veg_fract * area_fract;
            }
        }
    }
    
    // TODO set demand to water-use module
}

void
irr_get_withdrawn(size_t iCell)
{
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern all_vars_struct *all_vars;
    extern global_param_struct global_param;
    
    double demand;
    double available;
    double avail_frac;
    double avail_irr;
    double area_fract;
    double veg_fract;
    double max_infilt;
    double max_moist;
    double max_added;
    
    size_t i;
    size_t j;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    cell_data_struct *ccell_var;
        
    // get demand
    // TODO get demand from water use module
    demand = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){
                demand += cirr_var->requirement * veg_fract * area_fract;
            }
        }
    }
    
    // get availability
    available = 0.0;
    if(plugin_options.POTENTIAL_IRRIGATION){
        available = demand;
    } else {
        // TODO get availability from water use module
    }
    
    if(available > 0){
        avail_frac = demand / available;
        if(avail_frac > 1){
            log_err("Available fraction > 1 [%.6f]?", avail_frac);
        }
    } else {
        avail_frac = 0;
    }
    
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;

        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
            area_fract = csoil_con->AreaFract[j];

            max_infilt = ccell_var->layer[0].Ksat / global_param.model_steps_per_day;
            max_moist = csoil_con->max_moist[0] - ccell_var->layer[0].moist;
            max_added = min(max_infilt, max_moist);

            avail_irr = cirr_var->requirement * avail_frac + cirr_var->leftover;
            if(avail_irr < max_added){
                ccell_var->layer[0].moist += avail_irr;
                cirr_var->prev_req -= avail_irr;
            } else {
                ccell_var->layer[0].moist += max_added;
                cirr_var->prev_req -= max_added;
                cirr_var->leftover = avail_irr - max_added;
            }
        }
    }
}     
