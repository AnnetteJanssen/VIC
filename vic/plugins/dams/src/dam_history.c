#include <vic_driver_image.h>
#include <plugin.h>

void
dam_history_year(dam_con_struct *dam_con, dam_var_struct *dam_var)
{
    double inflow[MONTHS_PER_YEAR];
    double demand[MONTHS_PER_YEAR];
    double efr[MONTHS_PER_YEAR];
    double release[MONTHS_PER_YEAR];
    double storage[MONTHS_PER_YEAR];    
    double k;
    double c;
    
    size_t i;
    size_t years_running;
    
    years_running = (size_t)(dam_var->months_running / MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }
    
    // Setup arrays
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        inflow[i] = array_average(dam_var->history_inflow,
        years_running, 1, i, MONTHS_PER_YEAR - i - 1);
        demand[i] = array_average(dam_var->history_demand,
        years_running, 1, i, MONTHS_PER_YEAR - i - 1);
        efr[i] = array_average(dam_var->history_efr,
        years_running, 1, i, MONTHS_PER_YEAR - i - 1);
    }

    double_flip(inflow, MONTHS_PER_YEAR);
    double_flip(demand, MONTHS_PER_YEAR);
    double_flip(efr, MONTHS_PER_YEAR);

    // Calculate dam release
    dam_calc_opt_release(inflow, demand, efr, release, MONTHS_PER_YEAR);
    k = dam_calc_k_factor(dam_con->capacity, dam_var->storage);
    c = dam_calc_c_factor(inflow, dam_con->capacity, MONTHS_PER_YEAR);
    dam_corr_opt_release(inflow, release, MONTHS_PER_YEAR, k, c);
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        dam_var->op_release[i] = release[i];
    }

    // Calculate dam volume
    dam_calc_opt_storage(inflow, release, storage, MONTHS_PER_YEAR, dam_var->storage);
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        dam_var->op_storage[i] = storage[i];
    }
}

void
dam_history_month(dam_var_struct *dam_var)
{
    dam_var->months_running++;

    // Shift array
    cshift(dam_var->history_demand, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var->history_inflow, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var->history_efr, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, 0, -1);
    cshift(dam_var->op_release, MONTHS_PER_YEAR, 1, 0, 1);
    cshift(dam_var->op_storage, MONTHS_PER_YEAR, 1, 0, 1);
    
    // Store monthly average
    dam_var->history_inflow[0] = dam_var->total_inflow;
    dam_var->history_demand[0] = dam_var->total_demand;
    dam_var->history_efr[0] = dam_var->total_efr;
    
    dam_var->total_inflow = 0.0;
    dam_var->total_demand = 0.0;
    dam_var->total_efr = 0.0;
}

//void
//dam_register_demand(dam_con_struct *dam_con, dam_var_struct *dam_var)
//{
//    extern domain_struct local_domain;
//    extern soil_con_struct *soil_con;
//    extern veg_con_struct **veg_con;
//    extern irr_con_map_struct *irr_con_map;
//    extern elev_con_map_struct *elev_con_map;
//    extern irr_var_struct ***irr_var;
//        
//    size_t other_cell;
//    size_t cur_veg;
//    double serve_factor;
//    
//    size_t i;
//    size_t j;
//    size_t k;
//    
//    irr_con_struct *cirr_con;
//    irr_var_struct *cirr_var;
//    soil_con_struct *csoil_con;
//    veg_con_struct *cveg_con;
//    
//    dam_var->demand = 0.0;
//    
//    for(i = 0; i < dam_con->nservice; i++){
//        other_cell = dam_con->service[i];
//        serve_factor = dam_con->service_frac[i];
//
//        for(j = 0; j < irr_con_map[other_cell].ni_active; j++){
//            cirr_con = &irr_con[other_cell][j];
//            cur_veg = cirr_con->veg_index;
//
//            csoil_con = &soil_con[other_cell];
//            cveg_con = &veg_con[other_cell][cur_veg];
//
//            for(k = 0; k < elev_con_map[other_cell].ne_active; k++){
//                cirr_var = &irr_var[other_cell][i][k];
//
//                dam_var->demand +=
//                        cirr_var->need * 
//                        local_domain.locations[other_cell].area *
//                        csoil_con->AreaFract[k] * cveg_con->Cv *
//                        serve_factor / MM_PER_M / M3_PER_HM3;
//            }
//        }
//    }
//}
//
//void
//dam_register_efr(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
//{
//    extern global_param_struct global_param;
//    extern efr_force_struct *efr_force;
//    extern size_t NR;
//    
//    dam_var->efr = efr_force[cur_cell].req_discharge[NR] * 
//            global_param.dt * 
//            dam_con->inflow_frac / M3_PER_HM3;
//}

void
global_dam_register_inflow(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern global_param_struct global_param;
    extern rout_var_struct *rout_var;
    
    dam_var->inflow = rout_var[cur_cell].discharge * global_param.dt *
            dam_con->inflow_frac / M3_PER_HM3;
}

void
local_dam_register_inflow(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern domain_struct local_domain;
    extern double           ***out_data;
    
    dam_var->inflow = out_data[cur_cell][OUT_RUNOFF][0] * 
            local_domain.locations[cur_cell].area *
            dam_con->inflow_frac / M3_PER_HM3 / MM_PER_M;
}

void
dam_activate(dam_con_struct *dam_con, dam_var_struct *dam_var)
{
    extern dmy_struct *dmy;
    extern size_t current;
    
    if(dmy[current].year >= dam_con->year){
        dam_var->active = true;
    } else {
        dam_var->active = false;
    }
}

void
local_dam_history(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern option_struct options;
    
    if(current > 0){
        if(dmy[current].month != dmy[current - 1].month){
            dam_history_month(dam_var);
        }
        if(dmy[current].year != dmy[current - 1].year){
            dam_history_year(dam_con, dam_var);
        }
    }
    
    local_dam_register_inflow(dam_con, dam_var, cur_cell);
    
//    if(options.IRRIGATION){
//        dam_register_demand(dam_con, dam_var);
//    }
//    if(options.EFR){
//        dam_register_efr(dam_con, dam_var, cur_cell);
//    }
    
    dam_var->total_inflow += dam_var->inflow;
    dam_var->total_demand += dam_var->demand;
    dam_var->total_efr += dam_var->efr;
    
    dam_activate(dam_con, dam_var);
}

void
global_dam_history(dam_con_struct *dam_con, dam_var_struct *dam_var, size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern option_struct options;
    
    if(current > 0){
        if(dmy[current].month != dmy[current - 1].month){
            dam_history_month(dam_var);
        }
        if(dmy[current].year != dmy[current - 1].year){
            dam_history_year(dam_con, dam_var);
        }
    }
    
    global_dam_register_inflow(dam_con, dam_var, cur_cell);
    
//    if(options.IRRIGATION){
//        dam_register_demand(dam_con, dam_var);
//    }
//    if(options.EFR){
//        dam_register_efr(dam_con, dam_var, cur_cell);
//    }
    
    dam_var->total_inflow += dam_var->inflow;
    dam_var->total_demand += dam_var->demand;
    dam_var->total_efr += dam_var->efr;
    
    dam_activate(dam_con, dam_var);
}
