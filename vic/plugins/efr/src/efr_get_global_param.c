#include <vic_driver_image.h>
#include <plugin.h>

bool
efr_get_global_param(char *cmdstr)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;

    char                              optstr[MAXSTRING];
    char                              flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("EFR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.EFR = str_to_bool(flgstr);
    }
    else if (strcasecmp("EFR_FORCING_FILE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.ef_path_pfx);
    }
    else {
        return false;
    }

    return true;
}

void
efr_validate_global_param(void)
{
    extern plugin_filenames_struct    plugin_filenames;

    // Forcing
    if (strcasecmp(plugin_filenames.ef_path_pfx, MISSING_S) == 0) {
        log_err("EFR = TRUE but file is missing");
    }
}
