#include <vic_driver_image.h>
#include <plugin.h>

void
set_wu_type(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    
    char                    typename[MAXSTRING];
    char                    sourcename[MAXSTRING];
    int                     type = WU_IRRIGATION;
    int                     source = WU_SKIP;

    strcpy(typename, "MISSING");
    strcpy(sourcename, "MISSING");
    
    sscanf(cmdstr, "%*s %s %s", typename, sourcename);
    
    if (strcasecmp("IRRIGATION", typename) == 0) {
        type = WU_IRRIGATION;
    }
    else if (strcasecmp("MUNICIPAL", typename) == 0) {
        type = WU_MUNICIPAL;
    }
    else if (strcasecmp("MANUFACTURING", typename) == 0) {
        type = WU_MANUFACTURING;
    }
    else if (strcasecmp("ENERGY", typename) == 0) {
        type = WU_ENERGY;
    }
    else if (strcasecmp("LIVESTOCK", typename) == 0) {
        type = WU_LIVESTOCK;
    } 
    else {
        log_err("Undefined wateruse forcing variable type %s",
                typename);
    }
    
    if (strcasecmp("FROM_FILE", sourcename) == 0) {
        source = WU_FROM_FILE;
    }
    else if (strcasecmp("CALCULATE", sourcename) == 0) {
        source = WU_CALCULATE;
    }
    else if (strcasecmp("SKIP", sourcename) == 0) {
        source = WU_SKIP;
    }
    else {
        log_err("Undefined wateruse forcing source type %s",
                sourcename);
    }
    
    plugin_options.WU_INPUT[type] = source;
}

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
    else if (strcasecmp("WU_TYPE", optstr) == 0) {
        set_wu_type(cmdstr);
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
    if (strcasecmp(plugin_filenames.wf_path_pfx, MISSING_S) == 0) {
        log_err("WATERUSE = TRUE but forcing is missing");
    }
}
