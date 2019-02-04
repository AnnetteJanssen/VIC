#include <vic_driver_image.h>
#include <plugin.h>

void
dam_start(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern global_param_struct     global_param;

    int                     status;
    
    // Check domain & get dimensions
    status = nc_open(plugin_filenames.dams.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.dams.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.dams.nc_filename);

    plugin_options.NDAMTYPES = get_nc_dimension(&(plugin_filenames.dams),
                                           "dam_class");
    plugin_options.NDAMSERVICE = get_nc_dimension(&(plugin_filenames.dams),
                                           "dam_service");
    compare_ncdomain_with_global_domain(&plugin_filenames.dams);

    status = nc_close(plugin_filenames.dams.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.dams.nc_filename);
}
