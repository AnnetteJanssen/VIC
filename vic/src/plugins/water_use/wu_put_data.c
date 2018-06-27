#include <vic.h>

void
wu_put_data(size_t cur_cell)
{
    extern wu_var_struct **wu_var;
    extern double       ***out_data;
    extern node           *outvar_types;

    size_t                 j;
    
    int OUT_WU_DEMAND = list_search_id(outvar_types, "OUT_WU_DEMAND");
    int OUT_WU_WITHDRAWN = list_search_id(outvar_types, "OUT_WU_WITHDRAWN");
    int OUT_WU_CONSUMED = list_search_id(outvar_types, "OUT_WU_CONSUMED");
    int OUT_WU_RETURNED = list_search_id(outvar_types, "OUT_WU_RETURNED");
    
    for (j = 0; j < WU_NSECTORS; j++) {
        out_data[cur_cell][OUT_WU_DEMAND][j] = wu_hist[cur_cell][j].demand;
        out_data[cur_cell][OUT_WU_WITHDRAWN][j] = wu_var[cur_cell][j].withdrawn;
        out_data[cur_cell][OUT_WU_CONSUMED][j] = wu_var[cur_cell][j].consumed;
        out_data[cur_cell][OUT_WU_RETURNED][j] = wu_var[cur_cell][j].returned;
    }
}

