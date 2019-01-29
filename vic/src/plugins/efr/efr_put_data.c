#include <vic.h>

void
efr_put_data(size_t cur_cell)
{
    extern efr_force_struct *efr_force;
    extern double       ***out_data;
    extern node           *outvar_types;


    int                    OUT_EFR_DIS_REQ;
    int                    OUT_EFR_BASE_REQ;
    
    OUT_EFR_DIS_REQ = list_search_id(outvar_types, "OUT_EFR_DIS_REQ");
    OUT_EFR_BASE_REQ = list_search_id(outvar_types, "OUT_EFR_BASE_REQ");

    out_data[cur_cell][OUT_EFR_DIS_REQ][0] = efr_force[cur_cell].req_discharge[NR];
    out_data[cur_cell][OUT_EFR_BASE_REQ][0] = efr_force[cur_cell].requirement_baseflow[NR];
}
