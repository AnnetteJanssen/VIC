/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine handles the startup tasks.
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

/******************************************************************************
 * @brief    Wrapper function for VIC domain validation tasks.
 *****************************************************************************/
void
validate_domain_general(void)
{
    int                        status;
    extern filenames_struct    filenames;
    
    // open parameter file
    status = nc_open(filenames.params.nc_filename, NC_NOWRITE,
                     &(filenames.params.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.params.nc_filename);

    // check whether lat and lon coordinates in the parameter file match those
    // in the domain file
    compare_ncdomain_with_global_domain(&filenames.params);

    // close parameter file
    status = nc_close(filenames.params.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.params.nc_filename);
}