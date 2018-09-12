#include <vic.h>

double
dam_discharge_correction(double prev_op_volume, double next_op_volume, double max_correction, int steps, double volume)
{
    extern global_param_struct global_param;
    
    double calc_volume;
    double day_step;
    double discharge_correction;
    
    // Calculate expected volume
    day_step = steps / 
            global_param.model_steps_per_day;
    if(day_step > (size_t)DAYS_PER_MONTH_AVG){
        day_step = DAYS_PER_MONTH_AVG;
    }
    calc_volume = linear_interp(day_step,
            0, (size_t)DAYS_PER_MONTH_AVG,
            prev_op_volume,
            next_op_volume);

    // Calculate discharge correction
    discharge_correction = 
            (volume - calc_volume) / (global_param.dt * 
            global_param.model_steps_per_day * DAYS_PER_WEEK);
    if(fabs(discharge_correction) > max_correction){
        if(discharge_correction > 0){
            discharge_correction = 
                    max_correction;
        } else {
            discharge_correction = 
                    -max_correction;
        }
    }
    
    return discharge_correction;
}

void
dam_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern global_param_struct global_param;
    extern dam_var_struct **dam_var;
    extern rout_var_struct *rout_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    
    size_t i;
    double discharge_mod;
    
    for(i = 0; i < dam_con_map[cur_cell].nd_active; i++){
        years_running = (size_t)(dam_var[cur_cell][i].months_running / 
                MONTHS_PER_YEAR);
        if(years_running > DAM_HIST_YEARS){
            years_running = DAM_HIST_YEARS;
        }
        
        if(dmy[current].year >= dam_con[cur_cell][i].year && years_running > 0){
            
            // Calculate discharge modifier
            discharge_mod = dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_var[cur_cell][i].op_discharge[0] * DAM_MOD_DIS_FRAC / 
                    global_param.model_steps_per_day,  
                    //dam_var[cur_cell][i].volume * DAM_MOD_VOL_FRAC / 
                    //global_param.model_steps_per_day,
                    dam_var[cur_cell][i].total_steps,
                    dam_var[cur_cell][i].volume);   
            
            // Fill reservoir
            dam_var[cur_cell][i].volume += dam_var[cur_cell][i].inflow * 
                    global_param.dt;
            
            // Calculate discharge               
            dam_var[cur_cell][i].discharge = 
                dam_var[cur_cell][i].op_discharge[0] + 
                    discharge_mod;
                
            
            if(dam_var[cur_cell][i].discharge < 0){
                dam_var[cur_cell][i].discharge = 0.0;
            }
            
            dam_var[cur_cell][i].volume -= 
                    dam_var[cur_cell][i].discharge * 
                    global_param.dt;
            
            // Empty reservoir
            if(dam_var[cur_cell][i].volume < 0){
                dam_var[cur_cell][i].discharge -= 
                        dam_var[cur_cell][i].volume / 
                        global_param.dt;
                dam_var[cur_cell][i].volume = 0.0;
            }
            
            // Full reservoir
            if(dam_var[cur_cell][i].volume >
                    dam_con[cur_cell][i].max_volume){
                dam_var[cur_cell][i].discharge +=
                        (dam_var[cur_cell][i].volume -
                        dam_con[cur_cell][i].max_volume) / 
                        global_param.dt;            
                dam_var[cur_cell][i].volume = 
                        dam_con[cur_cell][i].max_volume;
            }
            
            // Discharge
            rout_var[cur_cell].discharge = 0.0;                          
            rout_var[cur_cell].discharge += 
                    dam_var[cur_cell][i].discharge;
        }
    }
}

double
dam_area(double volume, double max_volume, double max_area, double max_height){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * max_volume ) / ( max_height * max_area );
    double area = max_area * pow(( volume / max_volume ), ((2-N)/N));
    
    if(area > max_area){
        area=max_area;
    }
    
    return area;        
}

double
dam_height(double area, double max_height){
    // Calculate dam height based on Liebe et al 2005
    double height = sqrt(area) * (1 / 19.45);
    
    if(height > max_height){
        height = max_height;
    }
    
    return height;
}