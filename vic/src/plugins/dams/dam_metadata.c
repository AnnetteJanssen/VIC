#include <vic.h>

void
dam_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;
    extern option_struct    options;
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].varname, "OUT_DAM_VOLUME");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].long_name, "dam_water_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].standard_name,
           "dam_water_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].units, "mcm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].description,
           "dam_water_volume");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].varname, "OUT_DAM_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].long_name, "dam_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].standard_name,
           "dam_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].description,
           "dam_discharge");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].varname, "OUT_DAM_AREA");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].long_name, "dam_water_area");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].standard_name,
           "dam_water_area");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].units, "km2");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].description,
           "dam_water_area");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].varname, "OUT_DAM_HEIGHT");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].long_name, "dam_water_height");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].standard_name,
           "dam_water_height");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].units, "m");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].description,
           "dam_water_height");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].varname, "OUT_DAM_OP_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].long_name, "dam_operational_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].standard_name,
           "dam_operational_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].description,
           "dam_operational_discharge");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].varname, "OUT_DAM_OP_VOLUME");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].long_name, "dam_operational_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].standard_name,
           "dam_operational_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].units, "km3");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].description,
           "dam_operational_volume");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].varname, "OUT_DAM_OP_MONTH");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].long_name, "dam_operational_month");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].standard_name,
           "dam_operational_month");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].units, "#");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].description,
           "dam_operational_month");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].varname, "OUT_DAM_HIST_DEMAND");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].long_name, "dam_historical_demand");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].standard_name,
           "dam_historical_demand");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].description,
           "dam_historical_demand");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].varname, "OUT_DAM_HIST_FLOW");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].long_name, "dam_historical_inflow");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].standard_name,
           "dam_historical_inflow");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].description,
           "dam_historical_inflow");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].varname, "OUT_DAM_HIST_SHORTAGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].long_name, "dam_historical_shortage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].standard_name,
           "dam_historical_shortage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].description,
           "dam_historical_shortage");

    out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_DEMAND")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_FLOW")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_HIST_SHORTAGE")].nelem = options.MAXDAMS;
}

void
dam_state_metadata(void)
{
    
}