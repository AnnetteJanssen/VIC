#include <vic.h>

void
wu_set_nreceiving(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
    extern int mpi_rank;
    
    int status;
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
        
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                         &(filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.water_use.nc_filename);
    }

    get_scatter_nc_field_int(&(filenames.water_use), 
            "nreceiving", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nreceiving = ivar[i];
    }

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
    
    free(ivar);
}

void
wu_set_nservice(void)
{
    extern domain_struct local_domain;
    extern wu_con_struct *wu_con;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t cur_ser;
    size_t i;
    size_t j;
    size_t k;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            for(k = 0; k < dam_con[i][j].nservice; k++){
                cur_ser = dam_con[i][j].service[k];

                wu_con[cur_ser].nservice++;
            }
        }
    }
}

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern size_t NF;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_force_struct **wu_force;
    
    size_t i;
    size_t j;
    
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var,"Memory allocation error");
    
    wu_force = malloc(local_domain.ncells_active * sizeof(*wu_force));
    check_alloc_status(wu_force,"Memory allocation error");

    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con,"Memory allocation error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nreceiving = 0;
    }
    
    if(options.WU_REMOTE){
        wu_set_nreceiving();
    }
    
    for(i=0; i<local_domain.ncells_active; i++){
        wu_var[i] = malloc(WU_NSECTORS * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i],"Memory allocation error");
        
        wu_force[i] = malloc(WU_NSECTORS * sizeof(*wu_force[i]));
        check_alloc_status(wu_force[i],"Memory allocation error");
        
        wu_con[i].receiving = malloc(wu_con[i].nreceiving * sizeof(*wu_con[i].receiving));
        check_alloc_status(wu_con[i].receiving,"Memory allocation error");

        for(j = 0; j < WU_NSECTORS; j++){
            wu_force[i][j].demand = malloc(NF * sizeof(*wu_force[i][j].demand));
            check_alloc_status(wu_force[i][j].demand,"Memory allocation error");
        }  
    }
}

void
wu_late_alloc(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern wu_con_struct *wu_con;
    size_t i;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nservice = 0;
    }
    
    if(options.DAMS){
        wu_set_nservice();
    }

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
    
    size_t i;
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < WU_NSECTORS; j++){
            free(wu_force[i][j].demand);
        }
        free(wu_force[i]);
        free(wu_var[i]);
    	free(wu_con[i].receiving);
    	free(wu_con[i].service);
    	free(wu_con[i].service_idx);
    }
    free(wu_con);
    free(wu_force);
    free(wu_var);
}