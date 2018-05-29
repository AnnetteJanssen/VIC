#include <vic.h>

void
efr_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    int OUT_EFR_DIS_REQ;
    int OUT_EFR_BASE_REQ;
    
    OUT_EFR_DIS_REQ = list_search_id(outvar_types, "OUT_EFR_DIS_REQ");
    OUT_EFR_BASE_REQ = list_search_id(outvar_types, "OUT_EFR_BASE_REQ");
    
    strcpy(out_metadata[OUT_EFR_DIS_REQ].varname, "OUT_EFR_DIS_REQ");
    strcpy(out_metadata[OUT_EFR_DIS_REQ].long_name, "environmental_flow_requirement");
    strcpy(out_metadata[OUT_EFR_DIS_REQ].standard_name, "environmental_flow_requirement");
    strcpy(out_metadata[OUT_EFR_DIS_REQ].units, "m3/s");
    strcpy(out_metadata[OUT_EFR_DIS_REQ].description, "environmental_flow_requirement");
    
    strcpy(out_metadata[OUT_EFR_BASE_REQ].varname, "OUT_EFR_BASE_REQ");
    strcpy(out_metadata[OUT_EFR_BASE_REQ].long_name, "environmental_baseflow_requirement");
    strcpy(out_metadata[OUT_EFR_BASE_REQ].standard_name, "environmental_baseflow_requirement");
    strcpy(out_metadata[OUT_EFR_BASE_REQ].units, "mm");
    strcpy(out_metadata[OUT_EFR_BASE_REQ].description, "environmental_baseflow_requirement");
    
    out_metadata[OUT_EFR_DIS_REQ].nelem = 1;
    out_metadata[OUT_EFR_BASE_REQ].nelem = 1;
}

void
efr_state_metadata(void)
{
}
