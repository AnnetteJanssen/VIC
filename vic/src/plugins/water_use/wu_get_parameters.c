#include <vic.h>

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
        
        // TODO: implement compensation time for water use from file
        //sscanf(cmdstr, "%*s %*s %*s %d %*s", &options.WU_COMPENSATION_TIME[cur_sector]);
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
    
    size_t i;
    
    if(!options.ROUTING){
        log_err("WATER_USE = TRUE but ROUTING = FALSE");
    }
    
    for(i = 0; i < WU_NSECTORS; i ++){
        if(options.WU_INPUT_LOCATION[i] == WU_INPUT_FROM_FILE){
            if(strcasecmp(filenames.water_use_forcing_pfx[i], MISSING_S) == 0){
                log_err("WATER_USE = TRUE but WATER_USE_FORCING is missing");
            }
        }
    }
    
      // TODO: implement compensation time for water use from file
//    for(i = 0; i < WU_NSECTORS; i++){
//        if(options.WU_COMPENSATION_TIME[i] < 0){
//            log_err("WATER_USE_SECTOR COMPENSATION_TIME must be defined on the interval [0,inf) (days)");
//        }
//    }
}