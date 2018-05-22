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

        options.MAXRECEIVING = get_nc_dimension(&(filenames.water_use), 
                "max_receiving");

        // close routing parameter file
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
}

void
wu_validate_domain(void)
{
    extern option_struct options;
    extern filenames_struct filenames;
    extern global_param_struct global_param;
    
    int status;
    size_t i;
    
    if (options.WU_REMOTE) {
        // open routing parameter file
        status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                         &(filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.water_use.nc_filename);

        compare_ncdomain_with_global_domain(&filenames.water_use);

        // close routing parameter file
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
    
    for (i = 0; i < WU_NSECTORS; i++) {
        if(options.WU_INPUT_LOCATION[i] == WU_INPUT_FROM_FILE){                        
            // Open first-year forcing files and get info
            sprintf(filenames.water_use_forcing[i].nc_filename, "%s%4d.nc",
                    filenames.water_use_forcing_pfx[i], global_param.startyear);
            status = nc_open(filenames.water_use_forcing[i].nc_filename, NC_NOWRITE,
                             &(filenames.water_use_forcing[i].nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.water_use_forcing[i].nc_filename);  
            
            // Get information from the forcing file(s)
            compare_ncdomain_with_global_domain(&filenames.water_use_forcing[i]); 
            
            // Close first-year forcing files
            status = nc_close(filenames.water_use_forcing[i].nc_id);
            check_nc_status(status, "Error closing %s",
                            filenames.water_use_forcing[i].nc_filename);
        }
    }
}