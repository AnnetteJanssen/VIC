#include <vic_driver_image.h>
#include <plugin.h>

void
efr_start(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern global_param_struct     global_param;

    int                            status;
    
    // Check the forcing
    
    // discharge
    snprintf(plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_filename, MAXSTRING,
             "%s%4d.nc",
             plugin_filenames.f_path_pfx[FORCING_EFR_DISCHARGE], global_param.startyear);
    status = nc_open(plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_filename,
                     NC_NOWRITE,
                     &(plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_filename);

    plugin_get_forcing_file_info(&plugin_filenames.forcing[FORCING_EFR_DISCHARGE]);
    compare_ncdomain_with_global_domain(&plugin_filenames.forcing[FORCING_EFR_DISCHARGE]);

    status = nc_close(plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.forcing[FORCING_EFR_DISCHARGE].nc_filename);
    
    // baseflow
    snprintf(plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_filename, MAXSTRING,
             "%s%4d.nc",
             plugin_filenames.f_path_pfx[FORCING_EFR_BASEFLOW], global_param.startyear);
    status = nc_open(plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_filename,
                     NC_NOWRITE,
                     &(plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_filename);

    plugin_get_forcing_file_info(&plugin_filenames.forcing[FORCING_EFR_BASEFLOW]);
    compare_ncdomain_with_global_domain(&plugin_filenames.forcing[FORCING_EFR_BASEFLOW]);

    status = nc_close(plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.forcing[FORCING_EFR_BASEFLOW].nc_filename);
}
