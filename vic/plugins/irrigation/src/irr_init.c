#include <vic_driver_image.h>
#include <plugin.h>

void
irr_set_mapping(void)
{
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern veg_con_struct           **veg_con;
    extern irr_con_struct           **irr_con;
    extern irr_con_map_struct        *irr_con_map;

    size_t                            i;
    size_t                            j;
    int                            irr_index;
    int                            veg_index;
    size_t                            veg_class;
    
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NIRRTYPES; j++){
            irr_index = irr_con_map[i].iidx[j];
            veg_index = irr_con_map[i].vidx[j];
            
            if (irr_index != NODATA_VEG) {
                veg_class = veg_con[i][veg_index].veg_class;
                
                irr_con[i][irr_index].irr_class = j;
                irr_con[i][irr_index].veg_index = veg_index;
                irr_con[i][irr_index].veg_class = veg_class;
            }
        }
    }
}

void
irr_set_paddy(void)
{
    extern domain_struct local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct       plugin_options;
    extern irr_con_map_struct  *irr_con_map;
    extern int mpi_rank;
    extern MPI_Comm                MPI_COMM_VIC;
    
    int *ivar;
    
    size_t i;
    size_t j;
    int    status;
    
    size_t  d1count[1];
    size_t  d1start[1];
    
    d1start[0] = 0;
    d1count[0] = plugin_options.NIRRTYPES;
    
    ivar = malloc(plugin_options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    if(mpi_rank == VIC_MPI_ROOT){
        get_nc_field_int(&(plugin_filenames.irrigation), 
                "paddy", d1start, d1count, ivar);
    }
    
    status = MPI_Bcast(ivar, plugin_options.NIRRTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            if(ivar[irr_con[i][j].irr_class] == 1){
                irr_con[i][j].paddy = true;
            }
        }
    }
    
    free(ivar);
}

void
irr_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.irrigation.nc_filename);
    }

    irr_set_mapping();
    irr_set_paddy();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.irrigation.nc_filename);
    }
}
