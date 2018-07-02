#include <vic.h>

void
dam_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;
    extern option_struct    options;

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
    
    strcpy(out_metadata[OUT_DAM_VOLUME].varname, "OUT_DAM_VOLUME");
    strcpy(out_metadata[OUT_DAM_VOLUME].long_name, "dam_water_volume");
    strcpy(out_metadata[OUT_DAM_VOLUME].standard_name,
           "dam_water_volume");
    strcpy(out_metadata[OUT_DAM_VOLUME].units, "mcm");
    strcpy(out_metadata[OUT_DAM_VOLUME].description,
           "dam volume");

    strcpy(out_metadata[OUT_DAM_DISCHARGE].varname, "OUT_DAM_DISCHARGE");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].long_name, "dam_discharge");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].standard_name,
           "dam_discharge");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].description,
           "flow out dam reservoir");

    strcpy(out_metadata[OUT_DAM_INFLOW].varname, "OUT_DAM_INFLOW");
    strcpy(out_metadata[OUT_DAM_INFLOW].long_name, "dam_inflow");
    strcpy(out_metadata[OUT_DAM_INFLOW].standard_name,
           "dam_inflow");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_DISCHARGE].description,
           "flow in dam reservoir");

    strcpy(out_metadata[OUT_DAM_DEMAND].varname, "OUT_DAM_DEMAND");
    strcpy(out_metadata[OUT_DAM_DEMAND].long_name, "dam_demand");
    strcpy(out_metadata[OUT_DAM_DEMAND].standard_name,
           "dam_demand");
    strcpy(out_metadata[OUT_DAM_DEMAND].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_DEMAND].description,
           "demand of downstream irrigation");

    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE].varname, "OUT_DAM_OP_DISCHARGE");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE].long_name, "dam_operational_discharge");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE].standard_name,
           "dam_operational_discharge");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE].description,
           "dam_operational_discharge");

    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE_IRR].varname, "OUT_DAM_OP_DISCHARGE_IRR");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE_IRR].long_name, "dam_operational_discharge");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE_IRR].standard_name,
           "dam_operational_discharge");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE_IRR].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_OP_DISCHARGE_IRR].description,
           "dam_operational_discharge");

    strcpy(out_metadata[OUT_DAM_OP_VOLUME].varname, "OUT_DAM_OP_VOLUME");
    strcpy(out_metadata[OUT_DAM_OP_VOLUME].long_name, "dam_operational_volume");
    strcpy(out_metadata[OUT_DAM_OP_VOLUME].standard_name,
           "dam_operational_volume");
    strcpy(out_metadata[OUT_DAM_OP_VOLUME].units, "km3");
    strcpy(out_metadata[OUT_DAM_OP_VOLUME].description,
           "dam_operational_volume");

    strcpy(out_metadata[OUT_DAM_OP_MONTH].varname, "OUT_DAM_OP_MONTH");
    strcpy(out_metadata[OUT_DAM_OP_MONTH].long_name, "dam_operational_month");
    strcpy(out_metadata[OUT_DAM_OP_MONTH].standard_name,
           "dam_operational_month");
    strcpy(out_metadata[OUT_DAM_OP_MONTH].units, "#");
    strcpy(out_metadata[OUT_DAM_OP_MONTH].description,
           "dam_operational_month");

    strcpy(out_metadata[OUT_DAM_HIST_DEMAND].varname, "OUT_DAM_HIST_DEMAND");
    strcpy(out_metadata[OUT_DAM_HIST_DEMAND].long_name, "dam_historical_demand");
    strcpy(out_metadata[OUT_DAM_HIST_DEMAND].standard_name,
           "dam_historical_demand");
    strcpy(out_metadata[OUT_DAM_HIST_DEMAND].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_HIST_DEMAND].description,
           "dam_historical_demand");

    strcpy(out_metadata[OUT_DAM_HIST_FLOW].varname, "OUT_DAM_HIST_FLOW");
    strcpy(out_metadata[OUT_DAM_HIST_FLOW].long_name, "dam_historical_inflow");
    strcpy(out_metadata[OUT_DAM_HIST_FLOW].standard_name,
           "dam_historical_inflow");
    strcpy(out_metadata[OUT_DAM_HIST_FLOW].units, "m3/s");
    strcpy(out_metadata[OUT_DAM_HIST_FLOW].description,
           "dam_historical_inflow");

    out_metadata[OUT_DAM_VOLUME].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_DISCHARGE].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_INFLOW].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_OP_DISCHARGE].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_OP_DISCHARGE_IRR].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_OP_VOLUME].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_OP_MONTH].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_HIST_DEMAND].nelem = options.MAXDAMS;
    out_metadata[OUT_DAM_HIST_FLOW].nelem = options.MAXDAMS;
}

void
dam_state_metadata(void)
{
    
}