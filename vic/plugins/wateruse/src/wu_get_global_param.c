#include <vic_driver_image.h>
#include <plugin.h>

bool
wu_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("WATERUSE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WATERUSE = str_to_bool(flgstr);
    }
    else if (strcasecmp("WATERUSE_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.wateruse.nc_filename);
    }
    else if (strcasecmp("WATERUSE_FORCING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.wf_path_pfx);
    }
    else {
        return false;
    }

    return true;
}

void
wu_validate_global_param(void)
{
    extern plugin_filenames_struct plugin_filenames;
        
    // Parameters
    if (strcasecmp(plugin_filenames.wateruse.nc_filename, MISSING_S) == 0) {
        log_err("WATERUSE = TRUE but file is missing");
    }
    
    // forcing
    if (strcasecmp(plugin_filenames.rf_path_pfx, MISSING_S) == 0) {
        log_err("WATERUSE = TRUE but file is missing");
    }
}
