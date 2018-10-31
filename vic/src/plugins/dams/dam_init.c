#include <vic.h>

void
dam_set_info(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern filenames_struct    filenames;
    extern option_struct       options;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;

    int                       *ivar;
    double                    *dvar;

    size_t                     i;
    size_t                     j;

    size_t                     d3count[3];
    size_t                     d3start[3];

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

    for (j = 0; j < (size_t)options.MAXDAMS; j++) {
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(filenames.dams), 
                "id", d3start, d3count, ivar);
        
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].id = ivar[i];
            }
        }
        
        get_scatter_nc_field_int(&(filenames.dams), 
                		 "year", d3start, d3count, ivar);
        
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].year = ivar[i];
            }
        }

        get_scatter_nc_field_double(&(filenames.dams),
                                    "volume", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].max_volume = dvar[i] * pow(M_PER_KM, 2);
            }
        }

        get_scatter_nc_field_int(&(filenames.dams),
                                 "type", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].function = ivar[i];
            }
        }
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
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    int *service_var;
    int *ivar;
    double *dvar;
    int **adjustment;
    size_t                  service_count;
    bool done;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    size_t  d2count[2];
    size_t  d2start[2];
    size_t  d4count[4];
    size_t  d4start[4];
        
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");   
    service_var = malloc(local_domain.ncells_active * sizeof(*service_var));
    check_alloc_status(service_var, "Memory allocation error."); 
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error."); 
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = malloc(options.MAXDAMS * sizeof(*adjustment[i]));
        check_alloc_status(adjustment[i], "Memory allocation error.");
    }
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < (size_t)options.MAXDAMS; j++){
            adjustment[i][j] = 0;
        }
    }
    
    get_scatter_nc_field_int(&(filenames.dams), 
            "service_id", d2start, d2count, service_var);
            
    service_count = 0;
    for(k = 0; k < (size_t)options.MAXSERVICE; k++){
        d4start[0] = k;
        
        for(j = 0; j < (size_t)options.MAXDAMS; j++){
            d4start[1] = j;
                        
            get_scatter_nc_field_int(&(filenames.dams), 
                    "service", d4start, d4count, ivar);
            get_scatter_nc_field_double(&(filenames.dams), 
                    "serve_factor", d4start, d4count, dvar);
            
            for(i = 0; i < local_domain.ncells_active; i++){
                if(j < dam_con_map[i].nd_active){
                    if(k < dam_con[i][j].nservice){
                        
                        done = false;
                        for(l = 0; l < local_domain.ncells_active; l++){
                            if(ivar[i] == service_var[l]){
                                dam_con[i][j].service[k - adjustment[i][j]] = l;
                                done = true;
                                break;
                            }
                        }
                        dam_con[i][j].serve_factor[k - adjustment[i][j]] = 
                                dvar[i];
                                
                        if(!done){
                            service_count++;
                            dam_con[i][j].nservice--;
                            adjustment[i][j]++;
                        }
                    }
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
    
    for(i = 0; i < local_domain.ncells_active; i++){
        free(adjustment[i]);
    }
    free(adjustment);
    free(ivar);
    free(service_var);
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