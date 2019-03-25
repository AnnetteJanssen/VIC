#include <vic_driver_image.h>
#include <plugin.h>

void
irr_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].varname, MAXSTRING,
             "%s", "OUT_SHORTAGE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].long_name, MAXSTRING,
             "%s", "irrigation_shortage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].standard_name,
             MAXSTRING, "%s", "irrigation shortage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].description,
             MAXSTRING, "%s", "average irrigation shortage");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].varname, MAXSTRING,
             "%s", "OUT_REQUIREMENT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].long_name,
             MAXSTRING, "%s", "irrigation_requirement");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].standard_name,
             MAXSTRING, "%s", "irrigation requirement");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].description,
             MAXSTRING, "%s", "average irrigation requirement");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NEED].varname, MAXSTRING,
             "%s", "OUT_NEED");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NEED].long_name,
             MAXSTRING, "%s", "irrigation_need");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NEED].standard_name,
             MAXSTRING, "%s", "irrigation need");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NEED].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NEED].description,
             MAXSTRING, "%s", "cumulative irrigation requirement");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].varname, MAXSTRING,
             "%s", "OUT_DEFICIT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].long_name,
             MAXSTRING, "%s", "irrigation_deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].standard_name,
             MAXSTRING, "%s", "irrigation deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].description,
             MAXSTRING, "%s", "cumulative irrigation shortage");

    out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_NEED].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DEFICIT].nelem = 1;
}

void
irr_history(int           varid,
             unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_NEED:
    case  N_OUTVAR_TYPES + OUT_DEFICIT:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    case  N_OUTVAR_TYPES + OUT_REQUIREMENT:
    case  N_OUTVAR_TYPES + OUT_SHORTAGE:
        (*agg_type) = AGG_TYPE_AVG;
        break;
    }
}

void
irr_put_data(size_t iCell)
{
    extern option_struct options;
    extern irr_var_struct  ***irr_var;
    extern irr_con_map_struct   *irr_con_map;
    extern soil_con_struct   *soil_con;
    extern veg_con_struct   **veg_con;
    extern double           ***out_data;

    size_t i;
    size_t j;
    double veg_fract;
    double area_fract;
    int veg_index;
    
    for(i = 0; i < plugin_options.NIRRTYPES; i++){
        veg_index = irr_con_map[iCell].vidx[i];
        if(veg_index != NODATA_VEG){
            veg_fract = veg_con[iCell][veg_index].Cv;
                    
            for(j = 0; j < options.SNOW_BAND; j++){
                area_fract = soil_con[iCell].AreaFract[j];
                if(area_fract > 0){
                    
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_REQUIREMENT][0] = irr_var[iCell][i][j].requirement *
                            veg_fract * area_fract;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_SHORTAGE][0] = irr_var[iCell][i][j].shortage *
                            veg_fract * area_fract;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_DEFICIT][0] = irr_var[iCell][i][j].deficit *
                            veg_fract * area_fract;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_NEED][0] = irr_var[iCell][i][j].need *
                            veg_fract * area_fract;
                }
            }
        }
    }
}
