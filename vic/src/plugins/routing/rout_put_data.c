#include <vic.h>

void
rout_put_data(size_t cur_cell)
{
    extern global_param_struct global_param;
    extern domain_struct local_domain;
    extern rout_var_struct *rout_var;
    extern double        ***out_data;
    extern node            *outvar_types;

    int                     OUT_DISCHARGE;
    int                     OUT_STREAM_MOIST;
    
    OUT_DISCHARGE = list_search_id(outvar_types, "OUT_DISCHARGE");
    OUT_STREAM_MOIST = list_search_id(outvar_types, "OUT_STREAM_MOIST");

    out_data[cur_cell][OUT_STREAM_MOIST][0] = rout_var[cur_cell].stream * 
            global_param.dt / local_domain.locations[cur_cell].area *
            MM_PER_M;
    out_data[cur_cell][OUT_DISCHARGE][0] = rout_var[cur_cell].discharge;
}
