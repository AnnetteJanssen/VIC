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
    else {
        return false;
    }

    return true;
}

void
wu_validate_global_param(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    
    // Options
    if(!plugin_options.ROUTING){
        log_err("WATERUSE = TRUE but ROUTING = FALSE");
    }
    if(plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION){
        log_err("WATERUSE = TRUE but DECOMPOSITION = RANDOM");
    }
   
    // Parameters
    if (strcasecmp(plugin_filenames.wateruse.nc_filename, MISSING_S) == 0) {
        log_err("WATERUSE = TRUE but file is missing");
    }
    
    // Forcing
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_IRRIGATION] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_MUNICIPAL] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_LIVESTOCK] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENG_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENG_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENG_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_ENERGY] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_MANUFACTURING] = WU_FROM_FILE;
    }
}
