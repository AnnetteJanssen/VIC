#include <vic.h>

void
initialize_wu_var(wu_var_struct *wu_var)
{    
    size_t i;
    
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[i].demand = 0.0;
        wu_var[i].withdrawn = 0.0;
        wu_var[i].consumed = 0.0; 
        wu_var[i].returned = 0.0; 
    }
}

void
initialize_wu_hist(wu_hist_struct *wu_hist)
{
    size_t i;
    
    for(i = 0; i < WU_NSECTORS; i++){
        wu_hist[i].consumption_fraction = 0.0; 
        wu_hist[i].demand = 0.0;
    }
}

void
initialize_wu_con(wu_con_struct *wu_con)
{    
    int i;
    
    wu_con->link_id = 0;    
    for(i = 0; i < wu_con->nreceiving; i++){
        wu_con->receiving[i] = MISSING_USI;
    }
    for(i = 0; i < wu_con->nsending; i++){
        wu_con->sending[i] = MISSING_USI;
    }
}

void
initialize_wu_local_structures(void)
{
    extern domain_struct local_domain;
    extern wu_var_struct **wu_var;
    extern wu_hist_struct **wu_hist;
    extern wu_con_struct *wu_con;
    
    size_t i;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_wu_con(&wu_con[i]);
        initialize_wu_hist(wu_hist[i]);
        initialize_wu_var(wu_var[i]);
    }
}