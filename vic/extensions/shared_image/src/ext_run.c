#include <ext_driver_shared_image.h>

void
ext_run()
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_option_struct ext_options;
    
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern double ***out_data;    
    
    extern int mpi_decomposition;
    
    ext_all_vars_struct *ext_all_vars_global=NULL;
    rout_con_struct *rout_con_global=NULL;
    double *runoff_global=NULL;
    
    double runoff;
    
    timer_struct               timer;
    size_t i;
        
    // Update variables locally
    for(i=0;i<local_domain.ncells_active;i++){
        if(ext_options.ROUTING){
            routing_update_step_vars(&ext_all_vars[i]);
        }
    }
       
    // Run the extensions
    timer_start(&timer);
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        for(i=0;i<local_domain.ncells_active;i++){
            if(ext_options.ROUTING){
                runoff = (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) * local_domain.locations[i].area / (MM_PER_M * global_param.dt);
                routing_run(rout_con[i],&ext_all_vars[i],ext_all_vars, runoff); 
            }
        }
    }
    
    //TODO: implement random decomposition support for water use
    else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        
        // Gather everything to the master node
        if(ext_options.ROUTING){
            routing_run_alloc(&ext_all_vars_global,&rout_con_global,&runoff_global);
            routing_run_gather(ext_all_vars_global,rout_con_global,runoff_global);
        }
        
        for(i=0;i<global_domain.ncells_active;i++){
            if(ext_options.ROUTING){
                routing_run(rout_con_global[i],&ext_all_vars_global[i],ext_all_vars_global, runoff_global[i]); 
            }
        }
        
        // Scater everything to the local nodes
        if(ext_options.ROUTING){
            routing_run_scatter(ext_all_vars_global);
            routing_run_free(ext_all_vars_global,rout_con_global,runoff_global);
        }
    }
    timer_stop(&timer);
    
    // Save the output
    for(i=0;i<local_domain.ncells_active;i++){
        ext_put_data(&ext_all_vars[i], out_data[i], &timer);            
    }
}