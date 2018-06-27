#include <vic.h>

void
irr_put_data(size_t cur_cell)
{
    extern irr_var_struct   ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;
    extern double           ***out_data;
    extern node               *outvar_types;

    size_t                     j;
    size_t                     k;
    size_t                     cur_veg;
    
    int                        OUT_IRR_REQUIREMENT;
    int                        OUT_IRR_NEED;
    int                        OUT_IRR_DEFICIT;
    int                        OUT_IRR_SHORTAGE;
    int                        OUT_IRR_POND_STORAGE;
    int                        OUT_IRR_LEFTOVER;
    
    OUT_IRR_REQUIREMENT = list_search_id(outvar_types, "OUT_IRR_REQUIREMENT");
    OUT_IRR_NEED = list_search_id(outvar_types, "OUT_IRR_NEED");
    OUT_IRR_DEFICIT = list_search_id(outvar_types, "OUT_IRR_DEFICIT");
    OUT_IRR_SHORTAGE = list_search_id(outvar_types, "OUT_IRR_SHORTAGE");
    OUT_IRR_POND_STORAGE = list_search_id(outvar_types, "OUT_IRR_POND_STORAGE");
    OUT_IRR_LEFTOVER = list_search_id(outvar_types, "OUT_IRR_LEFTOVER");
    
    for (j = 0; j < irr_con_map[cur_cell].ni_active; j++) {
        cur_veg = irr_con[cur_cell][j].veg_index;

        for(k = 0; k < elev_con_map[cur_cell].ne_active; k++){
            out_data[cur_cell][OUT_IRR_REQUIREMENT][0] += irr_var[cur_cell][j][k].requirement * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;
            out_data[cur_cell][OUT_IRR_SHORTAGE][0] += irr_var[cur_cell][j][k].shortage * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;

            out_data[cur_cell][OUT_IRR_NEED][0] += irr_var[cur_cell][j][k].need * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;
            out_data[cur_cell][OUT_IRR_DEFICIT][0] += irr_var[cur_cell][j][k].deficit * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;

            out_data[cur_cell][OUT_IRR_POND_STORAGE][0] += irr_var[cur_cell][j][k].pond_storage * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;
            out_data[cur_cell][OUT_IRR_LEFTOVER][0] += irr_var[cur_cell][j][k].leftover * 
                soil_con[cur_cell].AreaFract[k] * veg_con[cur_cell][cur_veg].Cv;
        }
    }
}
