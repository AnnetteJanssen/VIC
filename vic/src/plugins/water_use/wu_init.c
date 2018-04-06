#include <vic.h>

void
wu_set_link_id(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
    
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
    
    get_scatter_nc_field_int(&(filenames.water_use), 
            "cellID", d2start, d2count, ivar);

    for(i = 0; i < local_domain.ncells_active; i++){
        wu_con[i].link_id = ivar[i];
    }
    
    free(ivar);
    
}

void
wu_set_receiving(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
    
    int *ivar;
    int *adjustment;
    bool done;
    char                       locstr[MAXSTRING];
    
    size_t i;
    int j;
    size_t k;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error."); 
    
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = 0;
    }
    
    for(j = 0; j < options.MAXRECEIVING; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(filenames.water_use), 
                "receiving", d3start, d3count, ivar);

        for(i = 0; i < local_domain.ncells_active; i++){
            if(j < wu_con[i].nreceiving){
                
                done = false;
                for(k = 0; k < local_domain.ncells_active; k++){
                    if(wu_con[k].link_id == (size_t)ivar[i]){
                        wu_con[i].receiving[j - adjustment[i]] = k;
                        done = true;
                        break;
                    }
                }
                
                if(!done){
                    sprint_location(locstr, &(local_domain.locations[i]));
                    log_warn("Receiving water use link_id %d not found. Removing...\n%s", ivar[i],locstr);
                    wu_con[i].nreceiving--;
                    adjustment[i]++;
                }
            }
        }
    }
    
    free(ivar);
}

void
wu_set_sending(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
    
    int *ivar;
    bool done;
    int *adjustment;
    char                       locstr[MAXSTRING];
    
    size_t i;
    int j;
    size_t k;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error."); 
    
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = 0;
    }
    
    for(j = 0; j < options.MAXSENDING; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(filenames.water_use), 
                "sending", d3start, d3count, ivar);

        for(i = 0; i < local_domain.ncells_active; i++){
            if(j < wu_con[i].nsending){
                
                done = false;
                for(k = 0; k < local_domain.ncells_active; k++){
                    if(wu_con[k].link_id == (size_t)ivar[i]){
                        wu_con[i].sending[j - adjustment[i]] = k;
                        done = true;
                        break;
                    }
                }
                
                if(!done){
                    sprint_location(locstr, &(local_domain.locations[i]));
                    log_warn("Sending water use link_id %d not found. Removing...\n%s", ivar[i],locstr);
                    wu_con[i].nsending--;
                    adjustment[i]++;
                }
            }
        }
    }
    
    free(ivar);
}

void
wu_init(void)
{
    extern filenames_struct filenames;
    extern int mpi_rank;
    
    int status;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                         &(filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.water_use.nc_filename);
    }
    
    wu_set_link_id();
    wu_set_receiving();
    wu_set_sending();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
}