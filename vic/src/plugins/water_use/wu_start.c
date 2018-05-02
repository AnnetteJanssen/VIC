#include <vic.h>

void
wu_start(void)
{
    extern option_struct options;
    extern filenames_struct filenames;
    
    int status;
    
    if (options.WU_REMOTE) {
        // open routing parameter file
        status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                         &(filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.water_use.nc_filename);

        compare_ncdomain_with_global_domain(&filenames.water_use);

        options.MAXRECEIVING = get_nc_dimension(&(filenames.water_use), 
                "max_receiving");

        // close routing parameter file
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
}