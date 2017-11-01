#include <ext_driver_shared_image.h>

void
ext_start(){
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    extern size_t *cell_order;
    extern int mpi_decomposition;
    extern MPI_Comm            MPI_COMM_VIC;
    extern int mpi_rank;
    
    int status;
    size_t i;
    
    if(mpi_rank == VIC_MPI_ROOT){      
            validate_ext_parameters();   
        if(ext_options.ROUTING){        
            // open extension routing file
            status = nc_open(ext_filenames.routing.nc_filename, NC_NOWRITE,
                             &(ext_filenames.routing.nc_id));
            check_nc_status(status, "Error opening %s",
                            ext_filenames.routing.nc_filename);
            
            // Calculate derived option variables
            ext_options.uh_steps = global_param.model_steps_per_day * ext_param.UH_LENGTH;
            
            // Allocate global variables
            if(mpi_decomposition == RANDOM_DECOMPOSITION){
                cell_order = malloc(global_domain.ncells_active * sizeof(*cell_order));
                check_alloc_status(cell_order, "Memory allocation error");
                
                for(i=0;i<global_domain.ncells_active;i++){
                    cell_order[i] = MISSING_USI;
                }
            }
        }
        if(ext_options.DAMS){         
            // open extension dam file
            status = nc_open(ext_filenames.dams.nc_filename, NC_NOWRITE,
                             &(ext_filenames.dams.nc_id));
            check_nc_status(status, "Error opening %s",
                            ext_filenames.dams.nc_filename);
            
            // calculate derived option variables
            ext_options.model_steps_per_history_step = ext_param.DAM_HISTORY_LENGTH * global_param.model_steps_per_day;
            ext_options.history_steps_per_history_year = ceil((float)DAYS_PER_LYEAR / (float)ext_param.DAM_HISTORY_LENGTH);
            ext_options.history_steps = ext_options.history_steps_per_history_year * ext_param.DAM_HISTORY;
            ext_options.ndams = get_nc_dimension(&ext_filenames.dams, "dams");
        }
    }  
    
    status = MPI_Bcast(&ext_param, 1, mpi_ext_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&ext_options, 1, mpi_ext_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&mpi_decomposition, 1, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
            
    if(ext_options.ROUTING){
        debug_basins();
        debug_node_domain();
    }
}