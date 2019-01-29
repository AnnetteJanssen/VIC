#include <vic.h>

void
dam_set_info(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern filenames_struct    filenames;
    extern dam_con_struct     *local_dam_con;
    extern dam_con_struct     *global_dam_con;

    int                       *ivar;
    double                    *dvar;

    size_t                     i;

    size_t                     d2count[2];
    size_t                     d2start[2];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    get_scatter_nc_field_int(&(filenames.dams), 
                             "run_local", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i].run = ivar[i];
    }
    
    get_scatter_nc_field_int(&(filenames.dams), 
                             "year_local", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i].year = ivar[i];
    }

    get_scatter_nc_field_double(&(filenames.dams),
                                "capacity_local", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i].capacity = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.dams),
                                "inflow_frac_local", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i].inflow_frac = dvar[i];
    }
    
    
    get_scatter_nc_field_int(&(filenames.dams), 
                             "run_global", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con[i].run = ivar[i];
    }
    
    get_scatter_nc_field_int(&(filenames.dams), 
                             "year_global", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con[i].year = ivar[i];
    }

    get_scatter_nc_field_double(&(filenames.dams),
                                "capacity_global", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con[i].capacity = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.dams),
                                "inflow_frac_global", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con[i].inflow_frac = dvar[i];
    }
    
    free(ivar);
    free(dvar);
}

void
dam_set_service(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern option_struct options;
    extern dam_con_struct     *local_dam_con;
    extern dam_con_struct     *global_dam_con;
    
    int *service_id;
    int *ivar;
    double *dvar;
    int *adjustment;
    size_t                  service_count;
    bool done;
    
    size_t i;
    size_t j;
    size_t k;
    
    size_t  d2count[2];
    size_t  d2start[2];
    size_t  d3count[3];
    size_t  d3start[3];
        
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");   
    service_id = malloc(local_domain.ncells_active * sizeof(*service_id));
    check_alloc_status(service_id, "Memory allocation error."); 
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error.");
    
    get_scatter_nc_field_int(&(filenames.dams), 
            "service_id", d2start, d2count, service_id);
            
    service_count = 0;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = 0;
    }
    for(j = 0; j < (size_t)options.NDAMSERVICE; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_int(&(filenames.dams), 
                "service_local", d3start, d3count, ivar);
        get_scatter_nc_field_double(&(filenames.dams), 
                "serve_factor_local", d3start, d3count, dvar);

        for(i = 0; i < local_domain.ncells_active; i++){
            if(j < local_dam_con[i].nservice){

                done = false;
                for(k = 0; k < local_domain.ncells_active; k++){
                    if(ivar[i] == service_id[k]){
                        local_dam_con[i].service[j - adjustment[i]] = k;
                        local_dam_con[i].service_frac[j - adjustment[i]] = dvar[i];
                        done = true;
                    }
                }

                if(!done){
                    service_count++;
                    local_dam_con[i].nservice--;
                    adjustment[i]++;
                }
            }
        }
    }
    
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = 0;
    }
    for(j = 0; j < (size_t)options.NDAMSERVICE; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_int(&(filenames.dams), 
                "service_global", d3start, d3count, ivar);
        get_scatter_nc_field_double(&(filenames.dams), 
                "serve_factor_global", d3start, d3count, dvar);

        for(i = 0; i < local_domain.ncells_active; i++){
            if(j < global_dam_con[i].nservice){

                done = false;
                for(k = 0; k < local_domain.ncells_active; k++){
                    if(ivar[i] == service_id[k]){
                        global_dam_con[i].service[j - adjustment[i]] = k;
                        global_dam_con[i].service_frac[j - adjustment[i]] = dvar[i];
                        done = true;
                    }
                }

                if(!done){
                    service_count++;
                    global_dam_con[i].nservice--;
                    adjustment[i]++;
                }
            }
        }
    }
    
    if(service_count > 0){
        log_err("Dams service cell id not found for %zu cells; "
                "Probably the ID was outside of the mask or "
                "the ID was not set; "
                "Removing from dam service", service_count);
    }
    
    free(ivar);
    free(dvar);
    free(service_id);
    free(adjustment);
}

void
dam_init(void)
{    
    extern filenames_struct filenames;
    extern int 		    mpi_rank;
    
    int 		    status;
    
    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                         &(filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.dams.nc_filename);
    }

    dam_set_info();
    dam_set_service();
    
    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.dams.nc_filename);
    }
}