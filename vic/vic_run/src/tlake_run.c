#include <vic_run.h>

veg_lib_struct *vic_run_veg_lib;

/******************************************************************************
* @brief        This subroutine controls the lake temperature core, it solves
*               the energy balance model only for lakes
* @author       Bram Droppers (9-2018)
******************************************************************************/
int
tlake_run(force_data_struct   *force,
            all_vars_struct     *all_vars,
            dmy_struct          *dmy,
            global_param_struct *gp,
            lake_con_struct     *lake_con,
            soil_con_struct     *soil_con,
            veg_con_struct      *veg_con,
            veg_lib_struct      *veg_lib)
{
    extern option_struct     options;
    extern parameters_struct param;

    unsigned short           iveg;
    size_t                   Nveg;
    unsigned short           band;
    size_t                   Nbands;
    int                      ErrorFlag;
    double                   Cv;
    double                   rainonly;
    double                   sum_runoff;
    double                   sum_baseflow;
    double                   gauge_correction[2];
    double                   lakefrac;
    double                   fraci;
    double                   wetland_runoff;
    double                   wetland_baseflow;
    double                   snowprec;
    double                   rainprec;
    lake_var_struct         *lake_var;
    cell_data_struct        *cell;
    
    /* set local pointers */
    lake_var = &all_vars->lake_var;

    Nbands = options.SNOW_BAND;

    /* Set number of vegetation tiles */
    Nveg = veg_con[0].vegetat_type_num;
    
    /****************************
       Run Lake Model
    ****************************/

    /** Compute total runoff and baseflow for all vegetation types
        within each snowband. **/
    if (options.LAKES && lake_con->lake_idx >= 0) {
        wetland_runoff = wetland_baseflow = 0;
        sum_runoff = sum_baseflow = 0;

        /** Run lake model **/
        iveg = lake_con->lake_idx;
        band = 0;
        lake_var->runoff_in =
            (sum_runoff * lake_con->rpercent +
             wetland_runoff) * soil_con->cell_area / MM_PER_M;                                               // m3
        lake_var->baseflow_in =
            (sum_baseflow * lake_con->rpercent +
             wetland_baseflow) * soil_con->cell_area / MM_PER_M;                                                 // m3
        lake_var->channel_in = force->channel_in[NR] * soil_con->cell_area /
                               MM_PER_M;                                        // m3
        // TODO: probably set precipitation to zero
        lake_var->prec = force->prec[NR] * lake_var->sarea / MM_PER_M; // m3
        rainonly = calc_rainonly(force->air_temp[NR], force->prec[NR],
                                 param.SNOW_MAX_SNOW_TEMP,
                                 param.SNOW_MIN_RAIN_TEMP);
        if ((int) rainonly == ERROR) {
            return(ERROR);
        }

        /**********************************************************************
           Solve the energy budget for the lake.
        **********************************************************************/
        // TODO: probably remove?
        snowprec = gauge_correction[SNOW] * (force->prec[NR] - rainonly);
        rainprec = gauge_correction[SNOW] * rainonly;
        Cv = veg_con[iveg].Cv * lakefrac;
        force->out_prec += (snowprec + rainprec) * Cv;
        force->out_rain += rainprec * Cv;
        force->out_snow += snowprec * Cv;

        ErrorFlag = solve_lake_tlake(snowprec, rainprec, force->air_temp[NR],
                               force->wind[NR], force->vp[NR] / PA_PER_KPA,
                               force->shortwave[NR], force->longwave[NR],
                               force->vpd[NR] / PA_PER_KPA,
                               force->pressure[NR] / PA_PER_KPA,
                               force->density[NR], lake_var,
                               *soil_con, gp->dt, gp->wind_h, *dmy,
                               fraci);
        if (ErrorFlag == ERROR) {
            return (ERROR);
        }

        /**********************************************************************
           Solve the water budget for the lake.
        **********************************************************************/
        
        ErrorFlag = water_balance_tlake(lake_var, *lake_con, gp->dt, all_vars,
                                  iveg, band, lakefrac, *soil_con,
                                  veg_con[iveg]);
        if (ErrorFlag == ERROR) {
            return (ERROR);
        }
    } // end if (options.LAKES && lake_con->lake_idx >= 0)
}