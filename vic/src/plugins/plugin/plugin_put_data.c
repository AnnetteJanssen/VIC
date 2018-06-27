#include <vic.h>

void
plugin_put_data(double           **out_data,
                timer_struct      *timer)
{    
    extern node *outvar_types;
    extern int   N_OUTVAR_TYPES_ALL;
    
    int                     OUT_TIME_VICPLUGIN_WALL;
    int                     OUT_TIME_VICPLUGIN_CPU;
    
    OUT_TIME_VICPLUGIN_WALL = list_search_id(outvar_types, "OUT_TIME_VICPLUGIN_WALL");
    OUT_TIME_VICPLUGIN_CPU = list_search_id(outvar_types, "OUT_TIME_VICPLUGIN_CPU");
    
    // vic_plugin run time
    out_data[OUT_TIME_VICPLUGIN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_VICPLUGIN_CPU][0] = timer->delta_cpu;

    outvar_types = list_add_ids(outvar_types, N_OUTVAR_TYPES);
    N_OUTVAR_TYPES_ALL = list_count(outvar_types) + N_OUTVAR_TYPES;
}