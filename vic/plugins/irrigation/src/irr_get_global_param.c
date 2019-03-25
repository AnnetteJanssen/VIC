#include <vic_driver_image.h>
#include <plugin.h>

bool
irr_get_global_param(char *cmdstr)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;

    char                              optstr[MAXSTRING];
    char                              flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.POTENTIAL_IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("IRRIGATION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.irrigation.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
irr_validate_global_param(void)
{
    extern plugin_filenames_struct    plugin_filenames;
    
    // Parameters
    if (strcasecmp(plugin_filenames.irrigation.nc_filename, MISSING_S) == 0) {
        log_err("IRRIGATION = TRUE but file is missing");
    }
}

bool
irr_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct   plugin_param;

    char                              optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("Wirr", optstr) == 0) {
        sscanf(cmdstr, "%*s %lf", &plugin_param.Wirr);
    }
    else {
        return false;
    }

    return true;
}

void
irr_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    if (!(plugin_param.Wirr >= 0 && plugin_param.Wirr <= 1)) {
        log_err("Wirr must be defined on the interval [0,1] (-)");
    }
}