#include <vic_driver_image.h>
#include <plugin.h>

void
dam_generate_default_state(void)
{
    extern domain_struct       local_domain;
    extern plugin_option_struct       plugin_options;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_con_struct     **local_dam_con;
    extern dam_var_struct     **local_dam_var;
    extern dam_con_map_struct  *global_dam_con_map;
    extern dam_con_struct     **global_dam_con;
    extern dam_var_struct     **global_dam_var;
    extern plugin_parameters_struct   plugin_param;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NDAMTYPES; j++){
            if(local_dam_con_map[i].didx[j] != NODATA_DAM){
                local_dam_var[i][j].storage = local_dam_con[i][j].capacity * 0;
            }
            if(global_dam_con_map[i].didx[j] != NODATA_DAM){
                global_dam_var[i][j].storage = global_dam_con[i][j].capacity * plugin_param.DAM_ALPHA;
            }
        }
    }
}