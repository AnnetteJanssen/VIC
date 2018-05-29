#include <vic.h>

void
rout_put_data(void)
{
    extern domain_struct    local_domain;
    extern rout_var_struct *rout_var;
    extern double        ***out_data;
    extern node            *outvar_types;

    size_t                  i;

    int                     OUT_DISCHARGE;
    int                     OUT_STREAM_MOIST;
    
    OUT_DISCHARGE = list_search_id(outvar_types, "OUT_DISCHARGE");
    OUT_STREAM_MOIST = list_search_id(outvar_types, "OUT_STREAM_MOIST");

    for (i = 0; i < local_domain.ncells_active; i++) {
        out_data[i][OUT_STREAM_MOIST][0] = rout_var[i].moist;
        out_data[i][OUT_DISCHARGE][0] = rout_var[i].discharge[0];
    }
}
