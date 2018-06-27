#include <vic.h>

void
plugin_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    int                     OUT_TIME_VICPLUGIN_WALL;
    int                     OUT_TIME_VICPLUGIN_CPU;
    
    OUT_TIME_VICPLUGIN_WALL = list_search_id(outvar_types, "OUT_TIME_VICPLUGIN_WALL");
    OUT_TIME_VICPLUGIN_CPU = list_search_id(outvar_types, "OUT_TIME_VICPLUGIN_CPU");
    
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_WALL].varname, "OUT_TIME_VICPLUGIN_WALL");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_WALL].long_name, "time_vicplugin_wall");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_WALL].standard_name, "time_vicplugin_wall");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_WALL].units, "seconds");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_WALL].description, "time_vicplugin_wall");

    strcpy(out_metadata[OUT_TIME_VICPLUGIN_CPU].varname, "OUT_TIME_VICPLUGIN_CPU");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_CPU].long_name, "time_vicplugin_cpu");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_CPU].standard_name, "time_vicplugin_cpu");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_CPU].units, "seconds");
    strcpy(out_metadata[OUT_TIME_VICPLUGIN_CPU].description, "time_vicplugin_cpu");

    out_metadata[OUT_TIME_VICPLUGIN_WALL].nelem = 1;
    out_metadata[OUT_TIME_VICPLUGIN_CPU].nelem = 1;
}
