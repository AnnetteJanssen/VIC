#include <vic.h>

void
dam_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;
 
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
    
    //OUT_LDAM_INFLOW
    strcpy(out_metadata[OUT_LDAM_INFLOW].varname, "OUT_LDAM_INFLOW");
    strcpy(out_metadata[OUT_LDAM_INFLOW].long_name, 
            "local_dam_inflow");
    strcpy(out_metadata[OUT_LDAM_INFLOW].standard_name,
           "local_dam_inflow");
    strcpy(out_metadata[OUD_LDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[OUD_LDAM_RELEASE].description,
           "current reservoir inflow");

    //OUT_LDAM_DEMAND
    strcpy(out_metadata[OUT_LDAM_DEMAND].varname, "OUT_LDAM_DEMAND");
    strcpy(out_metadata[OUT_LDAM_DEMAND].long_name, 
            "local_dam_demand");
    strcpy(out_metadata[OUT_LDAM_DEMAND].standard_name,
           "local_dam_demand");
    strcpy(out_metadata[OUT_LDAM_DEMAND].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_DEMAND].description,
           "current service cells demand");

    //OUT_LDAM_EFR
    strcpy(out_metadata[OUT_LDAM_EFR].varname, "OUT_LDAM_EFR");
    strcpy(out_metadata[OUT_LDAM_EFR].long_name, 
            "local_dam_environmental_flow_requirement");
    strcpy(out_metadata[OUT_LDAM_EFR].standard_name,
           "local_dam_environmental_flow_requirement");
    strcpy(out_metadata[OUT_LDAM_EFR].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_EFR].description,
           "current environmental flow requirement");
    
    //OUT_LDAM_STORAGE
    strcpy(out_metadata[OUT_LDAM_STORAGE].varname, "OUT_LDAM_STORAGE");
    strcpy(out_metadata[OUT_LDAM_STORAGE].long_name, 
            "local_dam_storage");
    strcpy(out_metadata[OUT_LDAM_STORAGE].standard_name,
           "local_dam_storage");
    strcpy(out_metadata[OUT_LDAM_STORAGE].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_STORAGE].description,
           "current reservoir storage");

    //OUD_LDAM_RELEASE
    strcpy(out_metadata[OUD_LDAM_RELEASE].varname, "OUD_LDAM_RELEASE");
    strcpy(out_metadata[OUD_LDAM_RELEASE].long_name, 
            "local_dam_release");
    strcpy(out_metadata[OUD_LDAM_RELEASE].standard_name,
           "local_dam_release");
    strcpy(out_metadata[OUD_LDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[OUD_LDAM_RELEASE].description,
           "current reservoir release");

    //OUT_LDAM_OP_RELEASE
    strcpy(out_metadata[OUT_LDAM_OP_RELEASE].varname, "OUT_LDAM_OP_RELEASE");
    strcpy(out_metadata[OUT_LDAM_OP_RELEASE].long_name, 
            "local_dam_operational_release");
    strcpy(out_metadata[OUT_LDAM_OP_RELEASE].standard_name,
           "local_dam_operational_release");
    strcpy(out_metadata[OUT_LDAM_OP_RELEASE].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_OP_RELEASE].description,
           "operational reservoir release");

    //OUT_LDAM_OP_STORAGE
    strcpy(out_metadata[OUT_LDAM_OP_STORAGE].varname, "OUT_LDAM_OP_STORAGE");
    strcpy(out_metadata[OUT_LDAM_OP_STORAGE].long_name, 
            "local_dam_operational_storage");
    strcpy(out_metadata[OUT_LDAM_OP_STORAGE].standard_name,
           "local_dam_operational_storage");
    strcpy(out_metadata[OUT_LDAM_OP_STORAGE].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_OP_STORAGE].description,
           "operational reservoir storage");

    //OUT_LDAM_HIST_INFLOW
    strcpy(out_metadata[OUT_LDAM_HIST_INFLOW].varname, "OUT_LDAM_HIST_INFLOW");
    strcpy(out_metadata[OUT_LDAM_HIST_INFLOW].long_name, 
            "local_dam_historical_inflow");
    strcpy(out_metadata[OUT_LDAM_HIST_INFLOW].standard_name,
           "local_dam_historical_inflow");
    strcpy(out_metadata[OUT_LDAM_HIST_INFLOW].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_HIST_INFLOW].description,
           "historical reservoir inflow");

    //OUT_LDAM_HIST_DEMAND
    strcpy(out_metadata[OUT_LDAM_HIST_DEMAND].varname, "OUT_LDAM_HIST_DEMAND");
    strcpy(out_metadata[OUT_LDAM_HIST_DEMAND].long_name, 
            "local_dam_historical_demand");
    strcpy(out_metadata[OUT_LDAM_HIST_DEMAND].standard_name,
           "local_dam_historical_demand");
    strcpy(out_metadata[OUT_LDAM_HIST_DEMAND].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_HIST_DEMAND].description,
           "historical service cells demand");

    //OUT_LDAM_HIST_INFLOW
    strcpy(out_metadata[OUT_LDAM_HIST_EFR].varname, "OUT_LDAM_HIST_EFR");
    strcpy(out_metadata[OUT_LDAM_HIST_EFR].long_name, 
            "local_dam_historical_environmental_flow_requirement");
    strcpy(out_metadata[OUT_LDAM_HIST_EFR].standard_name,
           "local_dam_historical_environmental_flow_requirement");
    strcpy(out_metadata[OUT_LDAM_HIST_EFR].units, "hm3");
    strcpy(out_metadata[OUT_LDAM_HIST_EFR].description,
           "historical environmental flow requirement");
    
    
    //OUT_GDAM_INFLOW
    strcpy(out_metadata[OUT_GDAM_INFLOW].varname, "OUT_GDAM_INFLOW");
    strcpy(out_metadata[OUT_GDAM_INFLOW].long_name, 
            "global_dam_inflow");
    strcpy(out_metadata[OUT_GDAM_INFLOW].standard_name,
           "global_dam_inflow");
    strcpy(out_metadata[OUD_GDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[OUD_GDAM_RELEASE].description,
           "current reservoir inflow");

    //OUT_GDAM_DEMAND
    strcpy(out_metadata[OUT_GDAM_DEMAND].varname, "OUT_GDAM_DEMAND");
    strcpy(out_metadata[OUT_GDAM_DEMAND].long_name, 
            "global_dam_demand");
    strcpy(out_metadata[OUT_GDAM_DEMAND].standard_name,
           "global_dam_demand");
    strcpy(out_metadata[OUT_GDAM_DEMAND].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_DEMAND].description,
           "current service cells demand");

    //OUT_GDAM_EFR
    strcpy(out_metadata[OUT_GDAM_EFR].varname, "OUT_GDAM_EFR");
    strcpy(out_metadata[OUT_GDAM_EFR].long_name, 
            "global_dam_environmental_flow_requirement");
    strcpy(out_metadata[OUT_GDAM_EFR].standard_name,
           "global_dam_environmental_flow_requirement");
    strcpy(out_metadata[OUT_GDAM_EFR].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_EFR].description,
           "current environmental flow requirement");
    
    //OUT_GDAM_STORAGE
    strcpy(out_metadata[OUT_GDAM_STORAGE].varname, "OUT_GDAM_STORAGE");
    strcpy(out_metadata[OUT_GDAM_STORAGE].long_name, 
            "global_dam_storage");
    strcpy(out_metadata[OUT_GDAM_STORAGE].standard_name,
           "global_dam_storage");
    strcpy(out_metadata[OUT_GDAM_STORAGE].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_STORAGE].description,
           "current reservoir storage");

    //OUD_GDAM_RELEASE
    strcpy(out_metadata[OUD_GDAM_RELEASE].varname, "OUD_GDAM_RELEASE");
    strcpy(out_metadata[OUD_GDAM_RELEASE].long_name, 
            "global_dam_release");
    strcpy(out_metadata[OUD_GDAM_RELEASE].standard_name,
           "global_dam_release");
    strcpy(out_metadata[OUD_GDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[OUD_GDAM_RELEASE].description,
           "current reservoir release");

    //OUT_GDAM_OP_RELEASE
    strcpy(out_metadata[OUT_GDAM_OP_RELEASE].varname, "OUT_GDAM_OP_RELEASE");
    strcpy(out_metadata[OUT_GDAM_OP_RELEASE].long_name, 
            "global_dam_operational_release");
    strcpy(out_metadata[OUT_GDAM_OP_RELEASE].standard_name,
           "global_dam_operational_release");
    strcpy(out_metadata[OUT_GDAM_OP_RELEASE].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_OP_RELEASE].description,
           "operational reservoir release");

    //OUT_GDAM_OP_STORAGE
    strcpy(out_metadata[OUT_GDAM_OP_STORAGE].varname, "OUT_GDAM_OP_STORAGE");
    strcpy(out_metadata[OUT_GDAM_OP_STORAGE].long_name, 
            "global_dam_operational_storage");
    strcpy(out_metadata[OUT_GDAM_OP_STORAGE].standard_name,
           "global_dam_operational_storage");
    strcpy(out_metadata[OUT_GDAM_OP_STORAGE].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_OP_STORAGE].description,
           "operational reservoir storage");

    //OUT_GDAM_HIST_INFLOW
    strcpy(out_metadata[OUT_GDAM_HIST_INFLOW].varname, "OUT_GDAM_HIST_INFLOW");
    strcpy(out_metadata[OUT_GDAM_HIST_INFLOW].long_name, 
            "global_dam_historical_inflow");
    strcpy(out_metadata[OUT_GDAM_HIST_INFLOW].standard_name,
           "global_dam_historical_inflow");
    strcpy(out_metadata[OUT_GDAM_HIST_INFLOW].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_HIST_INFLOW].description,
           "historical reservoir inflow");

    //OUT_GDAM_HIST_DEMAND
    strcpy(out_metadata[OUT_GDAM_HIST_DEMAND].varname, "OUT_GDAM_HIST_DEMAND");
    strcpy(out_metadata[OUT_GDAM_HIST_DEMAND].long_name, 
            "global_dam_historical_demand");
    strcpy(out_metadata[OUT_GDAM_HIST_DEMAND].standard_name,
           "global_dam_historical_demand");
    strcpy(out_metadata[OUT_GDAM_HIST_DEMAND].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_HIST_DEMAND].description,
           "historical service cells demand");

    //OUT_GDAM_HIST_INFLOW
    strcpy(out_metadata[OUT_GDAM_HIST_EFR].varname, "OUT_GDAM_HIST_EFR");
    strcpy(out_metadata[OUT_GDAM_HIST_EFR].long_name, 
            "global_dam_historical_environmental_flow_requirement");
    strcpy(out_metadata[OUT_GDAM_HIST_EFR].standard_name,
           "global_dam_historical_environmental_flow_requirement");
    strcpy(out_metadata[OUT_GDAM_HIST_EFR].units, "hm3");
    strcpy(out_metadata[OUT_GDAM_HIST_EFR].description,
           "historical environmental flow requirement");

    out_metadata[OUT_LDAM_INFLOW].nelem = 1;
    out_metadata[OUT_LDAM_DEMAND].nelem = 1;
    out_metadata[OUT_LDAM_EFR].nelem = 1;
    out_metadata[OUT_LDAM_STORAGE].nelem = 1;
    out_metadata[OUD_LDAM_RELEASE].nelem = 1;
    out_metadata[OUT_LDAM_HIST_DEMAND].nelem = 1;
    out_metadata[OUT_LDAM_HIST_INFLOW].nelem = 1;
    out_metadata[OUT_LDAM_HIST_EFR].nelem = 1;
    out_metadata[OUT_LDAM_OP_RELEASE].nelem = 1;
    out_metadata[OUT_LDAM_OP_STORAGE].nelem = 1;
    
    out_metadata[OUT_GDAM_INFLOW].nelem = 1;
    out_metadata[OUT_GDAM_DEMAND].nelem = 1;
    out_metadata[OUT_GDAM_EFR].nelem = 1;
    out_metadata[OUT_GDAM_STORAGE].nelem = 1;
    out_metadata[OUD_GDAM_RELEASE].nelem = 1;
    out_metadata[OUT_GDAM_HIST_DEMAND].nelem = 1;
    out_metadata[OUT_GDAM_HIST_INFLOW].nelem = 1;
    out_metadata[OUT_GDAM_HIST_EFR].nelem = 1;
    out_metadata[OUT_GDAM_OP_RELEASE].nelem = 1;
    out_metadata[OUT_GDAM_OP_STORAGE].nelem = 1;
}

void
dam_state_metadata(void)
{
    
}