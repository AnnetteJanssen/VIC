#include <vic.h>

void
wu_set_nreceiving(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
        
    int *ivar;
    
    size_t i;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    // Get active irrigated vegetation    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    get_scatter_nc_field_int(&(filenames.water_use), 
            "nreceiving", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nreceiving = ivar[i];
    }
    
    free(ivar);
}

void
wu_set_nservice(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern wu_con_struct *wu_con;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t cur_ser;
    size_t i;
    size_t j;
    size_t k;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nservice = 0;
    }
    
    if(options.DAMS){
        for (i = 0; i < local_domain.ncells_active; i++) {
            for(j = 0; j < dam_con_map[i].nd_active; j++){
                for(k = 0; k < dam_con[i][j].nservice; k++){
                    cur_ser = dam_con[i][j].service[k];
                    
                    wu_con[cur_ser].nservice++;
                }
            }
        }
    }
}

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern filenames_struct filenames;
    extern size_t NF;
    extern wu_var_struct **wu_var;
    extern wu_hist_struct **wu_hist;
    extern wu_con_struct *wu_con;
    extern wu_force_struct **wu_force;
    extern int mpi_rank;
    
    int status;
    size_t i;
    size_t j;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                         &(filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.water_use.nc_filename);
    }
    
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var,"Memory allocation error");
    
    wu_hist = malloc(local_domain.ncells_active * sizeof(*wu_hist));
    check_alloc_status(wu_hist,"Memory allocation error");
    
    wu_force = malloc(local_domain.ncells_active * sizeof(*wu_force));
    check_alloc_status(wu_force,"Memory allocation error");

    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con,"Memory allocation error");
    
    wu_set_nreceiving();

    for(i=0; i<local_domain.ncells_active; i++){        
        wu_hist[i] = malloc(WU_NSECTORS * sizeof(*wu_hist[i]));
        check_alloc_status(wu_hist[i],"Memory allocation error");
        
        wu_var[i] = malloc(WU_NSECTORS * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i],"Memory allocation error");
        
        wu_force[i] = malloc(WU_NSECTORS * sizeof(*wu_force[i]));
        check_alloc_status(wu_force[i],"Memory allocation error");
        
        wu_con[i].receiving = malloc(wu_con[i].nreceiving * sizeof(*wu_con[i].receiving));
        check_alloc_status(wu_con[i].receiving,"Memory allocation error");

        for(j = 0; j < WU_NSECTORS; j++){        
            wu_force[i][j].consumption_fraction = malloc(NF * sizeof(*wu_force[i][j].consumption_fraction));
            check_alloc_status(wu_force[i][j].consumption_fraction,"Memory allocation error");
            
            wu_force[i][j].gw_fraction = malloc(NF * sizeof(*wu_force[i][j].gw_fraction));
            check_alloc_status(wu_force[i][j].gw_fraction,"Memory allocation error");
            
            wu_force[i][j].demand = malloc(NF * sizeof(*wu_force[i][j].demand));
            check_alloc_status(wu_force[i][j].demand,"Memory allocation error");
        }  
}

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    } 
}

void
wu_late_alloc(void)
{
    extern domain_struct local_domain;
    extern wu_con_struct *wu_con;
    size_t i;
    
    wu_set_nservice();

    for(i=0; i<local_domain.ncells_active; i++){         
        wu_con[i].service = malloc(wu_con[i].nservice * sizeof(*wu_con[i].service));
        check_alloc_status(wu_con[i].service,"Memory allocation error");

        wu_con[i].service_idx = malloc(wu_con[i].nservice * sizeof(*wu_con[i].service_idx));
        check_alloc_status(wu_con[i].service_idx,"Memory allocation error");
    }
}

void
wu_finalize(void)
{
    extern domain_struct local_domain;
    extern wu_var_struct **wu_var;
    extern wu_hist_struct **wu_hist;
    
    size_t i;
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < WU_NSECTORS; j++){    
            free(wu_force[i][j].consumption_fraction);
            free(wu_force[i][j].gw_fraction);
            free(wu_force[i][j].demand);
        }
        free(wu_force[i]);
        free(wu_hist[i]);
        free(wu_var[i]);
    	free(wu_con[i].receiving);
    	free(wu_con[i].service);
    	free(wu_con[i].service_idx);
    }
    free(wu_con);
    free(wu_force);
    free(wu_hist);
    free(wu_var);
}