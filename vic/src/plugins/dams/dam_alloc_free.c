#include <vic.h>

void
dam_set_nservice(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern dam_con_struct *local_dam_con;
    extern dam_con_struct *global_dam_con;
        
    int *ivar;
    
    size_t i;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    get_scatter_nc_field_int(&(filenames.dams), 
            "Nservice_local", d2start, d2count, ivar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i].nservice = ivar[i];
    }

    get_scatter_nc_field_int(&(filenames.dams), 
            "Nservice_global", d2start, d2count, ivar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con[i].nservice = ivar[i];
    }

    free(ivar);
}

void
dam_alloc(void)
{
    extern domain_struct       local_domain;
    extern filenames_struct    filenames;
    extern dam_con_struct     *local_dam_con;
    extern dam_var_struct     *local_dam_var;
    extern dam_con_struct     *global_dam_con;
    extern dam_var_struct     *global_dam_var;
    extern int                 mpi_rank;

    int                        status;

    size_t                     i;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                         &(filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.dams.nc_filename);
    }

    local_dam_con = malloc(local_domain.ncells_active * sizeof(*local_dam_con));
    check_alloc_status(local_dam_con, "Memory allocation error");
    local_dam_var = malloc(local_domain.ncells_active * sizeof(*local_dam_var));
    check_alloc_status(local_dam_var, "Memory allocation error");

    global_dam_con = malloc(local_domain.ncells_active * sizeof(*global_dam_con));
    check_alloc_status(global_dam_con, "Memory allocation error");
    global_dam_var = malloc(local_domain.ncells_active * sizeof(*global_dam_var));
    check_alloc_status(global_dam_var, "Memory allocation error");
    
    dam_set_nservice();
    
    for(i=0; i<local_domain.ncells_active; i++){
        local_dam_con[i].service = malloc(local_dam_con[i].nservice * sizeof(*local_dam_con[i].service));
        check_alloc_status(local_dam_con[i].service,"Memory allocation error");
        local_dam_con[i].service_frac = malloc(local_dam_con[i].nservice * sizeof(*local_dam_con[i].service_frac));
        check_alloc_status(local_dam_con[i].service_frac,"Memory allocation error");
        
        global_dam_con[i].service = malloc(global_dam_con[i].nservice * sizeof(*global_dam_con[i].service));
        check_alloc_status(local_dam_con[i].service,"Memory allocation error");
        global_dam_con[i].service_frac = malloc(global_dam_con[i].nservice * sizeof(*global_dam_con[i].service_frac));
        check_alloc_status(global_dam_con[i].service_frac,"Memory allocation error");
    }
    
    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.dams.nc_filename);
    }
}

void
dam_finalize(void)
{
    extern domain_struct       local_domain;
    extern dam_con_struct     *local_dam_con;
    extern dam_var_struct     *local_dam_var;
    extern dam_con_struct     *global_dam_con;
    extern dam_var_struct     *global_dam_var;

    size_t                     i;
            
    for(i=0; i < local_domain.ncells_active; i++){
        free(local_dam_con[i].service);
        free(local_dam_con[i].service_frac);
        
        free(global_dam_con[i].service);
        free(global_dam_con[i].service_frac);
    }
    
    free(local_dam_con);
    free(local_dam_var);
    
    free(global_dam_con);
    free(global_dam_var);
}