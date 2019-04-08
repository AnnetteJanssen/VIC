#include <vic_driver_shared_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    This routine determines the current forcing file data type and
 *           stores its location in the description of the current forcing file.
 *****************************************************************************/
void
plugin_set_force_type(char *cmdstr)
{
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];
    char                    ncvarname[MAXSTRING];
    char                    ncfilename[MAXSTRING];
    int                     type = SKIP;

    strcpy(ncvarname, "MISSING");
    strcpy(ncfilename, "MISSING");

    /** Initialize flgstr **/
    strcpy(flgstr, "NULL");

    sscanf(cmdstr, "%*s %s %s %s", optstr, ncvarname, ncfilename);

    /***************************************
       Get meteorological data forcing info
    ***************************************/

    if (strcasecmp("DISCHARGE", optstr) == 0) {
        type = FORCING_DISCHARGE;
    }
    else if (strcasecmp("IRR_DEMAND", optstr) == 0) {
        type = FORCING_IRR_DEMAND;
    }
    else if (strcasecmp("MUN_DEMAND", optstr) == 0) {
        type = FORCING_MUN_DEMAND;
    }
    else if (strcasecmp("LIV_DEMAND", optstr) == 0) {
        type = FORCING_LIV_DEMAND;
    }
    else if (strcasecmp("MAN_DEMAND", optstr) == 0) {
        type = FORCING_MAN_DEMAND;
    }
    else if (strcasecmp("ENG_DEMAND", optstr) == 0) {
        type = FORCING_ENG_DEMAND;
    }
    else if (strcasecmp("IRR_GROUNDWATER", optstr) == 0) {
        type = FORCING_IRR_GROUNDWATER;
    }
    else if (strcasecmp("MUN_GROUNDWATER", optstr) == 0) {
        type = FORCING_MUN_GROUNDWATER;
    }
    else if (strcasecmp("LIV_GROUNDWATER", optstr) == 0) {
        type = FORCING_LIV_GROUNDWATER;
    }
    else if (strcasecmp("MAN_GROUNDWATER", optstr) == 0) {
        type = FORCING_MAN_GROUNDWATER;
    }
    else if (strcasecmp("ENG_GROUNDWATER", optstr) == 0) {
        type = FORCING_ENG_GROUNDWATER;
    }
    else if (strcasecmp("IRR_CONSUMPTION", optstr) == 0) {
        type = FORCING_IRR_CONSUMPTION;
    }
    else if (strcasecmp("MUN_CONSUMPTION", optstr) == 0) {
        type = FORCING_MUN_CONSUMPTION;
    }
    else if (strcasecmp("LIV_CONSUMPTION", optstr) == 0) {
        type = FORCING_LIV_CONSUMPTION;
    }
    else if (strcasecmp("MAN_CONSUMPTION", optstr) == 0) {
        type = FORCING_MAN_CONSUMPTION;
    }
    else if (strcasecmp("ENG_CONSUMPTION", optstr) == 0) {
        type = FORCING_ENG_CONSUMPTION;
    }
    /** Undefined variable type **/
    else {
        log_err("Undefined forcing variable type %s in forcing file.",
                optstr);
    }

    if (strcasecmp("MISSING", ncvarname) != 0) {
        strcpy(plugin_filenames.f_varname[type], ncvarname);
    }
    else {
        log_err(
            "Must supply netCDF variable name for %s forcing file",
            optstr);
    }

    if (strcasecmp("MISSING", ncfilename) != 0) {
        strcpy(plugin_filenames.f_path_pfx[type], ncfilename);
    }
    else {
        log_err(
            "Must supply netCDF file name for %s forcing file",
            optstr);
    }
}
