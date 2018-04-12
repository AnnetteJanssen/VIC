#include <vic.h>

int
dam_get_op_year_flo(double ay_flow, double *am_flow, int current_month)
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
    return (current_month + month_add) % MONTHS_PER_YEAR;   
}

int
dam_get_op_year_hyd(int current_month)
{
    return current_month;   
}

int
dam_get_op_year_irr(double *am_flow, double *op_discharge, int current_month)
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

        if(am_flow[month_nr] - op_discharge[month_nr] > 0){
            con_inflow += am_flow[month_nr] - op_discharge[month_nr];

            if(con_inflow > max_con_inflow){
                max_con_inflow = con_inflow;
                month_add = month_nr;
            }
        }else{
            con_inflow = 0.0;
        }
    }    
    month_add++;  
    return (current_month + month_add) % MONTHS_PER_YEAR; 
}

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

void
dam_get_operation_flo(double ay_flow, 
        double *am_flow, 
        double cur_volume, 
        double pref_volume, 
        double min_volume,
        double max_volume, 
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double amplitude;
    double offset;
    double volume_needed;
    double cur_volume_tmp;
    
    size_t i;
    
    cur_volume_tmp = cur_volume;
    if(cur_volume_tmp > max_volume){
        cur_volume_tmp = max_volume;
    }
    
    // Set offset for inter-annual storage changes
    offset = (cur_volume_tmp - pref_volume) /
            (DAYS_PER_YEAR * 
            global_param.model_steps_per_day *
            global_param.dt);
    
    // Set operational discharge
    for(amplitude = EFR_HIGH_DEMAND_FRAC; amplitude < 1; amplitude += DAM_AMP_STEP){
        dam_calc_year_discharge(ay_flow, am_flow, amplitude, offset, op_discharge);
        
        volume_needed = 0.0;
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            if(op_discharge[i] > am_flow[i]){
                volume_needed += (op_discharge[i] - am_flow[i]) * 
                        global_param.dt * 
                        global_param.model_steps_per_day * 
                        DAYS_PER_MONTH_AVG;
            }
        }
         
        if(volume_needed < max_volume - min_volume){
            break;
        }
    }    
    
    if(volume_needed > max_volume - min_volume){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i] + offset;
        }
    }
    
    // Set operational volume
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        if(i == 0){
            op_volume[i] = pref_volume + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        } else if(i == MONTHS_PER_YEAR - 1) {
            op_volume[i] = pref_volume;
        } else {
            op_volume[i] = op_volume[i-1] + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        }
        
        if(op_volume[i] < min_volume){
            op_volume[i] = min_volume;
        }else if(op_volume[i] > max_volume){
            op_volume[i] = max_volume;
        }
    }
}

void
dam_get_operation_hyd(double *am_flow, 
        double cur_volume, 
        double pref_volume, 
        double max_volume, 
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double offset;
    double cur_volume_tmp;
    
    size_t i;
    
    cur_volume_tmp = cur_volume;
    if(cur_volume_tmp > max_volume){
        cur_volume_tmp = max_volume;
    }
    
    // Set offset for inter-annual storage changes
    offset = (cur_volume_tmp - pref_volume) /
            (DAYS_PER_YEAR * 
            global_param.model_steps_per_day *
            global_param.dt);
    
    // Set operational discharge
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_discharge[i] = am_flow[i] + offset;
    }
    
    // Set operational volume
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        op_volume[i] = pref_volume;
    }
}

void
dam_get_operation_irr(double *am_flow,
        double *am_demand,
        double *am_shortage,
        double cur_volume,
        double pref_volume,
        double max_volume, 
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double cur_volume_tmp;    
    double total_shortage;
    double total_avail;
    double shortage_fraction;
    
    int month_nr;
    size_t i;
    
    total_avail = 0.0;
    total_shortage = 0.0;
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        total_avail += am_flow[i] - am_demand[i] * 3;
        total_shortage += am_shortage[i] * 1.5;
    }
    
    if(total_avail > 0.0){
        shortage_fraction = total_shortage / total_avail;
    }else{
        shortage_fraction = 0.0;
    }
       
    for(i = MONTHS_PER_YEAR * 2 - 1; (i+1) >= 1 ; i--){
        month_nr = i % MONTHS_PER_YEAR;

        if(am_shortage[month_nr] > 0.0){
            if(i < MONTHS_PER_YEAR){
                break;
            }else {
                if(shortage_fraction > 1){
                    op_discharge[month_nr] = am_flow[month_nr] + 
                            (am_shortage[month_nr] * 1.5) *
                            (1/shortage_fraction);
                } else {
                    op_discharge[month_nr] = am_flow[month_nr] + 
                            (am_shortage[month_nr] * 1.5);
                }
            }
        }else {
            if(shortage_fraction > 1){
                op_discharge[month_nr] = am_flow[month_nr] - 
                        (am_flow[i] - am_demand[i] * 3);
            } else {
                op_discharge[month_nr] = am_flow[month_nr] - 
                        (am_flow[i] - am_demand[i] * 3) *
                        shortage_fraction;
            }
        }        
    }
    
    cur_volume_tmp = cur_volume;
    if(cur_volume_tmp > max_volume){
        cur_volume_tmp = max_volume;
    }
    
    for(i = 0; i < MONTHS_PER_YEAR; i++){        
        if(i == 0){
            op_volume[i] = pref_volume + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        } else if(i == MONTHS_PER_YEAR - 1) {
            op_volume[i] = pref_volume;
        } else {
            op_volume[i] = op_volume[i-1] + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        }
        
        if(op_volume[i] < 0){
            op_volume[i] = 0.0;
        }else if(op_volume[i] > max_volume){
            op_volume[i] = max_volume;
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
    double am_flow[MONTHS_PER_YEAR];
    double am_demand[MONTHS_PER_YEAR];
    double am_shortage[MONTHS_PER_YEAR];
    
    size_t i;
        
    years_running = (size_t)(dam_var[cur_cell][cur_dam].months_running / 
            MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }
    
    // Store monthly average
    dam_var[cur_cell][cur_dam].history_flow[0] =
            dam_var[cur_cell][cur_dam].total_flow / 
            dam_var[cur_cell][cur_dam].total_steps;
    dam_var[cur_cell][cur_dam].history_demand[0] =
            dam_var[cur_cell][cur_dam].total_demand / 
            dam_var[cur_cell][cur_dam].total_steps;
    dam_var[cur_cell][cur_dam].history_shortage[0] =
            dam_var[cur_cell][cur_dam].total_shortage / 
            dam_var[cur_cell][cur_dam].total_steps;
    dam_var[cur_cell][cur_dam].total_flow = 0.0;
    dam_var[cur_cell][cur_dam].total_demand = 0.0;
    dam_var[cur_cell][cur_dam].total_shortage = 0.0;
    dam_var[cur_cell][cur_dam].total_steps = 0;       

    // Calculate averages            
    if(dmy[current].month == dam_var[cur_cell][cur_dam].op_year){
        ay_flow = array_average(dam_var[cur_cell][cur_dam].history_flow,
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
        // Calculate operational year
        if(dam_con[cur_cell][cur_dam].function == DAM_FUN_IRR){
            dam_get_operation_irr(am_flow, 
                    am_demand, 
                    am_shortage,
                    dam_var[cur_cell][cur_dam].volume,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);

            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_irr(am_flow, 
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_year);
            
        }else if(dam_con[cur_cell][cur_dam].function == DAM_FUN_FLO){
            dam_get_operation_flo(ay_flow, am_flow, 
                    dam_var[cur_cell][cur_dam].volume,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MAX_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_MIN_VOL_FRAC,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);

            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_flo(ay_flow, am_flow,
                    dam_var[cur_cell][cur_dam].op_year);
            
        }else if(dam_con[cur_cell][cur_dam].function == DAM_FUN_HYD){ 
            dam_get_operation_hyd(am_flow, 
                    dam_var[cur_cell][cur_dam].volume,
                    dam_con[cur_cell][cur_dam].max_volume * DAM_PREF_VOL_FRAC,
                    dam_con[cur_cell][cur_dam].max_volume,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);  

            dam_var[cur_cell][cur_dam].op_year = 
                    dam_get_op_year_hyd(dam_var[cur_cell][cur_dam].op_year);
            
        }else{
            log_err("Unknown dam function");
        }
    }

    // Shift array
    cshift(dam_var[cur_cell][cur_dam].history_flow, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].history_demand, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].history_shortage, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var[cur_cell][cur_dam].op_discharge, 1, MONTHS_PER_YEAR, 1, 1);
    cshift(dam_var[cur_cell][cur_dam].op_volume, 1, MONTHS_PER_YEAR, 1, 1); 
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
                            irr_var[other_cell][j][k].shortage / MM_PER_M * 
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
            
            // Calculate discharge
            if(dam_con[cur_cell][i].function == DAM_FUN_HYD){
                dam_var[cur_cell][i].discharge = 
                    rout_var[cur_cell].discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],
                    dam_con[cur_cell][i].max_volume * 
                    DAM_DIS_MOD_FRAC_HYD,
                    dam_var[cur_cell][i].total_steps,
                    dam_var[cur_cell][i].volume);
            } else if(dam_con[cur_cell][i].function == DAM_FUN_IRR){
                dam_var[cur_cell][i].discharge = 
                    dam_var[cur_cell][i].op_discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],    
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC,                
                    dam_var[cur_cell][i].total_steps,
                    dam_var[cur_cell][i].volume);
            } else if (dam_con[cur_cell][i].function == DAM_FUN_FLO){ 
                dam_var[cur_cell][i].discharge = 
                    dam_var[cur_cell][i].op_discharge[0] +
                    dam_discharge_correction(
                    dam_var[cur_cell][i].op_volume[MONTHS_PER_YEAR - 1],                    
                    dam_var[cur_cell][i].op_volume[0],    
                    dam_var[cur_cell][i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC,                
                    dam_var[cur_cell][i].total_steps,
                    dam_var[cur_cell][i].volume);
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