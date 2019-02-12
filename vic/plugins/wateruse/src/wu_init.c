#include <vic_driver_image.h>
#include <plugin.h>

void
wu_set_info(void)
{
    extern domain_struct    local_domain;
    extern wu_con_map_struct *wu_con_map;
    extern wu_con_struct **wu_con;

    size_t                  i;
    size_t                  j;
    int                     iSector;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NWUTYPES; j++){
            iSector = wu_con_map[i].sidx[j];

            if(iSector != NODATA_WU){
                wu_con[i][iSector].sector_type_num = plugin_options.NWUTYPES;
                wu_con[i][iSector].wu_sector = j;
            }
        }
    }
}

void
wu_set_receiving(void)
{
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_con_map_struct *wu_con_map;
    extern wu_con_struct **wu_con;

    int                    *id;
    int                    *receiving;
    int                   **adjustment;
    size_t                  error_count;
    
    bool                    found;

    size_t                  i;
    size_t                  j;
    size_t                  k;
    size_t                  l;
    int                     iSector;

    size_t                  d2count[2];
    size_t                  d2start[2];
    size_t                  d4count[4];
    size_t                  d4start[4];

    receiving = malloc(local_domain.ncells_active * sizeof(*receiving));
    check_alloc_status(receiving, "Memory allocation error.");
    id = malloc(local_domain.ncells_active * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error.");
    for(i = 0; i < local_domain.ncells_active; i++){
        adjustment[i] = malloc( plugin_options.NWUTYPES * sizeof(*adjustment[i]));
        check_alloc_status(adjustment[i], "Memory allocation error.");
    }

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

    get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving_id", 
                             d2start, d2count, id);
    
    error_count = 0;
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j <  plugin_options.NWUTYPES; j++){
            adjustment[i][j] = 0;
        }
    }
    for(k = 0; k < plugin_options.NWURECEIVING; k++){
        d4start[0] = k;
        
        for(j = 0; j < plugin_options.NWUTYPES; j++){
            d4start[1] = j;
            
            get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving", 
                                     d4start, d4count, receiving);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iSector = wu_con_map[i].sidx[j];
                
                if(iSector != NODATA_WU && k < wu_con[i][iSector].nreceiving){
                    found = false;
                    
                    for (l = 0; l < local_domain.ncells_active; l++) {
                        if (receiving[i] == id[j]) {
                            wu_con[i][iSector].receiving[k - adjustment[i][j]] = l;
                            found = true;
                        }
                    }

                    if(!found){
                        error_count++;
                        wu_con[i][iSector].nreceiving--;
                        adjustment[i][j]++;
                    }
                }
            }
        }
    }
    
    if(error_count > 0){
        log_warn("No receiving cell was found for %zu cells; "
                "Probably the ID was outside of the mask or "
                "the ID was not set; "
                "Removing receiving cells",
                error_count);
    }

    for(i = 0; i < local_domain.ncells_active; i++){
        free(adjustment[i]);
    }
    free(adjustment);
    free(receiving);
    free(id);
}

void
wu_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int              mpi_rank;

    int                     status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wateruse.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wateruse.nc_filename);
    }
    
    wu_set_info();
    wu_set_receiving();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wateruse.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wateruse.nc_filename);
    }
}