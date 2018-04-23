#include <vic.h>

void
wu_set_receiving(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_con_struct *wu_con;
    
    int *link_id;
    int *ivar;
    int *adjustment;
    bool done;
    char                       locstr[MAXSTRING];
    
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
    
    link_id = malloc(local_domain.ncells_active * sizeof(*link_id));
    check_alloc_status(link_id, "Memory allocation error.");     
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error."); 
    
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = 0;
    }
    
    get_scatter_nc_field_int(&(filenames.water_use), 
            "cellID", d2start, d2count, link_id);
    
    for(j = 0; j < options.MAXRECEIVING; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(filenames.water_use), 
                "receiving", d3start, d3count, ivar);

        for(i = 0; i < local_domain.ncells_active; i++){
            if(j < wu_con[i].nreceiving){
                
                done = false;
                for(k = 0; k < local_domain.ncells_active; k++){
                    if(link_id[k] == ivar[i]){
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
    
    free(link_id);
    free(ivar);
    free(adjustment);
}

void
wu_set_service(void)
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
    size_t l;
    
    if(options.DAMS){
        for (i = 0; i < local_domain.ncells_active; i++) {
            for(j = 0; j < dam_con_map[i].nd_active; j++){
                for(k = 0; k < dam_con[i][j].nservice; k++){
                    cur_ser = dam_con[i][j].service[k];
                    
                    for(l = 0; l < wu_con[cur_ser].nservice; l++){
                        if(wu_con[cur_ser].service[l] == MISSING_USI){
                            wu_con[cur_ser].service[l] = i;
                            wu_con[cur_ser].service_idx[l] = j;
                            break;
                        }
                    }
                }
            }
        }
    }
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
    
    wu_set_receiving();
    wu_set_service();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
}