#include <vic_driver_image.h>
#include <plugin.h>

void
wu_start(void)
{
    extern plugin_option_struct plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern global_param_struct     global_param;

    size_t                  i;
    int                     status;
    int demand_idx;
    int groundwater_idx;
    int consumption_idx;
    
    // Check domain & get dimensions
    status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.wateruse.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.wateruse.nc_filename);
    
    compare_ncdomain_with_global_domain(&plugin_filenames.wateruse);
    plugin_options.NWUTYPES = get_nc_dimension(&(plugin_filenames.routing),
                                           "wu_class");
    plugin_options.NWURECEIVING = get_nc_dimension(&(plugin_filenames.routing),
                                           "wu_receiving");
    
    if(plugin_options.NWUTYPES != WU_NSECTORS){
        log_err("Number of sectors in the water-use file [%d] "
                "does not match the number of sectors [%d]", 
                plugin_options.NWUTYPES,
                WU_NSECTORS);
    }

    status = nc_close(plugin_filenames.wateruse.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.wateruse.nc_filename);

    // Check the forcing
    for(i = 0; i < WU_NSECTORS; i++){
        if(plugin_options.WU_INPUT[i] == WU_FROM_FILE){
            if(i == WU_IRRIGATION){
                demand_idx = FORCING_IRR_DEMAND;
                groundwater_idx = FORCING_IRR_GROUNDWATER;
                consumption_idx = FORCING_IRR_CONSUMPTION;
            } else if (i == WU_MUNICIPAL){
                demand_idx = FORCING_MUN_DEMAND;
                groundwater_idx = FORCING_MUN_GROUNDWATER;
                consumption_idx = FORCING_MUN_CONSUMPTION;
            } else if (i == WU_LIVESTOCK){
                demand_idx = FORCING_LIV_DEMAND;
                groundwater_idx = FORCING_LIV_GROUNDWATER;
                consumption_idx = FORCING_LIV_CONSUMPTION;
            } else if (i == WU_ENERGY){
                demand_idx = FORCING_ENG_DEMAND;
                groundwater_idx = FORCING_ENG_GROUNDWATER;
                consumption_idx = FORCING_ENG_CONSUMPTION;
            } else if (i == WU_MANUFACTURING){
                demand_idx = FORCING_MAN_DEMAND;
                groundwater_idx = FORCING_MAN_GROUNDWATER;
                consumption_idx = FORCING_MAN_CONSUMPTION;
            } else {
                log_err("Unknown water-use sector");
            }
            
            // demand
            sprintf(plugin_filenames.forcing[demand_idx].nc_filename, "%s%4d.nc", 
                    plugin_filenames.f_path_pfx[demand_idx], global_param.startyear);
            status = nc_open(plugin_filenames.forcing[demand_idx].nc_filename, NC_NOWRITE, 
                    &(plugin_filenames.forcing[demand_idx].nc_id));
            check_nc_status(status, "Error opening %s",  
                    plugin_filenames.forcing[demand_idx].nc_filename);

            plugin_get_forcing_file_info(&plugin_filenames.forcing[demand_idx]);
            compare_ncdomain_with_global_domain(&plugin_filenames.forcing[demand_idx]);

            status = nc_close(plugin_filenames.forcing[demand_idx].nc_id);
            check_nc_status(status, "Error closing %s", 
                    plugin_filenames.forcing[demand_idx].nc_filename);
            
            // groundwater
            sprintf(plugin_filenames.forcing[groundwater_idx].nc_filename, "%s%4d.nc", 
                    plugin_filenames.f_path_pfx[groundwater_idx], global_param.startyear);
            status = nc_open(plugin_filenames.forcing[groundwater_idx].nc_filename, NC_NOWRITE, 
                    &(plugin_filenames.forcing[groundwater_idx].nc_id));
            check_nc_status(status, "Error opening %s",  
                    plugin_filenames.forcing[groundwater_idx].nc_filename);

            plugin_get_forcing_file_info(&plugin_filenames.forcing[groundwater_idx]);
            compare_ncdomain_with_global_domain(&plugin_filenames.forcing[groundwater_idx]);

            status = nc_close(plugin_filenames.forcing[groundwater_idx].nc_id);
            check_nc_status(status, "Error closing %s", 
                    plugin_filenames.forcing[groundwater_idx].nc_filename);
            
            // consumption
            sprintf(plugin_filenames.forcing[consumption_idx].nc_filename, "%s%4d.nc", 
                    plugin_filenames.f_path_pfx[consumption_idx], global_param.startyear);
            status = nc_open(plugin_filenames.forcing[consumption_idx].nc_filename, NC_NOWRITE, 
                    &(plugin_filenames.forcing[consumption_idx].nc_id));
            check_nc_status(status, "Error opening %s",  
                    plugin_filenames.forcing[consumption_idx].nc_filename);

            plugin_get_forcing_file_info(&plugin_filenames.forcing[consumption_idx]);
            compare_ncdomain_with_global_domain(&plugin_filenames.forcing[consumption_idx]);

            status = nc_close(plugin_filenames.forcing[consumption_idx].nc_id);
            check_nc_status(status, "Error closing %s", 
                    plugin_filenames.forcing[consumption_idx].nc_filename);
        }
    }
}
