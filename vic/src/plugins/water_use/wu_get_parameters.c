#include <vic.h>

void
get_wu_forcing_files_info(size_t sector)
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
    get_nc_field_double(&(filenames.water_use_forcing[sector]), "time", &start, &count,
                        nc_times);
    get_nc_var_attr(&(filenames.forcing[sector]), "time", "units",
                    &nc_unit_chars);
    get_nc_var_attr(&(filenames.forcing[sector]), "time", "calendar",
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
        log_err("Water use forcing file time must match the forcing file time.  "
                "Forcing file time is set to %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds] and the water use forcing "
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
        global_param.snow_steps_per_day) {
        log_err("Water use forcing file timestep must match the snow model timestep.  "
                "Snow model timesteps per day is set to %zu and the forcing "
                "file timestep is set to %zu",
                global_param.snow_steps_per_day,
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

int
wu_get_sector_id(char *flgstr)
{    
    if(strcasecmp("IRR", flgstr) == 0){
        return WU_IRRIGATION;
    }else if(strcasecmp("DOM", flgstr) == 0){
        return WU_DOMESTIC;
    }else if(strcasecmp("IND", flgstr) == 0){
        return WU_INDUSTRIAL;
    }else if(strcasecmp("ENE", flgstr) == 0){
        return WU_ENERGY;
    }else if(strcasecmp("LIV", flgstr) == 0){
        return WU_LIVESTOCK;
    }else if(strcasecmp("ENV", flgstr) == 0){
        return WU_ENVIRONMENTAL;
    }else{
        log_err("WU_SECTOR SECTOR should be IRR(IGATION), DOM(ESTIC),"
                "IND(USTRIAL), ENE(RGY), LIV(ESTOCK) or ENV(IRONMENTAL); %s is unknown", flgstr);
    }
}

bool
wu_get_global_parameters(char *cmdstr)
{    
    extern option_struct options;
    extern filenames_struct filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    char sector[MAXSTRING];
    char source[MAXSTRING];
    char file[MAXSTRING];
    char strategy[MAXSTRING];
    char priority_sector[WU_NSECTORS][MAXSTRING];
    int cur_sector;
    
    size_t i;
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("WATER_USE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.WATER_USE = str_to_bool(flgstr);
    }
    else if (strcasecmp("WATER_USE_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.water_use.nc_filename);
    }
    
    else if (strcasecmp("WU_SECTOR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s %s %s", sector, source, file);
        
        cur_sector = wu_get_sector_id(sector);
        
        if(strcasecmp("CALCULATE", source) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_CALCULATE;
        }else if(strcasecmp("NONE", source) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_NONE;
        }else if(strcasecmp("FROM_FILE", source) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_FROM_FILE;
        }else{
            log_err("WU_SECTOR SOURCE should be CALCULATE, NONE or FROM_FILE; "
                    "%s is unknown", source);
        }
        
        strcpy(filenames.water_use_forcing_pfx[cur_sector], file);
    }
    
    else if (strcasecmp("WU_STRATEGY", optstr) == 0) {
        sscanf(cmdstr, "%*s %s %s %s %s %s %s %s", strategy, 
                priority_sector[0],priority_sector[1],
                priority_sector[2],priority_sector[3],
                priority_sector[4],priority_sector[5]);
        
        if(strcasecmp("EQUAL", strategy) == 0){
            options.WU_STRATEGY = WU_STRATEGY_EQUAL;
        }else if(strcasecmp("PRIORITY", strategy) == 0){
            options.WU_STRATEGY = WU_STRATEGY_PRIORITY;
            
            for(i = 0; i < WU_NSECTORS; i++){
                options.WU_PRIORITY[i] = wu_get_sector_id(priority_sector[i]);
            }
        }else{
            log_err("WU_STRATEGY should be EQUAL or PRIORITY; "
                    "%s is unknown", strategy);
        }
    }     
    else if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.IRR_POTENTIAL = str_to_bool(flgstr);
    }
    else if (strcasecmp("REMOTE_WATER_USE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.WU_REMOTE = str_to_bool(flgstr);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
wu_validate_global_parameters(void)
{
    extern filenames_struct filenames;
    extern option_struct options;
    extern global_param_struct global_param;
    
    size_t i;
    int status;
    
    if(!options.ROUTING){
        log_err("WATER_USE = TRUE but ROUTING = FALSE");
    }
    
    if(options.IRR_POTENTIAL && !options.IRRIGATION){
        log_err("POTENTIAL_IRRIGATION = TRUE but IRRIGATION = FALSE");
    }
    
    if(!options.WU_REMOTE){
        if(strcasecmp(filenames.water_use.nc_filename, MISSING_S) == 0){
            log_err("WATER_USE_REMOTE = TRUE but WATER_USE_PARAMETERS is missing");
        }
    }
    
    for(i = 0; i < WU_NSECTORS; i ++){
        if(options.WU_INPUT_LOCATION[i] == WU_INPUT_FROM_FILE){
            if(strcasecmp(filenames.water_use_forcing_pfx[i], MISSING_S) == 0){
                log_err("WATER_USE SOURCE = FROM_FILE but WATER_USE_FORCING is missing");
            }
                        
            // Open first-year forcing files and get info
            sprintf(filenames.water_use_forcing[i].nc_filename, "%s%4d.nc",
                    filenames.water_use_forcing_pfx[i], global_param.startyear);
            status = nc_open(filenames.water_use_forcing[i].nc_filename, NC_NOWRITE,
                             &(filenames.water_use_forcing[i].nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.water_use_forcing[i].nc_filename);  
            
            // Get information from the forcing file(s)
            get_wu_forcing_files_info(i); 
            
            // Close first-year forcing files
            status = nc_close(filenames.water_use_forcing[i].nc_id);
            check_nc_status(status, "Error closing %s",
                            filenames.water_use_forcing[i].nc_filename);
            
        } else if(options.WU_INPUT_LOCATION[i] == WU_INPUT_CALCULATE){
            if(i == WU_IRRIGATION && !options.IRRIGATION){
                log_warn("WATER_USE SOURCE = CALCULATE but IRRIGATION = FALSE is missing. "
                        "ignoring IRRIGATION for now...");
            }
            if(i == WU_ENVIRONMENTAL && !options.EFR){
                log_warn("WATER_USE SOURCE = CALCULATE but EFR = FALSE is missing. "
                        "ignoring EFR for now...");
            }
        }
    }
}