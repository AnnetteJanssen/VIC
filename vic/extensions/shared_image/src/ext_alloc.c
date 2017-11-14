#include <ext_driver_shared_image.h>

void
ext_alloc(void)
{    
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    
    ext_all_vars = malloc(local_domain.ncells_active * sizeof(*ext_all_vars));
    check_alloc_status(ext_all_vars, "Memory allocation error");   
    
    if(ext_options.GROUNDWATER){
        gw_alloc();
    }    
        
    initialize_ext_local_structures();
}