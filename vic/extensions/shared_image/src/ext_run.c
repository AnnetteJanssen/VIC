#include <ext_driver_shared_image.h>

void
ext_run(dmy_struct dmy)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_option_struct ext_options;
    
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    extern double ***out_data;    
    extern size_t *cell_order;
    
    extern int mpi_decomposition;
    
    ext_all_vars_struct *ext_all_vars_global=NULL;
    rout_con_struct *rout_con_global=NULL;
    double *runoff_global=NULL;
    
    double runoff;
    
    timer_struct               timer;
    size_t i;
    size_t j;
    size_t cur_id;
    
    // Update variables locally
    for(i=0;i<local_domain.ncells_active;i++){
        if(ext_options.ROUTING){
            routing_update_step_vars(&ext_all_vars[i].rout_var);
        }
        if(ext_options.DAMS){
            for(j=0;j<dam_con_map[i].Ndams;j++){
                dams_update_step_vars(&ext_all_vars[i].dam_var[j], dam_con[i][j]);
            }
        }
    }
       
    // Run the extensions
    timer_start(&timer);
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        for(i=0;i<local_domain.ncells_active;i++){
            cur_id = cell_order[i];
            
            if(ext_options.ROUTING){
                runoff = ((out_data[cur_id][OUT_RUNOFF][0] + out_data[cur_id][OUT_BASEFLOW][0]) * 
                        local_domain.locations[cur_id].area) / (MM_PER_M * global_param.dt);
                routing_run(rout_con[cur_id], &ext_all_vars[cur_id], ext_all_vars, runoff);
            }
            if(ext_options.DAMS){
                for(j=0;j<dam_con_map[cur_id].Ndams;j++){
                    dam_run(dam_con[cur_id][j],&ext_all_vars[cur_id].dam_var[j], &ext_all_vars[cur_id].rout_var, dmy);
                }                
            }
        }
    }
    
    else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        
        // Gather everything to the master node
        if(ext_options.ROUTING){
            routing_run_alloc(&ext_all_vars_global,&rout_con_global,&runoff_global);
            routing_run_gather(ext_all_vars_global,rout_con_global,runoff_global);
        }
        
        for(i=0;i<global_domain.ncells_active;i++){
            cur_id = cell_order[i];
            
            if(ext_options.ROUTING){
                routing_run(rout_con_global[cur_id],&ext_all_vars_global[cur_id],ext_all_vars_global, runoff_global[cur_id]); 
                log_warn("MPI decomposition not implemented for natural discharge");
            }
            if(ext_options.DAMS){
                log_err("MPI decomposition not implemented for dams");
            }
        }
        
        // Scatter everything to the local nodes
        if(ext_options.ROUTING){
            routing_run_scatter(ext_all_vars_global);
            routing_run_free(ext_all_vars_global,rout_con_global,runoff_global);
        }
    }
    timer_stop(&timer);
    
    // Save the output
    for(i=0;i<local_domain.ncells_active;i++){
        ext_put_data(&ext_all_vars[i], dam_con[i], dam_con_map[i], out_data[i], &timer);            
    }
}