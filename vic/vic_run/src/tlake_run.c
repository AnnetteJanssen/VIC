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
            veg_con_struct      *veg_con)
{
    extern option_struct     options;
    extern parameters_struct param;

    unsigned short           iveg;
    int                      ErrorFlag;
    double                   Cv;
    double                   Tair;
    double                   Prec;
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
    
    /* set local pointers */
    lake_var = &all_vars->lake_var;
    
    /* Update areai to equal new ice area from previous time step. */
    lake_var->areai = lake_var->new_ice_area;
    
    /* Compute lake fraction and ice-covered fraction */
    if (lake_var->areai < 0) {
        lake_var->areai = 0;
    }
    if (lake_var->sarea > 0) {
        fraci = lake_var->areai / lake_var->sarea;
        if (fraci > 1.0) {
            fraci = 1.0;
        }
    }
    else {
        fraci = 0.0;
    }
    lakefrac = lake_var->sarea / lake_con->basin[0];
    
    /* set air temperature and precipitation for this snow band */
    Tair = force->air_temp[NR] + soil_con->Tfactor[lake_con->lake_elev_idx];
    Prec = force->prec[NR] * soil_con->Pfactor[lake_con->lake_elev_idx];
    
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
        lake_var->runoff_in =
            (sum_runoff * lake_con->rpercent +
             wetland_runoff) * soil_con->cell_area / MM_PER_M;                                               // m3
        lake_var->baseflow_in =
            (sum_baseflow * lake_con->rpercent +
             wetland_baseflow) * soil_con->cell_area / MM_PER_M;                                                 // m3
        lake_var->channel_in = force->channel_in[NR] * soil_con->cell_area /
                               MM_PER_M;                                        // m3
        // TODO: probably set precipitation to zero
        lake_var->prec = Prec * lake_var->sarea / MM_PER_M; // m3
        rainonly = calc_rainonly(Tair, Prec,
                                 param.SNOW_MAX_SNOW_TEMP,
                                 param.SNOW_MIN_RAIN_TEMP);
        if ((int) rainonly == ERROR) {
            return(ERROR);
        }

        /**********************************************************************
           Solve the energy budget for the lake.
        **********************************************************************/
        // TODO: probably remove?
        snowprec = gauge_correction[SNOW] * (Prec - rainonly);
        rainprec = gauge_correction[SNOW] * rainonly;
        Cv = veg_con[iveg].Cv * lakefrac;
        force->out_prec += (snowprec + rainprec) * Cv;
        force->out_rain += rainprec * Cv;
        force->out_snow += snowprec * Cv;

        ErrorFlag = solve_lake_tlake(snowprec, rainprec, Tair,
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
    } // end if (options.LAKES && lake_con->lake_idx >= 0)
    
    return (0);
}