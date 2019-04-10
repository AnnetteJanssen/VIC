#include <plugin.h>

void
plugin_initialize_options(void)
{
    extern plugin_option_struct plugin_options;

    plugin_options.DECOMPOSITION = RANDOM_DECOMPOSITION;
    plugin_options.ROUTING = false;
    plugin_options.EFR = false;
    plugin_options.DAMS = false;
    plugin_options.UH_LENGTH = 0;
    plugin_options.FORCE_ROUTING = false;
    plugin_options.NDAMTYPES = 0;
    plugin_options.NDAMSERVICE = 0;
}

void
plugin_initialize_global(void)
{
    extern plugin_global_param_struct plugin_global_param;

    plugin_global_param.rout_steps_per_day = 0;
    plugin_global_param.rout_dt = 0;
}

void
plugin_initialize_parameters(void)
{
    extern plugin_parameters_struct plugin_param;

    plugin_param.DAM_ALPHA = 0.85;
    plugin_param.DAM_BETA = 0.6;
    plugin_param.DAM_GAMMA = 5;
}

void
plugin_initialize_filenames(void)
{
    extern plugin_filenames_struct plugin_filenames;
    
    size_t i;
    
    snprintf(plugin_filenames.routing.nc_filename, MAXSTRING, "%s", MISSING_S);
    snprintf(plugin_filenames.decomposition.nc_filename, MAXSTRING, "%s", MISSING_S);
    snprintf(plugin_filenames.dams.nc_filename, MAXSTRING, "%s", MISSING_S);
    
    for(i = 0; i < PLUGIN_N_FORCING_TYPES; i++){
        snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING, "%s", MISSING_S);
        snprintf(plugin_filenames.f_path_pfx[i], MAXSTRING, "%s", MISSING_S);
        snprintf(plugin_filenames.f_varname[i], MAXSTRING, "%s", MISSING_S);
    }
}

void
plugin_initialize_global_structures(void)
{
    plugin_initialize_options();
    plugin_initialize_global();
    plugin_initialize_parameters();
    plugin_initialize_filenames();
}
