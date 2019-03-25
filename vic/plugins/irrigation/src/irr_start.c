#include <vic_driver_image.h>
#include <plugin.h>

void
irr_start(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    int                            status;

    // Check domain & get dimensions
    status = nc_open(plugin_filenames.irrigation.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.irrigation.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.irrigation.nc_filename);

    plugin_options.NIRRTYPES = get_nc_dimension(&(plugin_filenames.irrigation),
                                                "irr_class");
    compare_ncdomain_with_global_domain(&plugin_filenames.irrigation);

    status = nc_close(plugin_filenames.irrigation.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.irrigation.nc_filename);
}
