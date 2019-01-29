#include <vic.h>

void
get_efr_forcing_files_info()
{    
    extern global_param_struct global_param;
    extern filenames_struct    filenames;

    double                     nc_times[2];
    double                     nc_time_origin;
    size_t                     start = 0;
    size_t                     count = 2;
    char                      *nc_unit_chars = NULL;
    char                      *calendar_char = NULL;
    unsigned short int         time_units;
    unsigned short int         calendar;
    dmy_struct                 nc_origin_dmy;
    dmy_struct                 nc_start_dmy;
    size_t                     force_step_per_day;
    unsigned short int         forceyear;
    unsigned short int         forcemonth;
    unsigned short int         forceday;
    unsigned short int         forcesecond;
    
    // read time info from netcdf file
    get_nc_field_double(&(filenames.efr_forcing), "time", &start, &count,
                        nc_times);
    get_nc_var_attr(&(filenames.efr_forcing), "time", "units",
                    &nc_unit_chars);
    get_nc_var_attr(&(filenames.efr_forcing), "time", "calendar",
                    &calendar_char);

    // parse the calendar string and check to make sure it matches the global clock
    calendar = str_to_calendar(calendar_char);

    // parse the time units
    parse_nc_time_units(nc_unit_chars, &time_units, &nc_origin_dmy);

    // Get date/time of the first entry in the forcing file.
    nc_time_origin =
        date2num(0., &nc_origin_dmy, 0., calendar, TIME_UNITS_DAYS);
    num2date(nc_time_origin, nc_times[0], 0., calendar, time_units,
             &nc_start_dmy);

    // Assign file start date/time            
    forceyear = nc_start_dmy.year;
    forcemonth = nc_start_dmy.month;
    forceday = nc_start_dmy.day;
    forcesecond = nc_start_dmy.dayseconds;

    if(forceyear != global_param.forceyear[0] ||
            forcemonth != global_param.forcemonth[0] ||
            forceday != global_param.forceday[0] ||
            forcesecond != global_param.forcesec[0]){
        log_err("Efr forcing file time must match the forcing file time.  "
                "Forcing file time is set to %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds] and the efr forcing "
                "file time is set to  %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds]",
                global_param.forceyear[0],
                global_param.forcemonth[0],
                global_param.forceday[0],
                global_param.forcesec[0],
                forceyear,
                forcemonth,
                forceday,
                forcesecond);
    }

    // calculate timestep in forcing file
    if (time_units == TIME_UNITS_DAYS) {
        force_step_per_day =
            (size_t) nearbyint(1. / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_HOURS) {
       force_step_per_day =
            (size_t) nearbyint(HOURS_PER_DAY / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_MINUTES) {
        force_step_per_day =
            (size_t) nearbyint(MIN_PER_DAY / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_SECONDS) {
        force_step_per_day =
            (size_t) nearbyint(SEC_PER_DAY / (nc_times[1] - nc_times[0]));
    }

    // check that this forcing file will work
    if (force_step_per_day !=
        global_param.force_steps_per_day[0]) {
        log_err("Efr forcing file timestep must match the force model timestep.  "
                "Force model timesteps per day is set to %zu and the efr forcing "
                "file timestep is set to %zu",
                global_param.force_steps_per_day[0],
                force_step_per_day);
    }
    if (calendar != global_param.calendar) {
        log_err("Calendar in water use forcing file (%s) "
                "does not match the calendar of "
                "VIC's clock", calendar_char);
    }

    // Free attribute character arrays
    free(nc_unit_chars);
    free(calendar_char);
}

bool
efr_get_global_parameters(char *cmdstr)
{
    extern option_struct options;
    extern filenames_struct filenames;

    char                 optstr[MAXSTRING];
    char                 flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("EFR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.EFR = str_to_bool(flgstr);
    }
    else if (strcasecmp("EFR_FORCING_FILE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.efr_forcing_pfx);
    }
    else {
        return false;
    }

    return true;
}

void
efr_validate_global_param(void)
{
    extern option_struct options;
    extern filenames_struct filenames;
    extern global_param_struct global_param;
    
    int status;
    
    if(options.EFR){
        if(strcasecmp(filenames.efr_forcing_pfx, MISSING_S) == 0){
            log_err("EFR = TRUE but EFR_FORCING_FILE is missing");
        }

        // Open first-year forcing files and get info
        sprintf(filenames.efr_forcing.nc_filename, "%s%4d.nc",
                filenames.efr_forcing_pfx, global_param.startyear);
        status = nc_open(filenames.efr_forcing.nc_filename, NC_NOWRITE,
                         &(filenames.efr_forcing.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.efr_forcing.nc_filename);  

        // Get information from the forcing file(s)
        get_efr_forcing_files_info(); 

        // Close first-year forcing files
        status = nc_close(filenames.efr_forcing.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.efr_forcing.nc_filename);
    }
}
