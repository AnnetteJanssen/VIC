#include <vic.h>

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern size_t NF;
    extern wu_var_struct **wu_var;
    extern wu_hist_struct **wu_hist;
    extern wu_force_struct **wu_force;
    
    size_t i;
    size_t j;
    
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var,"Memory allocation error");
    
    wu_hist = malloc(local_domain.ncells_active * sizeof(*wu_hist));
    check_alloc_status(wu_hist,"Memory allocation error");
    
    wu_force = malloc(local_domain.ncells_active * sizeof(*wu_force));
    check_alloc_status(wu_force,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){        
        wu_hist[i] = malloc(WU_NSECTORS * sizeof(*wu_hist[i]));
        check_alloc_status(wu_hist[i],"Memory allocation error");
        
        wu_var[i] = malloc(WU_NSECTORS * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i],"Memory allocation error");
        
        wu_force[i] = malloc(WU_NSECTORS * sizeof(*wu_force[i]));
        check_alloc_status(wu_force[i],"Memory allocation error");
        
        for(j = 0; j < WU_NSECTORS; j++){        
            wu_force[i][j].consumption_fraction = malloc(NF * sizeof(*wu_force[i][j].consumption_fraction));
            check_alloc_status(wu_force[i][j].consumption_fraction,"Memory allocation error");
            
            wu_force[i][j].demand = malloc(NF * sizeof(*wu_force[i][j].demand));
            check_alloc_status(wu_force[i][j].demand,"Memory allocation error");
        }
    }   
}

void
wu_finalize(void)
{
    extern domain_struct local_domain;
    extern wu_var_struct **wu_var;
    extern wu_hist_struct **wu_hist;
    
    size_t i;
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < WU_NSECTORS; j++){    
            free(wu_force[i][j].consumption_fraction);
            free(wu_force[i][j].demand);
        }
        free(wu_force[i]);
        free(wu_hist[i]);
        free(wu_var[i]);
    }
    free(wu_force);
    free(wu_hist);
    free(wu_var);
}