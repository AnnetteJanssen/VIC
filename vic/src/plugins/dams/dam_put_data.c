#include <vic.h>

void
dam_put_data(void)
{
    extern domain_struct       local_domain;
    extern dam_var_struct    **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern double           ***out_data;
    extern node               *outvar_types;

    size_t years_running;
    
    size_t                     i;
    size_t                     j;

    // Write to output struct
    int OUT_DAM_VOLUME = list_search_id(outvar_types, "OUT_DAM_VOLUME");
    int OUT_DAM_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_DISCHARGE");
    int OUT_DAM_OP_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE");
    int OUT_DAM_OP_DISCHARGE_IRR = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE_IRR");
    int OUT_DAM_OP_VOLUME = list_search_id(outvar_types, "OUT_DAM_OP_VOLUME");
    int OUT_DAM_OP_MONTH = list_search_id(outvar_types, "OUT_DAM_OP_MONTH");
    int OUT_DAM_HIST_DEMAND = list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND");
    int OUT_DAM_HIST_FLOW = list_search_id(outvar_types, "OUT_DAM_HIST_FLOW");

    for (i = 0; i < local_domain.ncells_active; i++) { 
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
        
            years_running = (size_t)(dam_var[i][j].months_running / 
                    MONTHS_PER_YEAR);
            if(years_running > DAM_HIST_YEARS){
                years_running = DAM_HIST_YEARS;
            }
    
            out_data[i][OUT_DAM_VOLUME][j] = dam_var[i][j].volume / pow(M_PER_KM, 2);
            out_data[i][OUT_DAM_DISCHARGE][j] = dam_var[i][j].discharge;
            out_data[i][OUT_DAM_OP_DISCHARGE][j] = dam_var[i][j].op_discharge[0];
            out_data[i][OUT_DAM_OP_VOLUME][j] = dam_var[i][j].op_volume[0] / pow(M_PER_KM, 3);
            out_data[i][OUT_DAM_OP_MONTH][j] = dam_var[i][j].op_year;
            out_data[i][OUT_DAM_HIST_DEMAND][j] = dam_var[i][j].history_demand[years_running * MONTHS_PER_YEAR - 1];
            out_data[i][OUT_DAM_HIST_FLOW][j] = dam_var[i][j].history_flow[years_running * MONTHS_PER_YEAR - 1];
            out_data[i][OUT_DAM_OP_DISCHARGE_IRR][j] = dam_var[i][j].op_discharge_irr[0];
        }
    }    
}
