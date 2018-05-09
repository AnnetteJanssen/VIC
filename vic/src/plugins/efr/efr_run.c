#include <vic.h>

#include "efr.h"

double
calc_efr_vfm(double ay_flow, double discharge)
{
    /* Variable Monthly Flow (VMF) method (Pastor et al., 2014) */
    if(ay_flow > 0){
        if(discharge < ay_flow * VFM_LOW_FLOW_FRAC){
            return  discharge * VFM_LOW_DEMAND_FRAC;
        }else if(discharge > ay_flow * VFM_HIGH_FLOW_FRAC){
            return discharge * VFM_HIGH_DEMAND_FRAC;
        }else{
            return discharge * linear_interp(discharge,
                    ay_flow * VFM_LOW_FLOW_FRAC,
                    ay_flow * VFM_HIGH_FLOW_FRAC,
                    VFM_LOW_DEMAND_FRAC, VFM_HIGH_DEMAND_FRAC);
        }    
    }else{
        return 0.0;
    }
}

void
calc_efrs_vfm(double ay_flow, double *discharges, size_t length, double *efrs)
{
    size_t i;
    
    for (i = 0; i < length; i++) {
        efrs[i] = calc_efr_vfm(ay_flow, discharges[i]);
    }
}

void 
efr_run_vfm(size_t cur_cell)
{
    extern efr_var_struct *efr_var;
    extern efr_hist_struct *efr_hist;
    extern gw_con_struct *gw_con;
    extern all_vars_struct *all_vars;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern global_param_struct global_param;
    extern option_struct options;
    
    double frac;
    size_t i;
    size_t j;
    size_t l;
    size_t k;
    
    double calculated_baseflow;
    double moist;
    double liq;
    double ice;
    double rel_liq;
    double res_moist;
    double max_moist;
    double dsmax;
    double bflow;
    
    efr_var[cur_cell].requirement_discharge = 
            calc_efr_vfm(efr_hist[cur_cell].ay_discharge,
                         efr_hist[cur_cell].discharge);
    
    efr_var[cur_cell].requirement_baseflow = 
            calc_efr_vfm(efr_hist[cur_cell].ay_baseflow,
                         efr_hist[cur_cell].baseflow);
                
    if (options.GROUNDWATER) {
        for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
            for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                // Based on groundwater baseflow formulation
                efr_var[cur_cell].requirement_moist[i][j] = 
                        log(efr_var[cur_cell].requirement_baseflow / 
                            gw_con[cur_cell].Qb_max) /
                        gw_con[cur_cell].Qb_expt +
                        gw_con[cur_cell].Za_max;
            }
        }
    }
    else {
        l = options.Nlayer - 1;
        res_moist = soil_con[cur_cell].resid_moist[l] * 
                    soil_con[cur_cell].depth[l] * 
                    MM_PER_M;
        max_moist = soil_con[cur_cell].max_moist[l];
        dsmax = soil_con[cur_cell].Dsmax / global_param.model_steps_per_day;

        for (frac = 1.0; frac >= 0.0; frac -= EFR_FRAC_STEP){
            calculated_baseflow = 0.0;

            for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
                for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                    // Based on VIC baseflow formulation
                    moist = all_vars[cur_cell].cell[i][j].layer[l].moist;

                    ice = 0.0;
                    for (k = 0; k < options.Nfrost; k++){
                        ice += all_vars[cur_cell].cell[i][j].layer[l].ice[k] *
                                soil_con[cur_cell].frost_fract[k];
                    }
                    liq = moist - ice;
                    liq *= frac;

                    rel_liq = (liq - res_moist) / (max_moist - res_moist);
                    bflow = rel_liq * dsmax * soil_con[cur_cell].Ds / 
                            soil_con[cur_cell].Ws;
                    
                    if (rel_liq > soil_con->Ws) {
                        bflow += dsmax * (1 - soil_con->Ds / soil_con->Ws) * 
                            pow((rel_liq - soil_con->Ws) / (1 - soil_con->Ws), 
                                    soil_con->c);
                    }
                    
                    if(bflow < 0){
                        bflow = 0.0;
                    }

                    calculated_baseflow += bflow * 
                                           veg_con[cur_cell][i].Cv * 
                                           soil_con[cur_cell].AreaFract[j];
                }
            }

            if(calculated_baseflow < efr_var[cur_cell].requirement_baseflow){
                frac += EFR_FRAC_STEP;
                if(frac > 1.0){
                    frac = 1.0;
                }

                break;
            }
        }
        
        if(frac < 0){
            frac = 0.0;
        }
        
        for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
            for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
                moist = all_vars[cur_cell].cell[i][j].layer[l].moist;

                ice = 0.0;
                for (k = 0; k < options.Nfrost; k++){
                    ice += all_vars[cur_cell].cell[i][j].layer[l].ice[k] *
                            soil_con[cur_cell].frost_fract[k];
                }
                liq = moist - ice;
                
                efr_var[cur_cell].requirement_moist[i][j] = liq * frac;
                        
            }
        }
    }
}

void
efr_run(size_t cur_cell)
{
    extern option_struct options;
    
    if (options.EFR_METHOD == EFR_METHOD_VFM) {
        efr_run_vfm(cur_cell);
    } 
    else if (options.EFR_METHOD == EFR_METHOD_7Q10) {
        log_err("EFR_METHOD 7Q10 not yet implemented...");
    } 
    else {
        log_err("Unknown EFR_METHOD");
    }
}