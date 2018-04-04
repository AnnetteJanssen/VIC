/******************************************************************************
 * @section DESCRIPTION
 *
 * Save model state.
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

#include <vic.h>
#include <routing_rvic.h>

/******************************************************************************
 * @brief    Save model state.
 *****************************************************************************/
void
gw_add_types(void) {

    extern node    *outvar_types;

    // add outvar_types
    outvar_types = list_prepend(outvar_types, "OUT_GW_ZWT");
    outvar_types = list_prepend(outvar_types, "OUT_GW_RECHARGE");
    outvar_types = list_prepend(outvar_types, "OUT_GW_WA");
    outvar_types = list_prepend(outvar_types, "OUT_GW_WT");
    outvar_types = list_prepend(outvar_types, "OUT_GW_AVAIL");

    outvar_types = list_add_ids(outvar_types, N_OUTVAR_TYPES);
    N_OUTVAR_TYPES_ALL = list_count(outvar_types) + N_OUTVAR_TYPES;
}
