#include <vic.h>

bool
gw_get_global_parameters(char *cmdstr)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("GROUNDWATER", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.GROUNDWATER = str_to_bool(flgstr);
    }
    else if (strcasecmp("GROUNDWATER_EQUILIBRIUM", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.GW_EQUILIBRIUM = str_to_bool(flgstr);
    }
    else if (strcasecmp("GROUNDWATER_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.groundwater.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
gw_validate_global_parameters(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    if (strcasecmp(filenames.groundwater.nc_filename, MISSING_S) == 0) {
        log_err("GROUNDWATER = TRUE but GROUNDWATER_PARAMETERS is missing");
    }
    if (options.GW_EQUILIBRIUM){
        log_info("Running groundwater for equilibrium");
    }
}
