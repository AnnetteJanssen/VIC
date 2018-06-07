/******************************************************************************
 * @section DESCRIPTION
 *
 * This subroutine initalizes all filefilenames before they are called by
 * the model.
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
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_filenames()
{
    extern filenames_struct filenames;

    size_t                  i;

    strcpy(filenames.init_state.nc_filename, "MISSING");
    strcpy(filenames.statefile, "MISSING");
    strcpy(filenames.constants, "MISSING");
    strcpy(filenames.params.nc_filename, "MISSING");
    strcpy(filenames.domain.nc_filename, "MISSING");
    strcpy(filenames.result_dir, "MISSING");
    strcpy(filenames.log_path, "MISSING");
    for (i = 0; i < N_FORCING_TYPES; i++) {
        strcpy(filenames.f_path_pfx[i], "MISSING");
        strcpy(filenames.forcing[i].nc_filename, "MISSING");
    }

    strcpy(filenames.mpi.nc_filename, "MISSING");
    strcpy(filenames.groundwater.nc_filename, "MISSING");
    strcpy(filenames.routing.nc_filename, "MISSING");
    strcpy(filenames.routing_forcing_pfx, "MISSING");
    strcpy(filenames.routing_forcing.nc_filename, "MISSING");
    strcpy(filenames.rout_params.nc_filename, "MISSING");
    for (i = 0; i < WU_NSECTORS; i++) {
        strcpy(filenames.water_use_forcing_pfx[i], "MISSING");
        strcpy(filenames.water_use_forcing[i].nc_filename, "MISSING");
    }
    strcpy(filenames.irrigation.nc_filename, "MISSING");
    strcpy(filenames.dams.nc_filename, "MISSING");
    strcpy(filenames.efr_forcing_pfx, "MISSING");
    strcpy(filenames.efr_forcing.nc_filename, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all file pointers
 *****************************************************************************/
void
initialize_fileps()
{
    extern filep_struct filep;

    filep.globalparam = NULL;
    filep.constants = NULL;
    filep.logfile = NULL;
}
