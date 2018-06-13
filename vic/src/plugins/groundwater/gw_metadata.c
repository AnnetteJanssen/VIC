#include <vic.h>

void
gw_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    int                        OUT_GW_ZWT;
    int                        OUT_GW_RECHARGE;
    int                        OUT_GW_WA;
    int                        OUT_GW_WT;

    OUT_GW_ZWT = list_search_id(outvar_types, "OUT_GW_ZWT");
    OUT_GW_RECHARGE = list_search_id(outvar_types, "OUT_GW_RECHARGE");
    OUT_GW_WA = list_search_id(outvar_types, "OUT_GW_WA");
    OUT_GW_WT = list_search_id(outvar_types, "OUT_GW_WT");

    strcpy(out_metadata[OUT_GW_ZWT].varname, "OUT_GW_ZWT");
    strcpy(out_metadata[OUT_GW_ZWT].long_name, "groundwater_depth");
    strcpy(out_metadata[OUT_GW_ZWT].standard_name, "groundwater_depth");
    strcpy(out_metadata[OUT_GW_ZWT].units, "m");
    strcpy(out_metadata[OUT_GW_ZWT].description, "groundwater table depth");

    strcpy(out_metadata[OUT_GW_RECHARGE].varname, "OUT_GW_RECHARGE");
    strcpy(out_metadata[OUT_GW_RECHARGE].long_name, "groundwater_recharge");
    strcpy(out_metadata[OUT_GW_RECHARGE].standard_name, "groundwater_recharge");
    strcpy(out_metadata[OUT_GW_RECHARGE].units, "mm");
    strcpy(out_metadata[OUT_GW_RECHARGE].description, "groundwater recharge");

    strcpy(out_metadata[OUT_GW_WA].varname, "OUT_GW_WA");
    strcpy(out_metadata[OUT_GW_WA].long_name, "aquifer_storage");
    strcpy(out_metadata[OUT_GW_WA].standard_name, "aquifer_storage");
    strcpy(out_metadata[OUT_GW_WA].units, "mm");
    strcpy(out_metadata[OUT_GW_WA].description, "Aquifer storage relative to the reference (aquifer layer only)");

    strcpy(out_metadata[OUT_GW_WT].varname, "OUT_GW_WT");
    strcpy(out_metadata[OUT_GW_WT].long_name, "groundwater_storage");
    strcpy(out_metadata[OUT_GW_WT].standard_name, "groundwater_storage");
    strcpy(out_metadata[OUT_GW_WT].units, "mm");
    strcpy(out_metadata[OUT_GW_WT].description, "Groundwater storage relative to the reference (aquifer and soil layers combined)");

    out_metadata[OUT_GW_ZWT].nelem = 1;
    out_metadata[OUT_GW_RECHARGE].nelem = 1;
    out_metadata[OUT_GW_WA].nelem = 1;
    out_metadata[OUT_GW_WT].nelem = 1;
}

void
gw_state_metadata(void)
{
// TODO     extern metadata_struct state_metadata[N_STATE_VARS];
// strcpy(state_metadata[STATE_GW_ZWT].varname, "STATE_GW_ZWT");
// strcpy(state_metadata[STATE_GW_ZWT].long_name, "groundwater table depth");
// strcpy(state_metadata[STATE_GW_ZWT].standard_name,
// "groundwater table depth");
// strcpy(state_metadata[STATE_GW_ZWT].units, "m");
// strcpy(state_metadata[STATE_GW_ZWT].description,
// "groundwater table depth");
//
// strcpy(state_metadata[STATE_GW_WS].varname, "STATE_GW_WS");
// strcpy(state_metadata[STATE_GW_WS].long_name, "Water storage");
// strcpy(state_metadata[STATE_GW_WS].standard_name,
// "Water storage");
// strcpy(state_metadata[STATE_GW_WS].units, "mm");
// strcpy(state_metadata[STATE_GW_WS].description,
// "Water storage (between soil column and water table)");
//
}
