#include <vic.h>

double
dam_calc_discharge(double ay_flow, 
        double am_flow, 
        double amplitude, 
        double offset){
    
    double discharge = 0.0;
    
    discharge = ay_flow + ((am_flow - ay_flow) * amplitude) + offset;
    if(discharge < 0){
        discharge = 0.0;
    }
    
    return discharge;
}

void
dam_calc_year_discharge(double ay_flow, 
        double *am_flow, 
        double amplitude, 
        double offset,
        double *op_dicharge)
{
    size_t i;
    
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_dicharge[i] = dam_calc_discharge(ay_flow, am_flow[i], amplitude, offset);
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

int
dam_get_op_year_demand(double *am_demand)
{
    double con_demand;
    double max_con_demand;
    size_t month_start;
    size_t month_nr;
    size_t month_add;

    size_t i;

    con_demand = 0.0;
    max_con_demand = 0.0;
    month_add = 0;
    
    for(i = 0; i < 2 * MONTHS_PER_YEAR; i++){
        month_nr = i % MONTHS_PER_YEAR;

        if(am_demand[month_nr] > 0){
            if(month_start == MISSING_USI){
                month_start = month_nr;
            }
            
            con_demand += am_demand[month_nr];

            if(con_demand > max_con_demand){
                max_con_demand = con_demand;
                month_add = month_start;
            }
        }else{
            con_demand = 0.0;
            month_start = MISSING_USI;
        }
    }
    
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
    int month_nr;
    
    size_t i;    
    
    // Set operational discharge
    for(amplitude = EFR_HIGH_DEMAND_FRAC; amplitude < 1; amplitude += DAM_AMP_STEP){
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
    
    // Set operational volume
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

void
dam_get_operation_hyd(double *am_flow,
        double pref_volume,
        double *op_discharge, 
        double *op_volume)
{        
    size_t i;
    
    // Set operational discharge
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_discharge[i] = am_flow[i];
    }
    
    
    // Set operational volume
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        op_volume[i] = pref_volume;
    }
}

void
dam_get_operation_irr(double *am_flow,
        double ay_demand,
        double *am_demand,
        double *am_shortage,
        double pref_volume,
        double min_volume,
        int op_year,
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
      
    double tot_demand;
    double tot_flow;
    double frac;
    double max_volume;
    double adj_volume;
    
    int month_nr;
    size_t i;
    
    // Calculate availability and shortage
    tot_demand = 0.0;
    tot_flow = 0.0;
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        if(am_demand[i] <= ay_demand){
            tot_flow += am_flow[i] * (1 - EFR_LOW_DEMAND_FRAC);
        } else {
            tot_demand += am_demand[i];
        }
    }
    
    frac =  (tot_flow * SEC_PER_DAY * DAYS_PER_MONTH_AVG) /
            (pref_volume - min_volume);
    if(frac > 1.0){
        frac = 1.0;
    }
    
    // Calculate amount to save and redistribute
    if(tot_flow > 0 && tot_demand > 0){
        for(i = 0; i < MONTHS_PER_YEAR * 2; i++){
            month_nr = i % MONTHS_PER_YEAR;

            if(am_demand[i] <= ay_demand){
                op_discharge[i] = am_flow[i] - 
                        (pref_volume - min_volume) * frac / 
                        (DAYS_PER_MONTH_AVG * SEC_PER_DAY) * 
                        (am_flow[i] / tot_flow);
            } else {
                op_discharge[i] = am_flow[i] + 
                        (pref_volume - min_volume) * frac / 
                        (DAYS_PER_MONTH_AVG * SEC_PER_DAY) * 
                        (am_demand[i] / tot_demand);
            }        
        }
    } else {
        op_discharge[i] = am_flow[i];
    }
    
    // Set operational volume
    max_volume = 0.0;
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        month_nr = (i + op_year - 1);
        if(month_nr < 0){
            month_nr += MONTHS_PER_YEAR;
        }
        month_nr = month_nr % MONTHS_PER_YEAR;
        
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
        
        if(op_volume[month_nr] > max_volume){
            max_volume = op_volume[month_nr];
        }
    }
    
    if(max_volume > pref_volume){
        adj_volume = pref_volume - max_volume;
    }
    
    // Set operational volume
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        op_volume[i] += adj_volume;
        if(op_volume[i] < min_volume){
            op_volume[i] = min_volume;
        }
    }    
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
    if(abs(discharge_correction) > max_correction){
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
    cshift(dam_var[cur_cell][cur_dam].history_flow, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].history_demand, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].history_shortage, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].op_discharge, 1, MONTHS_PER_YEAR, 1, 1);
    cshift(dam_var[cur_cell][cur_dam].op_volume, 1, MONTHS_PER_YEAR, 1, 1);
    
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
        
        // Calculate operation discharge and volume
        if(dam_con[cur_cell][cur_dam].function == DAM_FUN_IRR){
            // Recalculate operational year
            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_demand(am_shortage);
        
            dam_get_operation_irr(am_flow, 
                    ay_demand, am_demand, 
                    am_shortage,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MIN_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_year,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);            
        }else if(dam_con[cur_cell][cur_dam].function == DAM_FUN_FLO){    
            // Calculate operational year
            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_flow(ay_flow, am_flow);
        
            dam_get_operation_flo(ay_flow, am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MIN_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_year,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);            
        }else if(dam_con[cur_cell][cur_dam].function == DAM_FUN_HYD){ 
            // Calculate operational year
            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_flow(ay_flow, am_flow);
        
            dam_get_operation_hyd(am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);  
            
        }else{
            log_err("Unknown dam function");
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
    extern irr_var_struct ***irr_var;
    extern dam_con_struct **dam_con;
        
    size_t other_cell;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    
    dam_var[cur_cell][cur_dam].total_flow += 
            rout_var[cur_cell].nat_discharge[0];
    
    if(options.IRRIGATION){
        for(i = 0; i < dam_con[cur_cell][cur_dam].nservice; i++){
            other_cell = dam_con[cur_cell][cur_dam].service[i];

            for(j = 0; j < irr_con_map[other_cell].ni_active; j++){
                cur_veg = irr_con[cur_cell][i].veg_index;
            
                for(k = 0; k < options.SNOW_BAND; k++){

                    dam_var[cur_cell][cur_dam].total_demand +=
                            irr_var[other_cell][j][k].need / MM_PER_M * 
                            (local_domain.locations[cur_cell].area *
                            soil_con[cur_cell].AreaFract[j] * 
                            veg_con[cur_cell][cur_veg].Cv) / 
                            global_param.dt;
                    dam_var[cur_cell][cur_dam].total_shortage +=
                            irr_var[other_cell][j][k].deficit / MM_PER_M * 
                            (local_domain.locations[cur_cell].area *
                            soil_con[cur_cell].AreaFract[j] * 
                            veg_con[cur_cell][cur_veg].Cv) / 
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
    extern efr_var_struct *efr_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    
    size_t i;
    double tmp_volume;
    double tmp_volume_frac;
    double tmp_discharge_modifier;
    
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
            
            tmp_volume = dam_var[cur_cell][i].volume;
            if(tmp_volume > dam_con[cur_cell][i].max_volume){
                tmp_volume = dam_con[cur_cell][i].max_volume;
            }  
            
            tmp_volume_frac = tmp_volume / dam_con[cur_cell][i].max_volume;
            
            tmp_discharge_modifier = tmp_volume_frac;
            if(tmp_volume_frac < 0.5){
                tmp_discharge_modifier = 1 - tmp_volume_frac;
            }
            tmp_discharge_modifier -= 0.5;
            tmp_discharge_modifier *= 2;
            
            // Calculate discharge
            if(dam_con[cur_cell][i].function == DAM_FUN_HYD){
                dam_var[cur_cell][i].discharge = 
                    rout_var[cur_cell].discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC * tmp_discharge_modifier,
                    dam_var[cur_cell][i].total_steps,
                   tmp_volume);
            } else if(dam_con[cur_cell][i].function == DAM_FUN_IRR){                
                dam_var[cur_cell][i].discharge = 
                    dam_var[cur_cell][i].op_discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],    
                    dam_con[cur_cell][i].max_volume * 
                    DAM_DIS_MOD_FRAC_IRR * tmp_discharge_modifier,                
                    dam_var[cur_cell][i].total_steps,
                    tmp_volume);
            } else if (dam_con[cur_cell][i].function == DAM_FUN_FLO){                
                dam_var[cur_cell][i].discharge = 
                    dam_var[cur_cell][i].op_discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC * tmp_discharge_modifier,   
                    dam_var[cur_cell][i].total_steps,
                    tmp_volume);
            }else{
                log_err("Unknown dam function");
            }        
            
            if(dam_var[cur_cell][i].discharge < 0){
                dam_var[cur_cell][i].discharge = 0.0;
            }
            
            // Consider EFR
            if(options.EFR){
                if(dam_var[cur_cell][i].discharge <
                        efr_var[cur_cell].requirement){
                    dam_var[cur_cell][i].discharge =
                        efr_var[cur_cell].requirement;
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
            
            // Recalculate dam info
            dam_var[cur_cell][i].area = 
                    dam_area(dam_var[cur_cell][i].volume,
                    dam_con[cur_cell][i].max_volume,
                    dam_con[cur_cell][i].max_area,
                    dam_con[cur_cell][i].max_height);
            dam_var[cur_cell][i].height = 
                    dam_height(dam_var[cur_cell][i].area,
                    dam_con[cur_cell][i].max_height);
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