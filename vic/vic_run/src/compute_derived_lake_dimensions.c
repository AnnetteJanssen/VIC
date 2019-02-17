/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine computes those lake variables that are completely dependent
 * on lake depth and basin dimensions.
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_shared_all.h>

/******************************************************************************
 * @brief    This routine computes those lake variables that are completely
 *           dependent on lake depth and basin dimensions.
 *****************************************************************************/
void
compute_derived_lake_dimensions(lake_var_struct *lake,
                                lake_con_struct *lake_con)
{
    extern parameters_struct param;

    size_t                   Nlake;
    size_t                   iLake;
    int                      k;
    int                      status;
    double                   depth;
    double                   tmp_volume;

    Nlake = lake_con[0].lake_type_num;
    
    for (iLake = 0; iLake <= Nlake; iLake++) {
        /* number and thicknesses of lake layers */
        if (lake[iLake].ldepth > param.LAKE_MAX_SURFACE && lake[iLake].ldepth < 2 *
            param.LAKE_MAX_SURFACE) {
            /* Not quite enough for two full layers. */
            lake[iLake].surfdz = lake[iLake].ldepth / 2.;
            lake[iLake].dz = lake[iLake].ldepth / 2.;
            lake[iLake].activenod = 2;
        }
        else if (lake[iLake].ldepth >= 2 * param.LAKE_MAX_SURFACE) {
            /* More than two layers. */
            lake[iLake].surfdz = param.LAKE_MAX_SURFACE;
            lake[iLake].activenod = (int) (lake[iLake].ldepth / param.LAKE_MAX_SURFACE);
            if (lake[iLake].activenod > MAX_LAKE_NODES) {
                lake[iLake].activenod = MAX_LAKE_NODES;
            }
            lake[iLake].dz = (lake[iLake].ldepth - lake[iLake].surfdz) /
                       ((double) (lake[iLake].activenod - 1));
        }
        else if (lake[iLake].ldepth > DBL_EPSILON) {
            lake[iLake].surfdz = lake[iLake].ldepth;
            lake[iLake].dz = 0.0;
            lake[iLake].activenod = 1;
        }
        else {
            lake[iLake].surfdz = 0.0;
            lake[iLake].dz = 0.0;
            lake[iLake].activenod = 0;
            lake[iLake].ldepth = 0.0;
        }

        // lake_con[iLake].basin equals the surface area at specific depths as input by
        // the user in the lake parameter file or calculated in read_lakeparam(),
        // lake[iLake].surface equals the area at the top of each dynamic solution layer

        for (k = 0; k <= lake[iLake].activenod; k++) {
            if (k == 0) {
                depth = lake[iLake].ldepth;
            }
            else {
                depth = lake[iLake].dz * (lake[iLake].activenod - k);
            }
            status = get_sarea(lake_con[iLake], depth, &(lake[iLake].surface[k]));
            if (status < 0) {
                log_err("record = %d, depth = %f, "
                        "sarea = %e", 0, depth, lake[iLake].surface[k]);
            }
        }

        lake[iLake].sarea = lake[iLake].surface[0];
        status = get_volume(lake_con[iLake], lake[iLake].ldepth, &tmp_volume);
        if (status < 0) {
            log_err("record = %d, depth = %f, "
                    "volume = %e", 0, depth, tmp_volume);
        }
        else if (status > 0) {
            log_err("lake depth exceeds maximum; "
                    "setting to maximum; record = %d", 0);
        }
        lake[iLake].volume = tmp_volume + lake[iLake].ice_water_eq;
    }
}
