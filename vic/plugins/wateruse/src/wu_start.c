#include <vic_driver_image.h>
#include <plugin.h>

void
wu_start(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern global_param_struct     global_param;

    int                     status;
    
    // Check domain & get dimensions
    status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.wateruse.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.wateruse.nc_filename);
    
    compare_ncdomain_with_global_domain(&plugin_filenames.wateruse);

    status = nc_close(plugin_filenames.wateruse.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.wateruse.nc_filename);

    // Check the forcing
    sprintf(plugin_filenames.wateruse_forcing.nc_filename, "%s%4d.nc", 
            plugin_filenames.rf_path_pfx, global_param.startyear);
    status = nc_open(plugin_filenames.wateruse_forcing.nc_filename, NC_NOWRITE, 
            &(plugin_filenames.wateruse_forcing.nc_id));
    check_nc_status(status, "Error opening %s",  
            plugin_filenames.wateruse_forcing.nc_filename);

    plugin_get_forcing_file_info(&plugin_filenames.wateruse_forcing);
    compare_ncdomain_with_global_domain(&plugin_filenames.wateruse_forcing);

    status = nc_close(plugin_filenames.wateruse_forcing.nc_id);
    check_nc_status(status, "Error closing %s", 
            plugin_filenames.wateruse_forcing.nc_filename);
}
