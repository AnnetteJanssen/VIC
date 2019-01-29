#include <vic.h>

void
dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern global_param_struct global_param;
    
    size_t years_running;
    
    double release_int;
    double storage_int;
    short unsigned int month_days;
    
    years_running = (size_t)(dam_var->months_running / 
            MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }

    if(dmy[current].year >= dam_con->year && years_running > 0){

        // Interpolate storage and release
        month_days = days_per_month(dmy[current].month, dmy[current].year, global_param.calendar);
        release_int = linear_interp(
                dmy[current].day * global_param.model_steps_per_day,
                1,
                month_days * global_param.model_steps_per_day,
                dam_var->op_release[0],
                dam_var->op_release[1]);
        storage_int = linear_interp(
                dmy[current].day * global_param.model_steps_per_day,
                1,
                month_days * global_param.model_steps_per_day,
                dam_var->op_storage[0],
                dam_var->op_storage[1]);
        
        // Fill reservoir
        dam_var->storage += dam_var->inflow;

        // Calculate discharge               
        dam_var->release = dam_corr_release(release_int,
                                            dam_var->storage,
                                            storage_int);
        
        // reduce reservoir
        dam_var->storage -= dam_var->release;

        // Empty reservoir
        if(dam_var->storage < 0){
            dam_var->release -= dam_var->storage;
            dam_var->storage = 0.0;
        }

        // Full reservoir
        if(dam_var->storage > dam_con->capacity){
            dam_var->release += dam_var->storage - dam_con->capacity;            
            dam_var->storage = dam_con->capacity;
        }
    }
}

void
local_dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern domain_struct local_domain;
    extern double ***out_data;
    
    dam_operate(dam_con, dam_var);
    out_data[cur_cell][OUT_RUNOFF][0] *= 1 - dam_con->inflow_frac;
    out_data[cur_cell][OUT_RUNOFF][0] += dam_var->release * M3_PER_HM3 * 
            MM_PER_M / local_domain.locations[cur_cell].area;
}

void
global_dam_operate(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern global_param_struct global_param;
    extern rout_var_struct *rout_var;
    
    dam_operate(dam_con, dam_var);
    rout_var[cur_cell].discharge *= 1 - dam_con->inflow_frac;
    rout_var[cur_cell].discharge += dam_var->release * M3_PER_HM3 / 
            global_param.dt;
}

void
local_dam_run(size_t cur_cell){
    extern dam_con_struct *local_dam_con;
    extern dam_var_struct *local_dam_var;
    
    if(local_dam_con[cur_cell].run){
        local_dam_history(&local_dam_con[cur_cell], &local_dam_var[cur_cell], cur_cell);
        
        if(local_dam_var[cur_cell].active){
            local_dam_operate(&local_dam_con[cur_cell], &local_dam_var[cur_cell], cur_cell);
        }
    }
}

void
global_dam_run(size_t cur_cell){
    extern dam_con_struct *global_dam_con;
    extern dam_var_struct *global_dam_var;
    
    if(global_dam_con[cur_cell].run){
        local_dam_history(&global_dam_con[cur_cell], &global_dam_var[cur_cell], cur_cell);
        
        if(global_dam_var[cur_cell].active){
            local_dam_operate(&global_dam_con[cur_cell], &global_dam_var[cur_cell], cur_cell);
        }
    }
}
