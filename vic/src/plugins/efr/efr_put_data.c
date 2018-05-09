#include <vic.h>

void
efr_put_data(void)
{
    extern domain_struct   local_domain;
    extern efr_var_struct *efr_var;
    extern double       ***out_data;
    extern node           *outvar_types;

    size_t                 i;

    int                    OUT_EFR_DIS_REQ;
    int                    OUT_EFR_BASE_REQ;
    
    OUT_EFR_DIS_REQ = list_search_id(outvar_types, "OUT_EFR_DIS_REQ");
    OUT_EFR_BASE_REQ = list_search_id(outvar_types, "OUT_EFR_BASE_REQ");

    for (i = 0; i < local_domain.ncells_active; i++) {
        out_data[i][OUT_EFR_DIS_REQ][0] = efr_var[i].requirement_discharge;
        out_data[i][OUT_EFR_BASE_REQ][0] = efr_var[i].requirement_baseflow;
    }
}
