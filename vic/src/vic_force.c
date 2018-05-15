
#include <vic.h>

/******************************************************************************
 * @brief    Read atmospheric forcing data.
 *****************************************************************************/
void
vic_force(void)
{
    extern option_struct options;
    
    // Force all non specific VIC structures
    force_general();
    
    // Force all plugins
    if(options.WATER_USE){
        wu_forcing();
    }
    if(options.EFR){
        efr_forcing();
    }
}