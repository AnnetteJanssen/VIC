#include <ext_driver_shared_image.h>

/******************************************************************************
 * @brief   Gather double precision variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_double(double *dvar, double *var_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    double *dvar_gathered = NULL;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_gathered =
                malloc(global_domain.ncells_active * sizeof (*dvar_gathered));
        check_alloc_status(dvar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var_local, local_domain.ncells_active, MPI_DOUBLE,
            dvar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_DOUBLE,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (double), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, dvar_gathered, dvar);

        // cleanup
        free(dvar_gathered);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void gather_double_2d(double **dvar, double **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    
    double *tmp_global = NULL;
    double *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        for(j=0;j<local_domain.ncells_active;j++){
            tmp_local[j] = var_local[j][i];
        }

        gather_double(tmp_global,tmp_local);
            
            
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                dvar[j][i] = tmp_global[j];
            }
        }
    }
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_int(int *ivar, int *ivar_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    int *ivar_gathered = NULL;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_gathered =
                malloc(global_domain.ncells_active * sizeof (*ivar_gathered));
        check_alloc_status(ivar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(ivar_local, local_domain.ncells_active, MPI_INT,
            ivar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_INT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (int), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, ivar_gathered, ivar);

        // cleanup
        free(ivar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_sizet(size_t *svar, size_t *svar_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    size_t *svar_gathered = NULL;
        
    if (mpi_rank == VIC_MPI_ROOT) {
        svar_gathered =
                malloc(global_domain.ncells_active * sizeof (*svar_gathered));
        check_alloc_status(svar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(svar_local, local_domain.ncells_active, MPI_AINT,
            svar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_AINT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (size_t), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, svar_gathered, svar);

        // cleanup
        free(svar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_sizet_2d(size_t **svar, size_t **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    
    size_t *tmp_global = NULL;
    size_t *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        for(j=0;j<local_domain.ncells_active;j++){
            tmp_local[j] = var_local[j][i];
        }

        gather_sizet(tmp_global,tmp_local);
            
            
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                svar[j][i] = tmp_global[j];
            }
        }
    }
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_double(double *dvar, double *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    double *dvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_mapped =
                malloc(global_domain.ncells_active * sizeof (*dvar_mapped));
        check_alloc_status(dvar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (double), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, dvar, dvar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(dvar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_DOUBLE,
            var_local, local_domain.ncells_active, MPI_DOUBLE,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(dvar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_double_2d(double **dvar, double **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
        
    double *tmp_global = NULL;
    double *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                tmp_global[j] = dvar[j][i];
            }
        }
        
        scatter_double(tmp_global, tmp_local);
                        
        for(j=0;j<local_domain.ncells_active;j++){
            var_local[j][i] = tmp_local[j];
        }
    } 
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter integer variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_int(int *ivar, int *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    int *ivar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_mapped =
                malloc(global_domain.ncells_active * sizeof (*ivar_mapped));
        check_alloc_status(ivar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (int), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, ivar, ivar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(ivar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_INT,
            var_local, local_domain.ncells_active, MPI_INT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(ivar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter size_t variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_sizet(size_t *svar, size_t *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    size_t *svar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        svar_mapped =
                malloc(global_domain.ncells_active * sizeof (*svar_mapped));
        check_alloc_status(svar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (size_t), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, svar, svar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(svar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_AINT,
            var_local, local_domain.ncells_active, MPI_AINT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(svar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter size_t variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_sizet_2d(size_t **svar, size_t **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
        
    size_t *tmp_global = NULL;
    size_t *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                tmp_global[j] = svar[j][i];
            }
        }
        
        scatter_sizet(tmp_global, tmp_local);
                        
        for(j=0;j<local_domain.ncells_active;j++){
            var_local[j][i] = tmp_local[j];
        }
    }    
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

void 
create_MPI_ext_option_struct_type(MPI_Datatype *mpi_type){
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;
    
    nitems = 1;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");

    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");

    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.");
    
    // none of the elements in location_struct are arrays.
    for(i=0; i< (size_t) nitems; i++){
        blocklengths[i]=1;
    }
    
    //reset i
    i=0;
    
    //bool GROUNDWATER;    
    offsets[i] = offsetof(ext_option_struct, GROUNDWATER);
    mpi_types[i++] = MPI_C_BOOL;
        
    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }

    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");

    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void 
create_MPI_ext_parameters_struct_type(MPI_Datatype *mpi_type){
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;
    
    nitems = 0;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");

    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");

    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.");
      
    // most the elements in location_struct are not arrays.
    for(i=0; i< (size_t) nitems; i++){
        blocklengths[i]=1;
    }
    
    //reset i
    i=0;
        
    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }

    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");

    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}