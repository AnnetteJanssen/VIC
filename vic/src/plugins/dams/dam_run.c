#include <vic.h>

void
dam_calc_year_discharge(double ay_flow, 
        double *am_flow, 
        double amplitude, 
        double offset,
        double *op_dicharge)
{
    size_t i;
    
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_dicharge[i] = ay_flow + ((am_flow[i] - ay_flow) * amplitude) + offset;
        if(op_dicharge[i] < 0){
            op_dicharge[i] = 0.0;
        }
    }
}

void
dam_calc_irr_year_discharge(double ay_flow, 
        double *am_flow,
        double ay_demand,
        double *am_demand,
        double amplitude,
        double offset,
        double *op_dicharge)
{
    size_t i;
    
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_dicharge[i] = (ay_flow + ((am_flow[i] - ay_flow) * amplitude)) * 
                         ((am_demand[i] / ay_demand) * (1 - amplitude)) + offset;
        if(op_dicharge[i] < 0){
            op_dicharge[i] = 0.0;
        }
    }
}


void
dam_calc_year_volume(double pref_volume, 
        double *am_flow, 
        double *op_discharge,
        int op_year,  
        double *op_volume)
{    
    extern global_param_struct global_param;
    
    int month_nr;
    
    size_t i;
    
    // Calculate operational volume
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        month_nr = (i + op_year) % MONTHS_PER_YEAR;
        
        if(i == 0){
                op_volume[month_nr] = pref_volume;
        } else if (month_nr==0){
            op_volume[month_nr] = op_volume[MONTHS_PER_YEAR - 1] + 
                    ((am_flow[month_nr] - op_discharge[month_nr]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        }else {
            op_volume[month_nr] = op_volume[month_nr - 1] + 
                    ((am_flow[month_nr] - op_discharge[month_nr]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        }
    }
}

int
dam_get_op_year_flow(double ay_flow, double *am_flow)
{
    double con_inflow;
    double max_con_inflow;
    size_t month_nr;
    size_t month_add;

    size_t i;

    con_inflow = 0.0;
    max_con_inflow = 0.0;
    month_add = 0;

    for(i = 0; i < 2 * MONTHS_PER_YEAR; i++){
        month_nr = i % MONTHS_PER_YEAR;

        if(am_flow[month_nr] > ay_flow){
            con_inflow += am_flow[month_nr];

            if(con_inflow > max_con_inflow){
                max_con_inflow = con_inflow;
                month_add = month_nr;
            }
        }else{
            con_inflow = 0.0;
        }
    }            
    month_add++;
    
    return month_add % MONTHS_PER_YEAR;   
}

void
dam_get_operation_flo(double ay_flow, 
        double *am_flow, 
        double pref_volume, 
        double min_volume,
        int op_year,
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double amplitude;
    double volume_needed;
    
    size_t i;    
    
    // Set operational discharge
    for(amplitude = 0; amplitude < 1; amplitude += DAM_AMP_STEP){
        dam_calc_year_discharge(ay_flow, am_flow, amplitude, 0.0, op_discharge);
        
        volume_needed = 0.0;
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            if(op_discharge[i] < am_flow[i]){
                volume_needed += (am_flow[i] - op_discharge[i]) * 
                        global_param.dt * 
                        global_param.model_steps_per_day * 
                        DAYS_PER_MONTH_AVG;
            }
        }
         
        if(volume_needed < pref_volume - min_volume){
            break;
        }
    }    
    
    if(volume_needed > pref_volume - min_volume){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i];
        }
    }
    
    dam_calc_year_volume(pref_volume, am_flow, op_discharge, op_year, op_volume);
}

void
dam_get_operation_irr(double ay_flow, 
        double *am_flow, 
        double ay_demand,
        double *am_demand,
        double pref_volume, 
        double min_volume,
        int op_year,
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double amplitude;
    double volume_needed;
    
    size_t i;    
    
    // Set operational discharge
    for(amplitude = 0; amplitude < 1; amplitude += DAM_AMP_STEP){
        dam_calc_irr_year_discharge(ay_flow, am_flow, ay_demand, am_demand, amplitude, 0.0, op_discharge);
        
        volume_needed = 0.0;
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            if(op_discharge[i] < am_flow[i]){
                volume_needed += (am_flow[i] - op_discharge[i]) * 
                        global_param.dt * 
                        global_param.model_steps_per_day * 
                        DAYS_PER_MONTH_AVG;
            }
        }
         
        if(volume_needed < pref_volume - min_volume){
            break;
        }
    }    
    
    if(volume_needed > pref_volume - min_volume){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i];
        }
    }  
    
    dam_calc_year_volume(pref_volume, am_flow, op_discharge, op_year, op_volume);
}

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
dam_history(size_t cur_cell, size_t cur_dam)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    double ay_flow;
    double ay_demand;
    double am_flow[MONTHS_PER_YEAR];
    double am_demand[MONTHS_PER_YEAR];
    double am_shortage[MONTHS_PER_YEAR];
    
    size_t i;
        
    years_running = (size_t)(dam_var[cur_cell][cur_dam].months_running / 
            MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }

    // Shift array
    cshift(dam_var[cur_cell][cur_dam].history_flow, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var[cur_cell][cur_dam].history_demand, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var[cur_cell][cur_dam].history_shortage, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var[cur_cell][cur_dam].op_discharge, MONTHS_PER_YEAR, 1, 0, 1);
    cshift(dam_var[cur_cell][cur_dam].op_volume, MONTHS_PER_YEAR, 1, 0, 1);
    
    // Store monthly average
    dam_var[cur_cell][cur_dam].history_flow[0] =
            dam_var[cur_cell][cur_dam].total_flow / 
            dam_var[cur_cell][cur_dam].total_steps;
    dam_var[cur_cell][cur_dam].history_demand[0] =
            dam_var[cur_cell][cur_dam].total_demand;
    dam_var[cur_cell][cur_dam].history_shortage[0] =
            dam_var[cur_cell][cur_dam].total_shortage;
    dam_var[cur_cell][cur_dam].total_flow = 0.0;
    dam_var[cur_cell][cur_dam].total_demand = 0.0;
    dam_var[cur_cell][cur_dam].total_shortage = 0.0;
    dam_var[cur_cell][cur_dam].total_steps = 0;       

    // Calculate averages            
    if(current > 0 && dmy[current].year != dmy[current - 1].year){
        ay_flow = array_average(dam_var[cur_cell][cur_dam].history_flow,
            years_running, MONTHS_PER_YEAR, 0, 0);
        ay_demand = array_average(dam_var[cur_cell][cur_dam].history_demand,
            years_running, MONTHS_PER_YEAR, 0, 0);
        for(i = 0; i < MONTHS_PER_YEAR; i++){
            am_flow[i] = array_average(dam_var[cur_cell][cur_dam].history_flow,
            years_running, 1, i, MONTHS_PER_YEAR - i - 1);
            am_demand[i] = array_average(dam_var[cur_cell][cur_dam].history_demand,
            years_running, 1, i, MONTHS_PER_YEAR - i - 1);
            am_shortage[i] = array_average(dam_var[cur_cell][cur_dam].history_shortage,
            years_running, 1, i, MONTHS_PER_YEAR - i - 1);
        }

        double_flip(am_flow,MONTHS_PER_YEAR);
        double_flip(am_demand,MONTHS_PER_YEAR);
        double_flip(am_shortage,MONTHS_PER_YEAR);
        
        // Calculate operational year, discharge and volume
        if(dam_con[cur_cell][cur_dam].function == DAM_FUN_IRR){
            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_flow(ay_flow, am_flow);
        
            dam_get_operation_irr(ay_flow, am_flow,
                    ay_demand, am_demand,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MIN_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_year,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);              
        }else {
            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_flow(ay_flow, am_flow);
        
            dam_get_operation_flo(ay_flow, am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MIN_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_year,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);            
        }
    } 
}

void
dam_total(size_t cur_cell, size_t cur_dam)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    extern dam_var_struct **dam_var;
    extern rout_var_struct *rout_var;
    extern irr_con_map_struct *irr_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern irr_var_struct ***irr_var;
    extern dam_con_struct **dam_con;
        
    size_t other_cell;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    
    dam_var[cur_cell][cur_dam].total_flow += 
            rout_var[cur_cell].discharge[0];
    
    if(options.IRRIGATION){
        for(i = 0; i < dam_con[cur_cell][cur_dam].nservice; i++){
            other_cell = dam_con[cur_cell][cur_dam].service[i];

            for(j = 0; j < irr_con_map[other_cell].ni_active; j++){
                cur_veg = irr_con[other_cell][j].veg_index;
            
                for(k = 0; k < elev_con_map[other_cell].ne_active; k++){

                    dam_var[cur_cell][cur_dam].total_demand +=
                            irr_var[other_cell][j][k].need / MM_PER_M * 
                            (local_domain.locations[other_cell].area *
                            soil_con[other_cell].AreaFract[k] * 
                            veg_con[other_cell][cur_veg].Cv) / 
                            global_param.dt;
                }
            }
        }
    }
    
    dam_var[cur_cell][cur_dam].total_steps++;
}

void
dam_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern global_param_struct global_param;
    extern option_struct options;
    extern dam_var_struct **dam_var;
    extern rout_var_struct *rout_var;
    extern efr_hist_struct *efr_hist;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    
    size_t i;
    double tmp_volume;
    double tmp_volume_frac;
    double vol_discharge_modifier;
    double op_year_discharge_modifier;
    int op_year_dist;
    
    for(i = 0; i < dam_con_map[cur_cell].nd_active; i++){
        
        if(current > 0 && dmy[current].month != dmy[current-1].month){
            dam_var[cur_cell][i].months_running++;
            if(dam_var[cur_cell][i].months_running > 
                    DAM_HIST_YEARS * MONTHS_PER_YEAR){
                dam_var[cur_cell][i].months_running =
                        DAM_HIST_YEARS * MONTHS_PER_YEAR;
            }
        }

        if(current > 0 && dmy[current].month != dmy[current-1].month){
            dam_history(cur_cell, i);
        }
        
        dam_total(cur_cell,i);
        
        years_running = (size_t)(dam_var[cur_cell][i].months_running / 
                MONTHS_PER_YEAR);
        if(years_running > DAM_HIST_YEARS){
            years_running = DAM_HIST_YEARS;
        }

        // Run dams
        if(dmy[current].year >= dam_con[cur_cell][i].year && years_running > 0){
            
            // Fill reservoir
            dam_var[cur_cell][i].volume += 
                    rout_var[cur_cell].discharge[0] * 
                    global_param.dt;       
            
            // Calculate volume at end of step
            tmp_volume = dam_var[cur_cell][i].volume;
            if(dam_con[cur_cell][i].function == DAM_FUN_HYD){
                tmp_volume -= rout_var[cur_cell].discharge[0] * 
                    global_param.dt;
            } else if(dam_con[cur_cell][i].function == DAM_FUN_IRR){ 
                tmp_volume -= rout_var[cur_cell].discharge[0] * 
                    global_param.dt;
            } else if (dam_con[cur_cell][i].function == DAM_FUN_FLO){  
                tmp_volume -= dam_var[cur_cell][i].op_discharge[0];
            }
            if(tmp_volume > dam_con[cur_cell][i].max_volume){
                tmp_volume = dam_con[cur_cell][i].max_volume;
            }         
            
            // Calculate volume fraction
            tmp_volume_frac = tmp_volume / dam_con[cur_cell][i].max_volume;
            
            // Calculate discharge modifier based on volume
            vol_discharge_modifier = tmp_volume_frac;
            if(tmp_volume_frac < 0.5){
                vol_discharge_modifier = 1 - tmp_volume_frac;
            }
            vol_discharge_modifier = pow((vol_discharge_modifier - 0.5) * 2, 2);
            
            // Calculate discharge modifier based on operational year
            if(dmy[current].month >= dam_var[cur_cell][i].op_year){
                op_year_dist = MONTHS_PER_YEAR - 
                        (dmy[current].month - dam_var[cur_cell][i].op_year) - 1;
            } else {
                op_year_dist = dam_var[cur_cell][i].op_year - 
                        dmy[current].month - 1;
            }
            op_year_discharge_modifier = pow(1 - 
                    ((double)op_year_dist / (double)MONTHS_PER_YEAR), 2);
            
            // Calculate discharge
            if (dam_con[cur_cell][i].function == DAM_FUN_IRR) {                
                dam_var[cur_cell][i].discharge = 
                    rout_var[cur_cell].discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC * vol_discharge_modifier,  
                    dam_var[cur_cell][i].total_steps,
                    tmp_volume);
            } else {                
                dam_var[cur_cell][i].discharge = 
                    dam_var[cur_cell][i].op_discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC * vol_discharge_modifier,   
                    dam_var[cur_cell][i].total_steps,
                    tmp_volume);
            }      
            
            if(dam_var[cur_cell][i].discharge < 0){
                dam_var[cur_cell][i].discharge = 0.0;
            }
            
            // Consider EFR
            if(options.EFR){
                if(dam_var[cur_cell][i].discharge <
                        efr_hist[cur_cell].requirement_discharge){
                    dam_var[cur_cell][i].discharge =
                        efr_hist[cur_cell].requirement_discharge;
                }
            }  
            
            // Release
            dam_var[cur_cell][i].volume -= 
                    dam_var[cur_cell][i].discharge * 
                    global_param.dt;
            if(dam_var[cur_cell][i].volume < 0){
                dam_var[cur_cell][i].discharge -= 
                        dam_var[cur_cell][i].volume / 
                        global_param.dt;
                dam_var[cur_cell][i].volume = 0.0;
            }
            
            // Overflow
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
            rout_var[cur_cell].discharge[0] = 0.0;                          
            rout_var[cur_cell].discharge[0] += 
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