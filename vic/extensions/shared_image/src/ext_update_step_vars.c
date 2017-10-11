#include <ext_driver_shared_image.h>

void
routing_update_step_vars(rout_var_struct *rout_var){
    
    extern ext_option_struct ext_options;  
    
    rout_var->discharge[0] = 0.0;
    rout_var->nat_discharge[0] = 0.0;
    cshift(rout_var->discharge, ext_options.uh_steps, 1, 0, 1);
    cshift(rout_var->nat_discharge, ext_options.uh_steps, 1, 0, 1);
}

void
dams_update_step_vars(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    extern dmy_struct *dmy;
    extern size_t current;
    
    double my_nat_inflow;
    double my_inflow;
    double ms_nat_inflow[ext_options.history_steps_per_history_year];
    double ms_inflow[ext_options.history_steps_per_history_year];
    
    double discharge[ext_options.history_steps_per_history_year];
    double efr[ext_options.history_steps_per_history_year];
    
    size_t years;
    
    size_t i;
    
    if(dam_var->inflow_offset >= ext_options.model_steps_per_history_step){
        // Model steps per history step has passed
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->calc_discharge, ext_options.history_steps, 1, 0, -1);
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->inflow_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->inflow_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->inflow_offset = 0;
    }
    
    if(current > 0 &&
            dmy[current].month == dam_var->op_year.month && 
            dmy[current].day == dam_var->op_year.day &&
            dmy[current].dayseconds == dam_var->op_year.dayseconds){
        // Operational year has passed
        
        dam_var->years_running ++;  
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->calc_discharge, ext_options.history_steps, 1, 0, -1);
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->inflow_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->inflow_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->inflow_offset = 0;
        
        if(dam_var->years_running < (size_t) ext_param.DAM_HISTORY){
            years = dam_var->years_running;
        }else{
            years = ext_param.DAM_HISTORY;
        }
        
        // Calculate multi-year averages
        calculate_multi_year_average(dam_var->inflow_history, 
                years, 
                years * ext_options.history_steps_per_history_year,
                0,
                0,
                &my_inflow);
        calculate_multi_year_average(dam_var->nat_inflow_history, 
                years, 
                years * ext_options.history_steps_per_history_year,
                0,
                0,
                &my_nat_inflow);
        for(i=0;i<ext_options.history_steps_per_history_year;i++){
            calculate_multi_year_average(dam_var->inflow_history,
                    years,
                    1,
                    i,
                    ext_options.history_steps_per_history_year - i - 1,
                    &ms_inflow[i]);
            calculate_multi_year_average(dam_var->nat_inflow_history,years,
                    1,
                    i,
                    ext_options.history_steps_per_history_year - i - 1,
                    &ms_nat_inflow[i]);
        }
        
        // Calculate optimal discharge
        calculate_optimal_discharge(dam_con, *dam_var, my_inflow, ms_inflow, discharge);
        
        // Calculate efr        
        for(i=0;i<ext_options.history_steps_per_history_year;i++){
            efr[i] = calculate_efr(ms_nat_inflow[i],my_nat_inflow);
        }
        
        // Ensure efr is being met (when possible)
        calculate_corrected_discharge(discharge,efr,my_inflow,dam_var->calc_discharge);
    }
    
    dam_var->inflow_offset++;
}

void 
calculate_multi_year_average(double *history, size_t repetitions, size_t length, size_t offset, size_t skip, double *average){
    
    size_t i;
    size_t j;
    
    (*average) = 0.0;
    for(i=0;i<repetitions;i++){
        for(j=0;j<length;j++){
            (*average) += history[(i * (offset+length+skip)) + offset + j] / (repetitions * length);
        }
    }    
}

double
calculate_efr(double flow, double annual_flow){
    return calculate_efr_fraction(flow,annual_flow) * flow;
}

double
calculate_efr_fraction(double flow, double annual_flow){
    if(flow < DAM_EFR_MINF * annual_flow){
        return DAM_EFR_MINR;
    }else if(flow > DAM_EFR_MAXF * annual_flow){        
        return DAM_EFR_MAXR;
    }else{
        return linear_interp((flow / annual_flow),DAM_EFR_MINF,DAM_EFR_MAXF,DAM_EFR_MINR,DAM_EFR_MAXR);
    }
}

void
calculate_optimal_discharge(dam_con_struct dam_con, dam_var_struct dam_var, 
        double my_inflow, double *ms_inflow, double *discharge){
    ext_option_struct ext_options;
    
    double volume_needed;
    double amplitude;
    double offset;
    
    size_t i;
    
    // Calculate amplitude
    for(amplitude = 0; amplitude <= 1; amplitude += DAM_ASTEP){
        volume_needed = calculate_volume_needed(amplitude, 0, my_inflow, ms_inflow,
                ext_options.history_steps_per_history_year, ext_options.model_steps_per_history_step);
        if(dam_con.max_volume * DAM_PVOLUME >= volume_needed){
            break;
        }
    }
    
    debug("AMPLITUDE %.2f",amplitude);
    
    // Calculate offset
    offset = ((dam_con.max_volume * DAM_PVOLUME) - dam_var.volume) /
            (ext_options.history_steps_per_history_year * ext_options.model_steps_per_history_step);
    
    // Calculate discharge;
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        discharge[i] = calculate_volume_needed(amplitude, offset, my_inflow, ms_inflow,
                ext_options.history_steps_per_history_year, ext_options.model_steps_per_history_step);
    }
}

double
calculate_volume_needed(double amplitude, double offset, double my_inflow, double *ms_inflow, 
        size_t history_length, size_t model_steps_per_history){
    
    double discharge;
    double capacity_needed;
    
    size_t i;
    
    capacity_needed = 0.0;
    if(my_inflow > 0){
        for(i = 0; i < history_length; i++){
            discharge = get_amplitude_discharge(my_inflow, ms_inflow[i], amplitude, offset);
            
            if(discharge < ms_inflow[i]){
                capacity_needed += (ms_inflow[i] - discharge) * model_steps_per_history;
            }
        }
    }
    
    return capacity_needed;
}

double 
get_amplitude_discharge(double my_inflow, double ms_inflow, double amplitude, double offset){
    return my_inflow * (1-amplitude) +
            my_inflow * amplitude * (ms_inflow / my_inflow) + 
            offset;
}

void 
calculate_corrected_discharge(double *ucor_discharge, double *efr, double my_inflow, double *cor_discharge){
    extern ext_option_struct ext_options;
    
    double cor_needed;
    double cor_available;
    
    size_t i;
    
    // See how much discharge we need to correct for and how much we have
    // to correct the discharge
    cor_needed = 0.0;
    cor_available = 0.0;
    for(i = 0; i < ext_options.history_steps_per_history_year; i++){
        if(ucor_discharge[i] < efr[i]){
            // Dam needs to discharge more
            cor_needed += ucor_discharge[i] - efr[i];
        }else if(ucor_discharge[i] > my_inflow){
            // Dam is storing water
        }else{
            // Can reduce discharge this month
            cor_available += ucor_discharge[i] - efr[i];
        }
    }
    
    // Correct the discharge to account for efr
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        cor_discharge[i] = ucor_discharge[i];
    }
    
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        if(ucor_discharge[i] < efr[i]){
            // Dam needs to discharge more
            cor_discharge[i] = efr[i];
        }else if(ucor_discharge[i] > my_inflow){
            // Dam is storing water
            cor_discharge[i] = ucor_discharge[i];
        }else{
            // Can reduce discharge this month
            if(cor_needed<cor_available){
                // Cannot correct for all efr, do as much as possible
                cor_discharge[i] = efr[i];
            }else{
                // Reduce discharge in months where correction is possible
                cor_discharge[i] = ucor_discharge[i] - 
                        ((ucor_discharge[i] - efr[i]) *
                        (cor_needed / cor_available));
            }
        }        
    }
}