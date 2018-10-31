#include <vic.h>

void
efr_forcing(void)
{
    extern size_t              current;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern dmy_struct         *dmy;
    extern filenames_struct filenames;
    extern efr_force_struct *efr_force;
    extern efr_hist_struct *efr_hist;
    extern size_t NF;
    extern int mpi_rank;
    
    int status;
    
    double *dvar;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    size_t i;
    size_t j;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
    
    // Open forcing file if it is the first time step
    if (current == 0 ) {
        if (mpi_rank == VIC_MPI_ROOT) {  
            // open new forcing file
            sprintf(filenames.efr_forcing.nc_filename, "%s%4d.nc",
                    filenames.efr_forcing_pfx, dmy[current].year);        
            status = nc_open(filenames.efr_forcing.nc_filename, NC_NOWRITE,
                             &(filenames.efr_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.efr_forcing.nc_filename);
        }
    // Open forcing file if it is a new year
    }
    else if (current > 0 && dmy[current].year != dmy[current - 1].year) {
        if (mpi_rank == VIC_MPI_ROOT) {            
            // close previous forcing file
            status = nc_close(filenames.efr_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            filenames.efr_forcing.nc_filename);

            // open new forcing file
            sprintf(filenames.efr_forcing.nc_filename, "%s%4d.nc",
                    filenames.efr_forcing_pfx, dmy[current].year);        
            status = nc_open(filenames.efr_forcing.nc_filename, NC_NOWRITE,
                             &(filenames.efr_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.efr_forcing.nc_filename);
        }
    }

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[0] +
                     global_param.forceoffset[0] + j - 1;

        get_scatter_nc_field_double(&(filenames.efr_forcing), 
            "discharge", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            efr_force[i].requirement_discharge[j] = dvar[i];
        }

        get_scatter_nc_field_double(&(filenames.efr_forcing), 
            "baseflow", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            efr_force[i].requirement_baseflow[j] = dvar[i];
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        efr_hist[i].requirement_discharge = 
                average(efr_force[i].requirement_discharge, NF);
    }       
    for (i = 0; i < local_domain.ncells_active; i++) {
        efr_hist[i].requirement_baseflow = 
                average(efr_force[i].requirement_baseflow, NF);
    }       

    // Close forcing file if it is the last time step
    if (current == global_param.nrecs - 1) {
        if (mpi_rank == VIC_MPI_ROOT) {         
            // close previous forcing file
            status = nc_close(filenames.efr_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            filenames.efr_forcing.nc_filename);
        }
    }
    
    free(dvar);
}