#include <vic_driver_image.h>
#include <plugin.h>

void
dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var)
{
    double prev_storage;

    // Fill reservoir
    prev_storage = dam_var->storage;
    dam_var->storage += dam_var->inflow;

    // Calculate discharge
    dam_var->release = dam_var->op_release[0];
    dam_var->release = dam_corr_release(dam_var->op_release[0],
                                        dam_var->storage,
                                        dam_con->capacity);

    // reduce reservoir
    dam_var->storage -= dam_var->release;

    // Empty reservoir
    if(dam_var->storage < 0){
        dam_var->release += dam_var->storage;
        dam_var->storage = 0.0;
    }

    // Full reservoir
    if(dam_var->storage > dam_con->capacity){
        dam_var->release += dam_var->storage - dam_con->capacity;            
        dam_var->storage = dam_con->capacity;
    }

    if(abs(dam_var->storage - prev_storage - 
            dam_var->inflow + dam_var->release) > DBL_EPSILON){
        log_err("Dam operation error:\n"
                "Prev storage [%.4f]\tStorage [%.4f]\t"
                "Inflow [%.4f]\tRelease [%.4f]\t",
                prev_storage, dam_var->storage,
                dam_var->inflow, dam_var->release);
    }
}

void
local_dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t iCell)
{
    extern domain_struct local_domain;
    extern double ***out_data;
    
    dam_operate(dam_con, dam_var);
    out_data[iCell][OUT_RUNOFF][0] *= 1 - dam_con->inflow_frac;
    out_data[iCell][OUT_RUNOFF][0] += dam_var->release * M3_PER_HM3 * 
            MM_PER_M / local_domain.locations[iCell].area;
}

void
global_dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t iCell)
{
    extern global_param_struct global_param;
    extern rout_var_struct *rout_var;
    
    dam_operate(dam_con, dam_var);
    rout_var[iCell].discharge *= 1 - dam_con->inflow_frac;
    rout_var[iCell].discharge += dam_var->release * M3_PER_HM3 / 
            global_param.dt;
}

void
local_dam_run(size_t iCell){
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_con_struct **local_dam_con;
    extern dam_var_struct **local_dam_var;
    
    size_t i;
    
    for(i = 0; i < plugin_options.NDAMTYPES; i++){
        if(local_dam_con_map[iCell].didx[i] != NODATA_DAM){
            local_dam_register(&local_dam_con[iCell][i], &local_dam_var[iCell][i], iCell);

            if(local_dam_var[iCell][i].active){
                local_dam_operate(&local_dam_con[iCell][i], &local_dam_var[iCell][i], iCell);
            }
        }
    }
}

void
global_dam_run(size_t iCell){
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *global_dam_con_map;
    extern dam_con_struct **global_dam_con;
    extern dam_var_struct **global_dam_var;
    
    size_t i;
    
    for(i = 0; i < plugin_options.NDAMTYPES; i++){
        if(global_dam_con_map[iCell].didx[i] != NODATA_DAM){
            global_dam_register(&global_dam_con[iCell][i], &global_dam_var[iCell][i], iCell);

            if(global_dam_var[iCell][i].active){
                global_dam_operate(&global_dam_con[iCell][i], &global_dam_var[iCell][i], iCell);
            }
        }
    }
}
