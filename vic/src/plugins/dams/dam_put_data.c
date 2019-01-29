#include <vic.h>

void
dam_put_data(size_t cur_cell)
{
    extern dam_var_struct     *local_dam_var;
    extern dam_var_struct     *global_dam_var;
    extern double           ***out_data;
    extern node               *outvar_types;

    size_t years_running;
 
    int OUT_LDAM_INFLOW;
    int OUT_LDAM_DEMAND;
    int OUT_LDAM_EFR;
    int OUT_LDAM_STORAGE;
    int OUD_LDAM_RELEASE;
    int OUT_LDAM_HIST_INFLOW;
    int OUT_LDAM_HIST_DEMAND;
    int OUT_LDAM_HIST_EFR;
    int OUT_LDAM_OP_RELEASE;
    int OUT_LDAM_OP_STORAGE;
    
    int OUT_GDAM_INFLOW;
    int OUT_GDAM_DEMAND;
    int OUT_GDAM_EFR;
    int OUT_GDAM_STORAGE;
    int OUD_GDAM_RELEASE;
    int OUT_GDAM_HIST_INFLOW;
    int OUT_GDAM_HIST_DEMAND;
    int OUT_GDAM_HIST_EFR;
    int OUT_GDAM_OP_RELEASE;
    int OUT_GDAM_OP_STORAGE;
    
    OUT_LDAM_INFLOW = list_search_id(outvar_types, "OUT_LDAM_INFLOW");
    OUT_LDAM_DEMAND = list_search_id(outvar_types, "OUT_LDAM_DEMAND");
    OUT_LDAM_EFR = list_search_id(outvar_types, "OUT_LDAM_EFR");
    OUT_LDAM_STORAGE = list_search_id(outvar_types, "OUT_LDAM_STORAGE");
    OUD_LDAM_RELEASE = list_search_id(outvar_types, "OUD_LDAM_RELEASE");
    OUT_LDAM_HIST_INFLOW = list_search_id(outvar_types, "OUT_LDAM_HIST_INFLOW");
    OUT_LDAM_HIST_DEMAND = list_search_id(outvar_types, "OUT_LDAM_HIST_DEMAND");
    OUT_LDAM_HIST_EFR = list_search_id(outvar_types, "OUT_LDAM_HIST_EFR");
    OUT_LDAM_OP_RELEASE = list_search_id(outvar_types, "OUT_LDAM_OP_RELEASE");
    OUT_LDAM_OP_STORAGE = list_search_id(outvar_types, "OUT_LDAM_OP_STORAGE");
    
    OUT_GDAM_INFLOW = list_search_id(outvar_types, "OUT_GDAM_INFLOW");
    OUT_GDAM_DEMAND = list_search_id(outvar_types, "OUT_GDAM_DEMAND");
    OUT_GDAM_EFR = list_search_id(outvar_types, "OUT_GDAM_EFR");
    OUT_GDAM_STORAGE = list_search_id(outvar_types, "OUT_GDAM_STORAGE");
    OUD_GDAM_RELEASE = list_search_id(outvar_types, "OUD_GDAM_RELEASE");
    OUT_GDAM_HIST_INFLOW = list_search_id(outvar_types, "OUT_GDAM_HIST_INFLOW");
    OUT_GDAM_HIST_DEMAND = list_search_id(outvar_types, "OUT_GDAM_HIST_DEMAND");
    OUT_GDAM_HIST_EFR = list_search_id(outvar_types, "OUT_GDAM_HIST_EFR");
    OUT_GDAM_OP_RELEASE = list_search_id(outvar_types, "OUT_GDAM_OP_RELEASE");
    OUT_GDAM_OP_STORAGE = list_search_id(outvar_types, "OUT_GDAM_OP_STORAGE");

    // Write to output struct
    years_running = (size_t)(local_dam_var[cur_cell].months_running / MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }
    
    out_data[cur_cell][OUT_LDAM_INFLOW][0] = local_dam_var[cur_cell].inflow;
    out_data[cur_cell][OUT_LDAM_DEMAND][0] = local_dam_var[cur_cell].demand;
    out_data[cur_cell][OUT_LDAM_EFR][0] = local_dam_var[cur_cell].efr;

    out_data[cur_cell][OUT_LDAM_STORAGE][0] = local_dam_var[cur_cell].storage;
    out_data[cur_cell][OUD_LDAM_RELEASE][0] = local_dam_var[cur_cell].release;

    out_data[cur_cell][OUT_LDAM_HIST_INFLOW][0] = local_dam_var[cur_cell].history_inflow[years_running * MONTHS_PER_YEAR - 1];
    out_data[cur_cell][OUT_LDAM_HIST_DEMAND][0] = local_dam_var[cur_cell].history_demand[years_running * MONTHS_PER_YEAR - 1];
    out_data[cur_cell][OUT_LDAM_HIST_EFR][0] = local_dam_var[cur_cell].history_efr[years_running * MONTHS_PER_YEAR - 1];

    out_data[cur_cell][OUT_LDAM_OP_RELEASE][0] = local_dam_var[cur_cell].op_release[0];
    out_data[cur_cell][OUT_LDAM_OP_STORAGE][0] = local_dam_var[cur_cell].op_storage[0] / pow(M_PER_KM, 2);
    
    years_running = (size_t)(global_dam_var[cur_cell].months_running / MONTHS_PER_YEAR);
    if(years_running > DAM_HIST_YEARS){
        years_running = DAM_HIST_YEARS;
    }
    
    out_data[cur_cell][OUT_GDAM_INFLOW][0] = global_dam_var[cur_cell].inflow;
    out_data[cur_cell][OUT_GDAM_DEMAND][0] = global_dam_var[cur_cell].demand;
    out_data[cur_cell][OUT_GDAM_EFR][0] = global_dam_var[cur_cell].efr;

    out_data[cur_cell][OUT_GDAM_STORAGE][0] = global_dam_var[cur_cell].storage;
    out_data[cur_cell][OUD_GDAM_RELEASE][0] = global_dam_var[cur_cell].release;

    out_data[cur_cell][OUT_GDAM_HIST_INFLOW][0] = global_dam_var[cur_cell].history_inflow[years_running * MONTHS_PER_YEAR - 1];
    out_data[cur_cell][OUT_GDAM_HIST_DEMAND][0] = global_dam_var[cur_cell].history_demand[years_running * MONTHS_PER_YEAR - 1];
    out_data[cur_cell][OUT_GDAM_HIST_EFR][0] = global_dam_var[cur_cell].history_efr[years_running * MONTHS_PER_YEAR - 1];

    out_data[cur_cell][OUT_GDAM_OP_RELEASE][0] = global_dam_var[cur_cell].op_release[0];
    out_data[cur_cell][OUT_GDAM_OP_STORAGE][0] = global_dam_var[cur_cell].op_storage[0] / pow(M_PER_KM, 2);
}    
