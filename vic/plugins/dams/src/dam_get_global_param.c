#include <vic_driver_image.h>
#include <plugin.h>

bool
dam_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("DAMS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.DAMS = str_to_bool(flgstr);
    }
    else if (strcasecmp("DAMS_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.dams.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
dam_validate_global_param(void)
{
    extern plugin_filenames_struct plugin_filenames;
    
    // Parameters
    if (strcasecmp(plugin_filenames.dams.nc_filename, MISSING_S) == 0) {
        log_err("DAMS = TRUE but file is missing");
    }
}

bool
dam_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct    plugin_param;

    char                    optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("DAM_ALPHA", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.DAM_ALPHA);
    } else if (strcasecmp("DAM_BETA", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.DAM_BETA);
    }
    else {
        return false;
    }

    return true;
}

void
dam_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    if (!(plugin_param.DAM_ALPHA >= 0 && plugin_param.DAM_ALPHA <= 1)) {
        log_err("DAM_ALPHA must be defined on the interval [0,1] (-)");
    }
    if (!(plugin_param.DAM_BETA >= 0)) {
        log_err("DAM_BETA must be defined on the interval [0,Inf) (-)");
    }
}