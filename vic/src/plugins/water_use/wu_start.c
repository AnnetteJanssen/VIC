#include <vic.h>

void
wu_start(void)
{
    extern option_struct options;
    extern filenames_struct filenames;
    
    int status;
    
    // open routing parameter file
    status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                     &(filenames.water_use.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.water_use.nc_filename);

    options.MAXRECEIVING = get_nc_dimension(&(filenames.water_use), 
            "max_receiving");
    options.MAXSENDING = get_nc_dimension(&(filenames.water_use), 
            "max_sending");

    // close routing parameter file
    status = nc_close(filenames.water_use.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.water_use.nc_filename);
}