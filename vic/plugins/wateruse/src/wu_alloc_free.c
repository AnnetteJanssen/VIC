#include <vic_driver_image.h>
#include <plugin.h>

void
wu_set_nsectors(void)
{
    extern domain_struct local_domain;
    extern plugin_option_struct       plugin_options;
    extern wu_con_map_struct  *wu_con_map;
        
    int *ivar;
    
    size_t i;
    size_t j;
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con_map[i].ns_types = WU_NSECTORS;
        wu_con_map[i].ns_active = 0;
        
        for(j = 0; j < WU_NSECTORS; j++){
            if(plugin_options.WU_INPUT[j] == WU_SKIP){
                wu_con_map[i].sidx[j] = NODATA_WU;
            } else {
                wu_con_map[i].sidx[j] = wu_con_map[i].ns_active;
                wu_con_map[i].ns_active++;
            }
        }
    }
    
    free(ivar);
}

void
wu_set_nreceiving(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_con_map_struct  *wu_con_map;
    extern wu_con_struct  **wu_con;
    extern int mpi_rank;
        
    int *ivar;
    
    size_t i;
    size_t j;
    int iSector;
    int status;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wateruse.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wateruse.nc_filename);
    }
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    for(j = 0; j < WU_NSECTORS; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(plugin_filenames.wateruse),
                                    "nreceiving", d3start, d3count, ivar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            iSector = wu_con_map[i].sidx[j];
            if(iSector != NODATA_WU){
                wu_con[i][iSector].nreceiving = ivar[i];
            }
        } 
    }
    
    
    free(ivar);

    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wateruse.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wateruse.nc_filename);
    }
}

void
wu_alloc(void)
{
    extern domain_struct    local_domain;
    extern size_t           NF;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    extern wu_force_struct **wu_force;
    
    size_t                     i;
    size_t                     j;

    wu_con_map = malloc(local_domain.ncells_active * sizeof(*wu_con_map));
    check_alloc_status(wu_con_map, "Memory allocation error");
    wu_force = malloc(local_domain.ncells_active * sizeof(*wu_force));
    check_alloc_status(wu_force, "Memory allocation error");
    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con, "Memory allocation error");
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con_map[i].sidx = malloc(WU_NSECTORS * sizeof(*wu_con_map[i].sidx));
        check_alloc_status(wu_con_map[i].sidx, "Memory allocation error");
    }
    
    wu_set_nsectors();

    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_force[i] = malloc(wu_con_map[i].ns_active * sizeof(*wu_force[i]));
        check_alloc_status(wu_force[i], "Memory allocation error");
        wu_con[i] = malloc(wu_con_map[i].ns_active * sizeof(*wu_con[i]));
        check_alloc_status(wu_con[i], "Memory allocation error");
        wu_var[i] = malloc(wu_con_map[i].ns_active * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i], "Memory allocation error");
        
        for (j = 0; j < wu_con_map[i].ns_active; j++) {
            wu_force[i][j].consumption_frac = malloc(NF * sizeof(*wu_force[i][j].consumption_frac));
            check_alloc_status(wu_force[i][j].consumption_frac, "Memory allocation error");
            wu_force[i][j].groundwater_frac = malloc(NF * sizeof(*wu_force[i][j].groundwater_frac));
            check_alloc_status(wu_force[i][j].groundwater_frac, "Memory allocation error");
            wu_force[i][j].demand = malloc(NF * sizeof(*wu_force[i][j].demand));
            check_alloc_status(wu_force[i][j].demand, "Memory allocation error");
        }
    }
    
    wu_set_nreceiving();
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < wu_con_map[i].ns_active; j++) {
            wu_con[i][j].receiving = malloc(wu_con[i][j].nreceiving * sizeof(*wu_con[i][j].receiving));
            check_alloc_status(wu_con[i][j].receiving, "Memory allocation error");
        }
    }
    
    wu_initialize_local_structures();
}

void
wu_finalize(void)
{
    extern domain_struct    local_domain;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    extern wu_force_struct **wu_force;

    size_t                  i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < wu_con_map[i].ns_active; j++) {
            free(wu_force[i][j].groundwater_frac);
            free(wu_force[i][j].consumption_frac);
            free(wu_force[i][j].demand);
        }
        for (j = 0; j < wu_con_map[i].ns_active; j++) {
            free(wu_con[i][j].receiving);
        }
        
        free(wu_con_map[i].sidx);
        free(wu_con[i]);
        free(wu_var[i]);
        free(wu_force[i]);
    }
    
    free(wu_con_map);
    free(wu_con);
    free(wu_var);
    free(wu_force);
}
