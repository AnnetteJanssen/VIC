#include <vic.h>

#include "efr.h"

double
calc_efr_vfm(double ay_flow, double discharge)
{
    /* Variable Monthly Flow (VMF) method (Pastor et al., 2014) */
    if(ay_flow > 0){
        if(discharge < ay_flow * EFR_LOW_FLOW_FRAC){
            return  discharge * EFR_LOW_DEMAND_FRAC;
        }else if(discharge > ay_flow * EFR_HIGH_FLOW_FRAC){
            return discharge * EFR_HIGH_DEMAND_FRAC;
        }else{
            return discharge * linear_interp(discharge,
                    ay_flow * EFR_LOW_FLOW_FRAC,
                    ay_flow * EFR_HIGH_FLOW_FRAC,
                    EFR_LOW_DEMAND_FRAC, EFR_HIGH_DEMAND_FRAC);
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
    extern option_struct options;
    
    size_t i;
    size_t j;
    size_t l;
    
    double baseflow;
    
    efr_var[cur_cell].requirement_flow = 
            calc_efr_vfm(efr_hist[cur_cell].ay_discharge,
                         efr_hist[cur_cell].discharge);
    
    l = options.Nlayer - 1;
    for (i = 0; i < veg_con_map[cur_cell].nv_active; i++) {
        for (j = 0; j < elev_con_map[cur_cell].ne_active; j++) {
            baseflow = calc_efr_vfm(efr_hist[cur_cell].ay_baseflow,
                                    efr_hist[cur_cell].baseflow);
            
            if (options.GROUNDWATER) {
                // Based on groundwater baseflow formulation
                efr_var[cur_cell].requirement_moist[i][j] = 
                        log(baseflow / gw_con[cur_cell].Qb_max) /
                        gw_con[cur_cell].Qb_expt +
                        gw_con[cur_cell].Za_max;
            }
            else {
                // Based on VIC baseflow formulation
                
                // For now assume that baseflow increases linear to max
                // (instead of exponential as in non-linear baseflow)
                // TODO: reverse engineer baseflow function in VIC

                efr_var[cur_cell].requirement_moist[i][j] =
                        (baseflow / all_vars[cur_cell].cell[i][j].baseflow) *
                        all_vars[cur_cell].cell[i][j].layer[l].moist;
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