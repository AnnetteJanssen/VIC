#include <vic.h>

void
rout_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].varname,
           "OUT_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].long_name, "discharge");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].standard_name,
           "discharge");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].description,
           "discharge");

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_STREAM_MOIST")].varname,
           "OUT_STREAM_MOIST");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_STREAM_MOIST")].long_name,
           "stream_storage");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_STREAM_MOIST")].standard_name,
           "stream storage");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_STREAM_MOIST")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_STREAM_MOIST")].description,
           "stream storage");

    out_metadata[list_search_id(outvar_types, "OUT_STREAM_MOIST")].nelem = 1;
    out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].nelem = 1;
}

void
rout_state_metadata(void)
{
}
