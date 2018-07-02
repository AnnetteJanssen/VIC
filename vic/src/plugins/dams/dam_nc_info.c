#include <vic.h>

bool
dam_set_nc_var_info(int                varid,
                    unsigned short int dtype,
                    nc_file_struct    *nc_file,
                    nc_var_struct     *nc_var)
{
    extern node *outvar_types;

    // set datatype
    nc_var->nc_type = get_nc_dtype(dtype);

    int OUT_DAM_VOLUME; 
    int OUT_DAM_DISCHARGE; 
    int OUT_DAM_INFLOW;
    int OUT_DAM_DEMAND;
    int OUT_DAM_OP_DISCHARGE; 
    int OUT_DAM_OP_DISCHARGE_IRR; 
    int OUT_DAM_OP_VOLUME; 
    int OUT_DAM_OP_MONTH; 
    int OUT_DAM_HIST_DEMAND; 
    int OUT_DAM_HIST_FLOW;
    
    OUT_DAM_VOLUME = list_search_id(outvar_types, "OUT_DAM_VOLUME");
    OUT_DAM_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_DISCHARGE");
    OUT_DAM_INFLOW = list_search_id(outvar_types, "OUT_DAM_INFLOW");
    OUT_DAM_DEMAND = list_search_id(outvar_types, "OUT_DAM_DEMAND");
    OUT_DAM_OP_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE");
    OUT_DAM_OP_DISCHARGE_IRR = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE_IRR");
    OUT_DAM_OP_VOLUME = list_search_id(outvar_types, "OUT_DAM_OP_VOLUME");
    OUT_DAM_OP_MONTH = list_search_id(outvar_types, "OUT_DAM_OP_MONTH");
    OUT_DAM_HIST_DEMAND = list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND");
    OUT_DAM_HIST_FLOW = list_search_id(outvar_types, "OUT_DAM_HIST_FLOW");

    if (varid == OUT_DAM_VOLUME ||
	varid == OUT_DAM_DISCHARGE ||
        varid == OUT_DAM_INFLOW ||
        varid == OUT_DAM_DEMAND ||
	varid == OUT_DAM_OP_DISCHARGE ||
	varid == OUT_DAM_OP_VOLUME ||
	varid == OUT_DAM_OP_MONTH ||
	varid == OUT_DAM_HIST_DEMAND ||
	varid == OUT_DAM_HIST_FLOW ||
	varid == OUT_DAM_OP_DISCHARGE_IRR) {
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_file->dam_size;
        nc_var->nc_counts[2] = nc_file->nj_size;
        nc_var->nc_counts[3] = nc_file->ni_size;
        return true;
    }
    else {
        return false;
    }
}

bool
dam_set_nc_var_dimids(int             varid,
                      nc_file_struct *nc_file,
                      nc_var_struct  *nc_var)
{
    extern node            *outvar_types;

    int OUT_DAM_VOLUME; 
    int OUT_DAM_DISCHARGE; 
    int OUT_DAM_INFLOW;
    int OUT_DAM_DEMAND;
    int OUT_DAM_OP_DISCHARGE; 
    int OUT_DAM_OP_DISCHARGE_IRR; 
    int OUT_DAM_OP_VOLUME; 
    int OUT_DAM_OP_MONTH; 
    int OUT_DAM_HIST_DEMAND; 
    int OUT_DAM_HIST_FLOW;
    
    OUT_DAM_VOLUME = list_search_id(outvar_types, "OUT_DAM_VOLUME");
    OUT_DAM_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_DISCHARGE");
    OUT_DAM_INFLOW = list_search_id(outvar_types, "OUT_DAM_INFLOW");
    OUT_DAM_DEMAND = list_search_id(outvar_types, "OUT_DAM_DEMAND");
    OUT_DAM_OP_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE");
    OUT_DAM_OP_DISCHARGE_IRR = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE_IRR");
    OUT_DAM_OP_VOLUME = list_search_id(outvar_types, "OUT_DAM_OP_VOLUME");
    OUT_DAM_OP_MONTH = list_search_id(outvar_types, "OUT_DAM_OP_MONTH");
    OUT_DAM_HIST_DEMAND = list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND");
    OUT_DAM_HIST_FLOW = list_search_id(outvar_types, "OUT_DAM_HIST_FLOW");

        if (varid == OUT_DAM_VOLUME ||
            varid == OUT_DAM_DISCHARGE ||
            varid == OUT_DAM_INFLOW ||
            varid == OUT_DAM_DEMAND ||
            varid == OUT_DAM_OP_DISCHARGE ||
            varid == OUT_DAM_OP_VOLUME ||
            varid == OUT_DAM_OP_MONTH ||
            varid == OUT_DAM_HIST_DEMAND ||
            varid == OUT_DAM_HIST_FLOW ||
            varid == OUT_DAM_OP_DISCHARGE_IRR) {
        nc_var->nc_dimids[0] = nc_file->time_dimid;
        nc_var->nc_dimids[1] = nc_file->dam_dimid;
        nc_var->nc_dimids[2] = nc_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_file->ni_dimid;
        return true;
    }
    else {
        return false;
    }
}
