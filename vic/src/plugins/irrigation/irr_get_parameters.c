#include <vic.h>

bool
irr_get_global_parameters(char *cmdstr)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("IRRIGATION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.irrigation.nc_filename);
    }
    else if (strcasecmp("POND_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.IRR_POND = str_to_bool(flgstr);
    }
    else if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.IRR_POTENTIAL = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}

void
irr_validate_global_parameters(void)
{
    extern filenames_struct filenames;
    extern option_struct options;
    
    if(strcasecmp(filenames.irrigation.nc_filename, MISSING_S) == 0){
        log_err("IRRIGATION = TRUE but IRRIGATION_PARAMETERS is missing");
    }
    
    if(options.IRR_POTENTIAL && options.WATER_USE){
        log_err("POTENTIAL_IRRIGATION = TRUE but WATER_USE = TRUE");
    }
}
