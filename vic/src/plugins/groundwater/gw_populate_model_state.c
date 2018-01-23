#include <vic.h>

void
gw_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct *soil_con;
    extern option_struct options;
    extern gw_var_struct ***gw_var;
    extern all_vars_struct *all_vars;
    extern gw_con_struct *gw_con;
    extern option_struct options;
    extern filenames_struct filenames;
    extern int mpi_rank;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    size_t m;
    size_t n;
    
    double                    *dvar = NULL;    
    size_t                     d4count[4];
    size_t                     d4start[4];
    int status;
    
    double z_tmp;
    double ice;
    double eff_porosity;
    bool in_column;
    
    if(options.GW_INIT_FROM_FILE){

        dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
        
        d4start[0] = 0;
        d4start[1] = 0;
        d4start[2] = 0;
        d4start[3] = 0;
        d4count[0] = 1;
        d4count[1] = 1;
        d4count[2] = global_domain.n_ny;
        d4count[3] = global_domain.n_nx;

        // open parameter file
        if(mpi_rank == VIC_MPI_ROOT){
            status = nc_open(filenames.groundwater.nc_filename, NC_NOWRITE,
                             &(filenames.groundwater.nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.groundwater.nc_filename);
        }
    
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            d4start[0] = j;
            for (k = 0; k < options.SNOW_BAND; k++) {
                d4start[1] = k;
                get_scatter_nc_field_double(&(filenames.groundwater),
                        "zwt_init", d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    gw_var[i][j][k].zwt = (double) dvar[i];
                }
            }
        }
        
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            d4start[0] = j;
            for (k = 0; k < options.SNOW_BAND; k++) {
                d4start[1] = k;
                get_scatter_nc_field_double(&(filenames.groundwater),
                        "Ws_init", d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    gw_var[i][j][k].Ws = (double) dvar[i];
                }
            }
        }
        
        // close parameter file
        if(mpi_rank == VIC_MPI_ROOT){
            status = nc_close(filenames.groundwater.nc_id);
            check_nc_status(status, "Error closing %s",
                        filenames.groundwater.nc_filename);
        }
        
    }else{
        for(i=0; i<local_domain.ncells_active; i++){
            for(j=0; j<veg_con_map[i].nv_active; j++){
                for(k=0; k<options.SNOW_BAND; k++){
                    gw_var[i][j][k].zwt = GW_DEF_ZWT_INIT;
                }                
            }
        }
    }
    
    for(i=0; i<local_domain.ncells_active; i++){ 
        for(j=0; j<veg_con_map[i].nv_active; j++){
            for(k=0; k<options.SNOW_BAND; k++){   
                in_column = false;
                
                z_tmp = 0.0;    
                for(l=0; l<options.Nlayer; l++){
                    z_tmp += soil_con[i].depth[l];
                    
                    if(gw_var[i][j][k].zwt < z_tmp){
                        // groundwater table is in layer
                        in_column = true;
                        
                        // add water for current layer
                        ice = 0.0;
                        for(m=0; m<options.Nfrost; m++){
                            ice += all_vars[i].cell[j][k].layer[l].ice[m];
                        }                        
                        eff_porosity = (soil_con[i].max_moist[l] - ice) / 
                                (soil_con[i].depth[l] * MM_PER_M);
                        
                        gw_var[i][j][k].Wt +=
                                (z_tmp - gw_var[i][j][k].zwt) * 
                                eff_porosity * MM_PER_M;
                        
                        // add water for lower layers
                        for(n=l + 1; n<options.Nlayer; n++){  
                            z_tmp += soil_con[i].depth[n];
                            
                            ice = 0.0;
                            for(m=0; m<options.Nfrost; m++){
                                ice += all_vars[i].cell[j][k].layer[n].ice[m];
                            }                        
                            eff_porosity = (soil_con[i].max_moist[n] - ice) / 
                                    (soil_con[i].depth[n] * MM_PER_M);

                            gw_var[i][j][k].Wt +=
                                    soil_con[i].depth[n] * 
                                    eff_porosity * MM_PER_M;
                        }
                        
                        // add water for aquifer
                        gw_var[i][j][k].Wa = 
                                (GW_REF_DEPTH - z_tmp) * 
                                gw_con[i].Sy * MM_PER_M;
                        gw_var[i][j][k].Wt +=
                                gw_var[i][j][k].Wa;
                        
                        break;
                    }
                }
                if(!in_column){
                    gw_var[i][j][k].Wt = 
                            (GW_REF_DEPTH - gw_var[i][j][k].zwt) * 
                            gw_con[i].Sy * MM_PER_M;
                    gw_var[i][j][k].Wa = 
                            gw_var[i][j][k].Wt;
                }
            }
        }
    }    
}

void
gw_restore(void)
{
//    extern domain_struct global_domain;
//    extern domain_struct local_domain;
//    
//    double                    *dvar = NULL;
//    int                       *ivar = NULL;
//    
//    size_t                     d3count[3];
//    size_t                     d3start[3];
//    size_t                     d4count[4];
//    size_t                     d4start[4];
//    
//    d3start[0] = 0;
//    d3start[1] = 0;
//    d3start[2] = 0;
//    d3count[0] = 1;
//    d3count[1] = global_domain.n_ny;
//    d3count[2] = global_domain.n_nx;
//    
//    d4start[0] = 0;
//    d4start[1] = 0;
//    d4start[2] = 0;
//    d4start[3] = 0;
//    d4count[0] = 1;
//    d4count[1] = 1;
//    d4count[2] = global_domain.n_ny;
//    d4count[3] = global_domain.n_nx;
//        
//    // Allocate
//    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
//    check_alloc_status(dvar, "Memory allocation error");
//    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
//    check_alloc_status(ivar, "Memory allocation error");
//    
//    // Read variables from state file
//    
//    
//    // Free
//    free(dvar);
//    free(ivar);
}