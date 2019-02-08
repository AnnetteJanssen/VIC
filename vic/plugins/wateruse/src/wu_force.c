#include <vic_driver_image.h>
#include <plugin.h>

void
wu_forcing(void)
{
    extern size_t              current;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern dmy_struct         *dmy;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_force_struct **wu_force;
    extern size_t NF;
    extern int mpi_rank;
    
    int status;
    
    double *dvar;
    
    size_t  d4count[4];
    size_t  d4start[4];
    
    size_t i;
    size_t j;
    size_t k;
    int iSector;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
    
    // Open forcing file if it is the first time step
    if (current == 0 ) {
        if (mpi_rank == VIC_MPI_ROOT) {  
            // open new forcing file
            sprintf(plugin_filenames.wateruse_forcing.nc_filename, "%s%4d.nc",
                    plugin_filenames.wf_path_pfx, dmy[current].year);        
            status = nc_open(plugin_filenames.wateruse_forcing.nc_filename, NC_NOWRITE,
                             &(plugin_filenames.wateruse_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.wateruse_forcing.nc_filename);
        }
    }
    // Open forcing file if it is a new year
    else if (current > 0 && dmy[current].year != dmy[current - 1].year) {
        if (mpi_rank == VIC_MPI_ROOT) {            
            // close previous forcing file
            status = nc_close(plugin_filenames.wateruse_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.wateruse_forcing.nc_filename);

            // open new forcing file
            sprintf(plugin_filenames.wateruse_forcing.nc_filename, "%s%4d.nc",
                    plugin_filenames.wf_path_pfx, dmy[current].year);        
            status = nc_open(plugin_filenames.wateruse_forcing.nc_filename, NC_NOWRITE,
                             &(plugin_filenames.wateruse_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.wateruse_forcing.nc_filename);
        }
    }

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    // Get forcing data
    for (j = 0; j < NF; j++) {
        d4start[0] = global_param.forceskip[0] +
                     global_param.forceoffset[0] + j - NF;
        
        for(k = 0; k < plugin_options.NWUTYPES; k++){
            d4start[1] = k;
            
            get_scatter_nc_field_double(&(plugin_filenames.wateruse_forcing), 
                "demand", d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iSector = wu_con_map[i].sidx[k];
                
                if(iSector != NODATA_WU){
                    wu_force[i][iSector].demand[j] = dvar[i];
                }
            }
            
            get_scatter_nc_field_double(&(plugin_filenames.wateruse_forcing), 
                "groundwater_fraction", d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iSector = wu_con_map[i].sidx[k];
                
                if(iSector != NODATA_WU){
                    wu_force[i][iSector].groundwater_frac[j] = dvar[i];
                }
            }
            
            get_scatter_nc_field_double(&(plugin_filenames.wateruse_forcing), 
                "consumption_fraction", d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iSector = wu_con_map[i].sidx[k];
                
                if(iSector != NODATA_WU){
                    wu_force[i][iSector].consumption_frac[j] = dvar[i];
                }
            }
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(k = 0; k < plugin_options.NWUTYPES; k++){
            iSector = wu_con_map[i].sidx[k];
            
            if(iSector != NODATA_WU){
                wu_force[i][iSector].demand[NR] = average(wu_force[i][iSector].demand, NF);
                wu_force[i][iSector].groundwater_frac[NR] = average(wu_force[i][iSector].groundwater_frac, NF);
                wu_force[i][iSector].consumption_frac[NR] = average(wu_force[i][iSector].consumption_frac, NF);
            }
        }
    }   

    // Close forcing file if it is the last time step
    if (current == global_param.nrecs - 1) {
        if (mpi_rank == VIC_MPI_ROOT) {         
            // close previous forcing file
            status = nc_close(plugin_filenames.wateruse_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.wateruse_forcing.nc_filename);
        }
    }
    
    free(dvar);
}