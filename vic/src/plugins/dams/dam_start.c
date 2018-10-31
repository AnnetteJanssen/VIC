#include <vic.h>

void
dam_start(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                     &(filenames.dams.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.dams.nc_filename);

    options.MAXSERVICE = get_nc_dimension(&(filenames.dams), 
                                          "max_service");

    options.MAXDAMS = get_nc_dimension(&(filenames.dams), 
                                       "max_dams");

    // close routing parameter file
    status = nc_close(filenames.dams.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.dams.nc_filename);
}

void
dam_validate_domain(void)
{
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                     &(filenames.dams.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.dams.nc_filename);
    
    compare_ncdomain_with_global_domain(&filenames.dams);

    // close routing parameter file
    status = nc_close(filenames.dams.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.dams.nc_filename);
}