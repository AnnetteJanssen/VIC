#include <vic_driver_image.h>
#include <plugin.h>

void
wu_run(size_t iCell)
{
    extern domain_struct  local_domain;
    extern all_vars_struct     *all_vars;
    extern option_struct options;
    extern plugin_option_struct plugin_options;
    extern global_param_struct global_param;
    extern soil_con_struct     *soil_con;
    extern veg_con_map_struct     *veg_con_map;
    extern veg_con_struct     **veg_con;
    
    double **av_gw;
    double resid_moist;
    double available_gw;
    double available_surf;
    double available_remote;
    double demand_gw;
    double demand_surf;
    double demand_remote;
    
    size_t i;
    size_t j;
    size_t iVeg;
    size_t iBand;
    size_t iFrost;
    size_t iLayer;
    int iSector;
    size_t iCell2;
    int iSector2;
    double frac;
    double ice;
    
    av_gw = malloc(veg_con_map[iCell].nv_active * sizeof(*av_gw));
    check_alloc_status(av_gw, "Memory allocation error.");
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        av_gw[iVeg] = malloc(options.SNOW_BAND * sizeof(*av_gw[iVeg]));
        check_alloc_status(av_gw[iVeg], "Memory allocation error.");
    }
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
            av_gw[iVeg][iBand] = 0.0;
        }
    }
    
    /******************************************
     Demand
    ******************************************/   
    demand_gw = 0;
    demand_surf = 0;
    demand_remote = 0;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        // groundwater
        wu_var[iCell][iSector].demand_gw = wu_force[iCell][iSector].demand[NR] * 
                wu_force[iCell][iSector].groundwater_frac[NR];
        demand_gw += wu_var[iCell][iSector].demand_gw;
        
        // surface water
        wu_var[iCell][iSector].demand_surf = wu_force[iCell][iSector].demand[NR] * 
                (1 - wu_force[iCell][iSector].groundwater_frac[NR]);
        demand_surf += wu_var[iCell][iSector].demand_surf;
        
        // remote
        wu_var[iCell][iSector].demand_remote = 0.0;
        for(j = 0; j < wu_con[iCell][iSector].nreceiving; j++){
            iCell2 = wu_con[iCell][iSector].receiving[j];
            
            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }
            
            wu_var[iCell][iSector].demand_remote += 
                    wu_var[iCell2][iSector2].demand_surf - 
                    wu_var[iCell2][iSector2].withdrawn_surf; 
            demand_remote += wu_var[iCell][iSector].demand_remote;       
        }
    }
    
    /******************************************
     Availability
    ******************************************/  
    available_gw = 0;
    available_surf = 0;
    available_remote = 0;
    
    // groundwater
    iLayer = options.Nlayer - 1;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
            if(soil_con[iCell].AreaFract[iBand] > 0){
                av_gw[iVeg][iBand] = all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist;
                
                resid_moist = soil_con[iCell].resid_moist[iLayer] *
                              soil_con[iCell].depth[iLayer] * MM_PER_M;
                av_gw[iVeg][iBand] -= resid_moist;
                
                ice = 0;
                for(iFrost = 0; iFrost < options.Nfrost; iFrost ++){
                    ice += all_vars[iCell].cell[iVeg][iBand].layer[iLayer].ice[iFrost] *
                           soil_con[iCell].frost_fract[iFrost];
                }
                av_gw[iVeg][iBand] -= ice;
                
                if(av_gw[iVeg][iBand] < 0){
                    av_gw[iVeg][iBand] = 0;
                }
                
                available_gw += av_gw[iVeg][iBand] *
                            soil_con[iCell].AreaFract[iBand] *
                            veg_con[iCell][iVeg].Cv;
            }
        }
    }
    
    // surface water
    available_surf = rout_var[iCell].discharge * global_param.dt / 
            local_domain.locations[iCell].area * MM_PER_M;
    
    
    // remote
    if(available_surf > demand_surf){
        available_remote = available_surf - demand_surf;
        available_surf -= available_remote;
    }
    
    /******************************************
     Split
    ******************************************/ 
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        // groundwater
        if(demand_gw > 0){
            wu_var[iCell][iSector].available_gw = available_gw *
                    wu_var[iCell][iSector].demand_gw / 
                    demand_gw;
        } else {
            wu_var[iCell][iSector].available_gw = 0.0;
        }
        
        // surface water
        if(demand_surf > 0){
            wu_var[iCell][iSector].available_surf = available_surf *
                    wu_var[iCell][iSector].demand_surf / 
                    demand_surf;
        } else {
            wu_var[iCell][iSector].available_surf = 0.0;
        }
        
        // remote
        wu_var[iCell][iSector].available_remote = 0.0;
        if(demand_remote > 0){
            for(j = 0; j < wu_con[iCell][iSector].nreceiving; j++){
                iSector2 = wu_con_map[iCell2].sidx[i];        
                if(iSector2 == NODATA_WU){
                    continue;
                }
            
                wu_var[iCell][iSector].available_remote += available_remote *
                        wu_var[iCell][iSector].demand_remote / 
                        demand_remote;
            }
        }
    }
    
    /******************************************
     Withdrawals & Returns
    ******************************************/ 
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        wu_var[iCell][iSector].returned = 0.0;
        
        // groundwater
        if(wu_var[iCell][iSector].available_gw > 0){
            frac = wu_var[iCell][iSector].demand_gw / 
                    wu_var[iCell][iSector].available_gw;
            frac = min(frac, 1);
            wu_var[iCell][iSector].withdrawn_gw = 
                    wu_var[iCell][iSector].available_gw * frac;
        } else {
            wu_var[iCell][iSector].withdrawn_gw = 0;
        }

        wu_var[iCell][iSector].returned += 
                wu_var[iCell][iSector].withdrawn_gw * 
                (1 - wu_force[iCell][iSector].consumption_frac[NR]);
        
        // surface water
        if(wu_var[iCell][iSector].available_surf > 0){
            frac = wu_var[iCell][iSector].demand_surf / 
                    wu_var[iCell][iSector].available_surf;
            frac = min(frac, 1);
            wu_var[iCell][iSector].withdrawn_surf = 
                    wu_var[iCell][iSector].available_surf * frac;
        } else {
            wu_var[iCell][iSector].withdrawn_surf = 0;
        }
        
        wu_var[iCell][iSector].returned += 
                wu_var[iCell][iSector].withdrawn_surf * 
                (1 - wu_force[iCell][iSector].consumption_frac[NR]);
        
        // remote
        for(j = 0; j < wu_con[iCell][iSector].nreceiving; j++){
            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }
            
            if(wu_var[iCell][iSector].available_remote > 0){
                frac = wu_var[iCell][iSector].demand_remote / 
                        wu_var[iCell][iSector].available_remote;
                frac = min(frac, 1);
                wu_var[iCell2][iSector2].withdrawn_remote = 
                        wu_var[iCell][iSector].available_remote * frac;
            } else {
                wu_var[iCell2][iSector2].withdrawn_remote = 0;
            }
        
            wu_var[iCell][iSector].returned += 
                    wu_var[iCell2][iSector2].withdrawn_remote * 
                    (1 - wu_force[iCell2][iSector2].consumption_frac[NR]);
        }
    }
    
    /******************************************
     Return
    ******************************************/
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        // groundwater
        if(available_gw > 0){
            iLayer = options.Nlayer - 1;
            for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
                for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
                    if(soil_con[iCell].AreaFract[iBand] > 0){
                        all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist -=
                                wu_var[iCell][iSector].withdrawn_gw *
                                av_gw[iVeg][iBand] /
                                available_gw;
                        
                        ice = 0;
                        for(iFrost = 0; iFrost < options.Nfrost; iFrost ++){
                            ice += all_vars[iCell].cell[iVeg][iBand].layer[iLayer].ice[iFrost] *
                                   soil_con[iCell].frost_fract[iFrost];
                        }
                        
                        if(all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist < ice){
                            all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist = ice;
                        }
                    }
                }
            }
        }
        
        // surface
        rout_var[iCell].discharge -= 
                (wu_var[iCell][iSector].withdrawn_surf + 
                wu_var[iCell][iSector].withdrawn_remote - 
                wu_var[iCell][iSector].returned) / 
                MM_PER_M * 
                local_domain.locations[iCell].area / 
                global_param.dt;
        if(rout_var[iCell].discharge < 0){
            rout_var[iCell].discharge = 0;
        }
        
        if(wu_var[iCell][iSector].withdrawn_gw - wu_var[iCell][iSector].available_gw > DBL_EPSILON ||
                wu_var[iCell][iSector].withdrawn_gw - wu_var[iCell][iSector].demand_gw > DBL_EPSILON ||
                wu_var[iCell][iSector].withdrawn_surf + wu_var[iCell][iSector].withdrawn_remote - wu_var[iCell][iSector].available_surf > DBL_EPSILON ||
                wu_var[iCell][iSector].withdrawn_surf + wu_var[iCell][iSector].withdrawn_remote - wu_var[iCell][iSector].demand_surf > DBL_EPSILON){
            log_err("Water-use water balance error:\n"
                    "groundwater\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "surface-water\twithdrawn [%.4f]\tremote[%.4f]\tdemand [%.4f]\tavailable [%.4f]",
                    wu_var[iCell][iSector].withdrawn_gw, 
                    wu_var[iCell][iSector].demand_gw, 
                    wu_var[iCell][iSector].available_gw,
                    wu_var[iCell][iSector].withdrawn_surf, 
                    wu_var[iCell][iSector].withdrawn_remote, 
                    wu_var[iCell][iSector].demand_surf, 
                    wu_var[iCell][iSector].available_surf);
        }
    }
    
    
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        free(av_gw[iVeg]);
    }
    free(av_gw);
}