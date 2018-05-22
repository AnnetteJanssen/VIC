#include <vic.h>

void
efr_validate_domain(void)
{
    extern filenames_struct filenames;
    extern global_param_struct global_param;
    
    int status;
                          
    // Open first-year forcing files and get info
    sprintf(filenames.efr_forcing.nc_filename, "%s%4d.nc",
            filenames.efr_forcing_pfx, global_param.startyear);
    status = nc_open(filenames.efr_forcing.nc_filename, NC_NOWRITE,
                     &(filenames.efr_forcing.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.efr_forcing.nc_filename);  

    // Get information from the forcing file(s)
    compare_ncdomain_with_global_domain(&filenames.efr_forcing); 

    // Close first-year forcing files
    status = nc_close(filenames.efr_forcing.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.efr_forcing.nc_filename);
}