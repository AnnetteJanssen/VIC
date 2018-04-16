#include <vic.h>

void
wu_run(size_t cur_cell)
{
    extern option_struct options;
    extern wu_hist_struct **wu_hist;
    extern wu_var_struct **wu_var;
    extern rout_var_struct *rout_var;
    
    double total_available;
    double total_demand;
    double total_withdrawn;
    double withdrawn_fraction;
    
    size_t i;    
    size_t j;
    
    // Reset values
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[cur_cell][i].demand = wu_hist[cur_cell][i].demand;
        wu_var[cur_cell][i].withdrawn = 0.0;
        wu_var[cur_cell][i].consumed = 0.0;
        wu_var[cur_cell][i].returned = 0.0;
    }    
    
    // Get availability
    total_available = 0.0;
    for(i = 0; i < options.RIRF_NSTEPS; i++){
        total_available += rout_var[cur_cell].discharge[i];
    }
    
    // Satisfy demand
    total_demand = 0.0;
    for(i = 0; i < WU_NSECTORS; i++){
        total_demand += wu_var[cur_cell][i].demand;
    }
    
    total_withdrawn = 0.0;
    if(options.WU_STRATEGY == WU_STRATEGY_EQUAL){
        
        withdrawn_fraction = 0.0;
        if(total_available > 0 && total_demand > 0){
            withdrawn_fraction = total_available / total_demand;
            if(withdrawn_fraction > 1){
                withdrawn_fraction = 1.0;
            }

            for(i = 0; i < WU_NSECTORS; i++){
                wu_var[cur_cell][i].withdrawn = 
                        wu_var[cur_cell][i].demand * 
                        withdrawn_fraction;
                wu_var[cur_cell][i].consumed += 
                        wu_var[cur_cell][i].withdrawn * 
                        wu_hist[cur_cell][i].consumption_fraction;
                wu_var[cur_cell][i].returned += 
                        wu_var[cur_cell][i].withdrawn * 
                        (1 - wu_hist[cur_cell][i].consumption_fraction);
            
                total_withdrawn += wu_var[cur_cell][i].withdrawn;
            }
        }
    } else {
        log_err("WU_STRATEGY PRIORITY has not been implemented yet");
    }
     
    // Discharge reduction and return flow
    if(total_withdrawn > 0){
        for(i = 0; i < WU_NSECTORS; i++){
            for(j = 0; j < options.RIRF_NSTEPS; j++){
                rout_var[cur_cell].discharge[j] -=
                        (wu_var[cur_cell][i].withdrawn - 
                        wu_var[cur_cell][i].returned) * 
                        (rout_var[cur_cell].discharge[j] / 
                        total_available);

                if(rout_var[cur_cell].discharge[j] < 0){
                    if(abs(rout_var[cur_cell].discharge[j]) > DBL_EPSILON){
                        log_err("Routing discharge reduction due to water use "
                                "is abnormally large [%.4f]", 
                                rout_var[cur_cell].discharge[j]);
                    } else {
                        rout_var[cur_cell].discharge[j] = 0.0;
                    }
                }    
            }
        }
    }
    
    if(options.WU_REMOTE){
        log_err("WU_REMOTE has not been implemented yet");
    }
}