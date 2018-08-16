#include <vic.h>

void
dam_calc_op_discharge(double ay_flow, 
                      double *am_flow, 
                      double amplitude,
                      double *op_dicharge)
{
    size_t i;
    if(ay_flow == 0){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_dicharge[i] = 0.0;
        }
    } else {
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_dicharge[i] = ay_flow + ((am_flow[i] - ay_flow) * amplitude);
            if(op_dicharge[i] < 0){
                op_dicharge[i] = 0.0;
            }
        }
    }
}

void
dam_calc_op_volume(double start_volume, 
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
                op_volume[month_nr] = start_volume;
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
dam_calc_vol_needed(double *op_discharge,
                    double *am_flow,
                    double *volume_needed)
{
    extern global_param_struct global_param;
    
    size_t i;
    
    *volume_needed = 0.0;
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        if(op_discharge[i] < am_flow[i]){
            *volume_needed += (am_flow[i] - op_discharge[i]) * 
                               global_param.dt * 
                               global_param.model_steps_per_day * 
                               DAYS_PER_MONTH_AVG;
        }
    }
}

void
dam_calc_sup_needed(double *op_discharge,
                    double *am_demand,
                    double *sup_needed)
{    
    size_t i;
    
    *sup_needed = 0.0;
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        if(op_discharge[i] < am_demand[i]){
            *sup_needed += am_demand[i] - op_discharge[i];
        }
    }
}

void
dam_get_operation_def(double ay_flow, 
                      double *am_flow, 
                      double avail_volume,
                      double start_volume,
                      int op_year,
                      double amp_start,
                      double amp_end,
                      double amp_step,
                      double *op_discharge, 
                      double *op_volume)
{    
    double amplitude;
    double volume_needed;
    bool discharge_calculated;
    
    size_t i;    
    
    // Set operational discharge
    discharge_calculated = false;
    for(amplitude = amp_start; amplitude < amp_end; amplitude += amp_step){
        dam_calc_op_discharge(ay_flow, am_flow, amplitude, op_discharge);
        
        dam_calc_vol_needed(op_discharge, am_flow, &volume_needed);         
        if(volume_needed < avail_volume){
            discharge_calculated = true;
            break;
        }
    }    
    
    if(!discharge_calculated){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i];
        }
    }
    
    dam_calc_op_volume(start_volume, am_flow, op_discharge, op_year, op_volume);
}

void
dam_get_operation_sup(double ay_flow, 
                      double *am_flow,
                      double *am_demand,
                      double avail_volume,
                      double pref_dem_sup,
                      double start_volume,
                      int op_year,
                      double amp_start,
                      double amp_end,
                      double amp_step,
                      double *op_discharge, 
                      double *op_volume)
{    
    double amplitude;
    double volume_needed;
    double supply_needed;
    
    bool discharge_calculated;
    
    size_t i;
    
    // Set operational discharge
    discharge_calculated = false;
    for(amplitude = amp_start; amplitude < amp_end; amplitude -= amp_step){
        dam_calc_op_discharge(ay_flow, am_flow, amplitude, op_discharge);
        
        dam_calc_sup_needed(op_discharge, am_demand, &supply_needed);
        dam_calc_vol_needed(op_discharge, am_flow, &volume_needed);
        
        if(volume_needed <= avail_volume && supply_needed <= pref_dem_sup){
            discharge_calculated = true;
            break;
        }
    }    
    
    if(!discharge_calculated){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i];
        }
    }
    
    dam_calc_op_volume(start_volume, am_flow, op_discharge, op_year, op_volume);
}

int
dam_get_op_year_start(double ay_flow, double *am_flow)
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
dam_history_month(size_t cur_cell, size_t cur_dam)
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
    cshift(dam_var[cur_cell][cur_dam].history_demand, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var[cur_cell][cur_dam].history_flow, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var[cur_cell][cur_dam].op_discharge, MONTHS_PER_YEAR, 1, 0, 1);
    cshift(dam_var[cur_cell][cur_dam].op_discharge_irr, MONTHS_PER_YEAR, 1, 0, 1);
    cshift(dam_var[cur_cell][cur_dam].op_volume, MONTHS_PER_YEAR, 1, 0, 1);
    
    // Store monthly average
    dam_var[cur_cell][cur_dam].history_flow[0] =
            dam_var[cur_cell][cur_dam].total_flow / 
            dam_var[cur_cell][cur_dam].total_steps;
    dam_var[cur_cell][cur_dam].history_demand[0] =
            dam_var[cur_cell][cur_dam].total_demand / 
            dam_var[cur_cell][cur_dam].total_steps;
    
    dam_var[cur_cell][cur_dam].total_flow = 0.0;
    dam_var[cur_cell][cur_dam].total_demand = 0.0;
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
        }

        double_flip(am_flow,MONTHS_PER_YEAR);
        double_flip(am_demand,MONTHS_PER_YEAR);
        double_flip(am_shortage,MONTHS_PER_YEAR);
        
        dam_var[cur_cell][cur_dam].op_year = 
                dam_get_op_year_start(ay_flow, am_flow);
            
        // Calculate operational year, discharge and volume
        if(dam_con[cur_cell][cur_dam].function == DAM_FUN_IRR || dam_con[cur_cell][cur_dam].function == DAM_FUN_WAS){
            dam_get_operation_sup(ay_flow, am_flow, am_demand,
                    dam_con[cur_cell][cur_dam].max_volume * 
                    (DAM_PREF_VOL_SUP - DAM_MIN_VOL_SUP),
                    ay_demand * DAM_PREF_DEM_SUP,
                    dam_con[cur_cell][cur_dam].max_volume * 
                    DAM_PREF_VOL_SUP,
                    dam_var[cur_cell][cur_dam].op_year,
                    DAM_SAMP_SUP, DAM_EAMP_SUP, DAM_AMP_STEP_NEG,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);
        } else if (dam_con[cur_cell][cur_dam].function == DAM_FUN_HYD) {
            dam_get_operation_def(ay_flow, am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * 
                    (DAM_PREF_VOL_HYD - DAM_MIN_VOL_HYD),
                    dam_con[cur_cell][cur_dam].max_volume * 
                    DAM_PREF_VOL_HYD,
                    dam_var[cur_cell][cur_dam].op_year,
                    DAM_SAMP_HYD, DAM_EAMP_HYD, DAM_AMP_STEP_POS,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);
        } else if (dam_con[cur_cell][cur_dam].function == DAM_FUN_FLO) {
            dam_get_operation_def(ay_flow, am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * 
                    (DAM_PREF_VOL_FLO - DAM_MIN_VOL_FLO),
                    dam_con[cur_cell][cur_dam].max_volume * 
                    DAM_PREF_VOL_FLO,
                    dam_var[cur_cell][cur_dam].op_year,
                    DAM_SAMP_FLO, DAM_EAMP_FLO, DAM_AMP_STEP_POS,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);
        } else {
            dam_get_operation_def(ay_flow, am_flow, 
                    dam_con[cur_cell][cur_dam].max_volume * 
                    (DAM_PREF_VOL_HYD - DAM_MIN_VOL_HYD),
                    dam_con[cur_cell][cur_dam].max_volume * 
                    DAM_PREF_VOL_HYD,
                    dam_var[cur_cell][cur_dam].op_year,
                    DAM_SAMP_FLO, DAM_EAMP_FLO, DAM_AMP_STEP_POS,
                    dam_var[cur_cell][cur_dam].op_discharge,
                    dam_var[cur_cell][cur_dam].op_volume);
        }
    } 
}

void
dam_history_step(size_t cur_cell, size_t cur_dam)
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
    double serve_factor;
    
    size_t i;
    size_t j;
    size_t k;
    
    dam_var[cur_cell][cur_dam].inflow = rout_var[cur_cell].discharge[0];
    dam_var[cur_cell][cur_dam].total_flow += 
            dam_var[cur_cell][cur_dam].inflow;
    
    if(options.IRRIGATION){
        dam_var[cur_cell][cur_dam].demand = 0.0;
        
        for(i = 0; i < dam_con[cur_cell][cur_dam].nservice; i++){
            other_cell = dam_con[cur_cell][cur_dam].service[i];
            serve_factor = dam_con[cur_cell][cur_dam].serve_factor[i];
                    
            for(j = 0; j < irr_con_map[other_cell].ni_active; j++){
                cur_veg = irr_con[other_cell][j].veg_index;
            
                for(k = 0; k < elev_con_map[other_cell].ne_active; k++){
                    dam_var[cur_cell][cur_dam].demand +=
                            irr_var[other_cell][j][k].need / MM_PER_M * 
                            (local_domain.locations[other_cell].area *
                            soil_con[other_cell].AreaFract[k] * 
                            veg_con[other_cell][cur_veg].Cv) / 
                            global_param.dt * 
                            serve_factor;
                }
            }
        }
    }
    dam_var[cur_cell][cur_dam].total_demand +=
            dam_var[cur_cell][cur_dam].demand;
                    
    dam_var[cur_cell][cur_dam].total_steps++;
}

void
dam_history(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern dam_con_map_struct *dam_con_map;
    
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
            dam_history_month(cur_cell, i);
        }        
        dam_history_step(cur_cell,i);
    }
}