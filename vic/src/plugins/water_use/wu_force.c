#include <vic.h>

void
wu_force(void)
{
    extern size_t              current;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern dmy_struct         *dmy;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_con_struct **wu_con;
    extern size_t NF;
    extern int mpi_rank;
    
    int status;
    
    double *dvar;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    size_t f;
    size_t i;
    size_t j;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
    
    for(f = 0; f < WU_NSECTORS; f++){
        if(options.WU_INPUT_LOCATION[f] == WU_INPUT_FROM_FILE){
            if (current == 0 ) {
                if (mpi_rank == VIC_MPI_ROOT) {  
                    // open new forcing file
                    sprintf(filenames.water_use[f].nc_filename, "%s%4d.nc",
                            filenames.water_use_forcing_pfx[f], dmy[current].year);        
                    status = nc_open(filenames.water_use[f].nc_filename, NC_NOWRITE,
                                     &(filenames.water_use[f].nc_id));
                    check_nc_status(status, "Error opening %s",
                                    filenames.water_use[f].nc_filename);
                }
            }else if (dmy[current].year != dmy[current - 1].year) {
                // reset offset
                options.wu_force_offset = 0;

                if (mpi_rank == VIC_MPI_ROOT) {            
                    // close previous forcing file
                    status = nc_close(filenames.water_use[f].nc_id);
                    check_nc_status(status, "Error closing %s",
                                    filenames.water_use[f].nc_filename);

                    // open new forcing file
                    sprintf(filenames.water_use[f].nc_filename, "%s%4d.nc",
                            filenames.water_use_forcing_pfx[f], dmy[current].year);        
                    status = nc_open(filenames.water_use[f].nc_filename, NC_NOWRITE,
                                     &(filenames.water_use[f].nc_id));
                    check_nc_status(status, "Error opening %s",
                                    filenames.water_use[f].nc_filename);
                }
            }
      
            d3start[1] = 0;
            d3start[2] = 0;
            d3count[0] = 1;
            d3count[1] = global_domain.n_ny;
            d3count[2] = global_domain.n_nx;

            for (j = 0; j < NF; j++) {
                d3start[0] = global_param.forceskip[1] +
                             global_param.forceoffset[1] + j;

                get_scatter_nc_field_double(&(filenames.water_use[f]), 
                    "consumption_fraction", d3start, d3count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    wu_con[i][j].consumption_fraction = dvar[i];
                }

                get_scatter_nc_field_double(&(filenames.water_use[f]), 
                    "demand", d3start, d3count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    wu_con[i][j].demand = dvar[i];
                }
            }
        }

        options.wu_force_offset += NF;
    }
    
    free(dvar);
}