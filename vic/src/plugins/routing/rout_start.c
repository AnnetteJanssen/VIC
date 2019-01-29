#include <vic.h>

void
rout_start(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.routing.nc_filename, NC_NOWRITE,
                     &(filenames.routing.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.routing.nc_filename);

    options.IUH_NSTEPS = get_nc_dimension(&(filenames.routing),
                                           "time_uh_inflow");
    options.RUH_NSTEPS = get_nc_dimension(&(filenames.routing),
                                           "time_uh_runoff");

    // close routing parameter file
    status = nc_close(filenames.routing.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.routing.nc_filename);
}

void
rout_validate_domain(void)
{
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.routing.nc_filename, NC_NOWRITE,
                     &(filenames.routing.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.routing.nc_filename);
    
    compare_ncdomain_with_global_domain(&filenames.routing);

    // close routing parameter file
    status = nc_close(filenames.routing.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.routing.nc_filename);
}