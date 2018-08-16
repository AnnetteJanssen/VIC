#include <vic.h>

void
dam_set_nd_active(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern filenames_struct    filenames;
    extern dam_con_map_struct *dam_con_map;

    int                       *ivar;

    size_t                     i;

    size_t                     d2count[2];
    size_t                     d2start[2];

    // Get active dams
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    get_scatter_nc_field_int(&(filenames.dams),
                             "Ndam", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con_map[i].nd_active = ivar[i];
    }
    
    free(ivar);
}

void
dam_set_nservice(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern dam_con_map_struct *dam_con_map;
    extern option_struct options;
        
    int *ivar;
    
    size_t i;
    size_t j;
    
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

    for(j = 0; j < (size_t)options.MAXDAMS; j++){
        d3start[0] = j;

        get_scatter_nc_field_int(&(filenames.dams), 
                "Nservice", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if(j < dam_con_map[i].nd_active){
                dam_con[i][j].nservice = ivar[i];
            }
        }
    }

    free(ivar);
}

void
dam_alloc(void)
{
    extern domain_struct       local_domain;
    extern filenames_struct    filenames;
    extern dam_var_struct    **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;
    extern int                 mpi_rank;

    int                        status;

    size_t                     i;
    size_t                     j;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                         &(filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.dams.nc_filename);
    }

    // Allocate cells
    dam_con_map = malloc(local_domain.ncells_active * sizeof(*dam_con_map));
    check_alloc_status(dam_con_map, "Memory allocation error");
    dam_con = malloc(local_domain.ncells_active * sizeof(*dam_con));
    check_alloc_status(dam_con, "Memory allocation error");
    dam_var = malloc(local_domain.ncells_active * sizeof(*dam_var));
    check_alloc_status(dam_var, "Memory allocation error");

    dam_set_nd_active();

    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con[i] = malloc(dam_con_map[i].nd_active * sizeof(*dam_con[i]));
        check_alloc_status(dam_con[i], "Memory allocation error");
        dam_var[i] = malloc(dam_con_map[i].nd_active * sizeof(*dam_var[i]));
        check_alloc_status(dam_var[i], "Memory allocation error"); 
    }   
    
    dam_set_nservice();
    
    for(i=0; i<local_domain.ncells_active; i++){        
        for(j=0; j < dam_con_map[i].nd_active; j++){
            dam_con[i][j].service = malloc(dam_con[i][j].nservice * sizeof(*dam_con[i][j].service));
            check_alloc_status(dam_con[i][j].service,"Memory allocation error");
            dam_con[i][j].serve_factor = malloc(dam_con[i][j].nservice * sizeof(*dam_con[i][j].serve_factor));
            check_alloc_status(dam_con[i][j].serve_factor,"Memory allocation error");
        }
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
    extern dam_var_struct    **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;

    size_t                     i;
    size_t                     j;
            
    for(i=0; i < local_domain.ncells_active; i++){
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            free(dam_con[i][j].service);
        }
        free(dam_var[i]);
        free(dam_con[i]);
    }
    free(dam_con_map);
    free(dam_con);
    free(dam_var);
}