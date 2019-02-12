#include <vic_driver_image.h>
#include <plugin.h>

void
wu_set_output_met_data_info(void)
{
    extern plugin_option_struct plugin_options;
    extern metadata_struct out_metadata[];

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].varname, "OUT_AVAILABLE_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].long_name, "available_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].standard_name, "available_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].description, "available from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].varname, "OUT_AVAILABLE_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].long_name, "available_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].standard_name, "available_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].description, "available from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].varname, "OUT_AVAILABLE_REMOTE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].long_name, "available_remote_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].standard_name, "available_remote_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].description, "available for remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].varname, "OUT_DEMAND_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].long_name, "demand_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].standard_name, "demand_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].description, "demand from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].varname, "OUT_DEMAND_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].long_name, "demand_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].standard_name, "demand_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].description, "demand from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].varname, "OUT_DEMAND_REMOTE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].long_name, "demand_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].standard_name, "demand_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].description, "demand from remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].varname, "OUT_WITHDRAWN_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].long_name, "withdrawn_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].standard_name, "withdrawn_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].description, "withdrawn from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].varname, "OUT_WITHDRAWN_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].long_name, "withdrawn_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].standard_name, "withdrawn_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].description, "withdrawn from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].varname, "OUT_WITHDRAWN_REMOTE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].long_name, "withdrawn_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].standard_name, "withdrawn_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].description, "withdrawn from remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].varname, "OUT_WITHDRAWN");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].long_name, "withdrawn");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].standard_name, "withdrawn");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].description, "total water withdrawn for sector");
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].varname, "OUT_AVAILABLE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].long_name, "available");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].standard_name, "available");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].description, "total water available for sector");
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].varname, "OUT_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].long_name, "demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].standard_name, "demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].description, "total water demand for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].varname, "OUT_RETURNED");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].long_name, "returned");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].standard_name, "returned");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].description, "water returned for sector");

    out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_GW].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_SURF].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_GW].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_SURF].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DEMAND_REMOTE].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_GW].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].nelem = plugin_options.NWUTYPES;
}

void
wu_initialize_nc_file(nc_file_struct  *nc_state_file)
{
    extern plugin_option_struct plugin_options;
    
    nc_state_file->wu_dimid = MISSING;
    nc_state_file->wu_size = plugin_options.NWUTYPES;
}

// Add dimensions to outfile
void
wu_add_hist_dim(nc_file_struct *nc,
                stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc->nc_id, "wu_class", nc->wu_size, &(nc->wu_dimid));
    check_nc_status(status, "Error defining wu_class bounds dimension in %s",
                    stream->filename);
}

void
wu_set_nc_var_info(unsigned int    varid,
                   nc_file_struct *nc_hist_file,
                   nc_var_struct  *nc_var)
{
    
    // Set the number of dimensions and dimids for each state variable
    switch(varid){
        case N_OUTVAR_TYPES + OUT_AVAILABLE_GW:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_SURF:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE:
        case N_OUTVAR_TYPES + OUT_DEMAND_GW:
        case N_OUTVAR_TYPES + OUT_DEMAND_SURF:
        case N_OUTVAR_TYPES + OUT_DEMAND_REMOTE:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_GW:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_hist_file->wu_size;
        nc_var->nc_counts[2] = nc_hist_file->nj_size;
        nc_var->nc_counts[3] = nc_hist_file->ni_size;
    }
}

void
wu_set_nc_var_dimids(unsigned int    varid,
                     nc_file_struct *nc_hist_file,
                     nc_var_struct  *nc_var)
{
    switch(varid){
        case N_OUTVAR_TYPES + OUT_AVAILABLE_GW:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_SURF:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE:
        case N_OUTVAR_TYPES + OUT_DEMAND_GW:
        case N_OUTVAR_TYPES + OUT_DEMAND_SURF:
        case N_OUTVAR_TYPES + OUT_DEMAND_REMOTE:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_GW:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->wu_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_hist_file->ni_dimid;
    }
}

void
wu_history(unsigned int varid, unsigned int *agg_type)
{
    switch (varid) {
        case N_OUTVAR_TYPES + OUT_AVAILABLE_GW:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_SURF:
        case N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE:
        case N_OUTVAR_TYPES + OUT_DEMAND_GW:
        case N_OUTVAR_TYPES + OUT_DEMAND_SURF:
        case N_OUTVAR_TYPES + OUT_DEMAND_REMOTE:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_GW:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        (*agg_type) = AGG_TYPE_SUM;
    }
}

void
wu_put_data(size_t cur_cell)
{
    extern plugin_option_struct plugin_options;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern double ***out_data;
    
    size_t i;
    int iSector;
    
    for(i = 0; i < plugin_options.NWUTYPES; i++){
        iSector = wu_con_map[cur_cell].sidx[i];
        if(iSector != NODATA_WU){
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE_GW][i] = wu_var[cur_cell][iSector].available_gw;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE_SURF][i] = wu_var[cur_cell][iSector].available_surf;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE_REMOTE][i] = wu_var[cur_cell][iSector].available_remote;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_GW][i] = wu_var[cur_cell][iSector].demand_gw;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_SURF][i] = wu_var[cur_cell][iSector].demand_surf;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_REMOTE][i] = wu_var[cur_cell][iSector].demand_remote;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_GW][i] = wu_var[cur_cell][iSector].withdrawn_gw;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF][i] = wu_var[cur_cell][iSector].withdrawn_surf;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE][i] = wu_var[cur_cell][iSector].withdrawn_remote;
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE][i] = 
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE_GW][i] +
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_AVAILABLE_SURF][i];
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND][i] = 
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_GW][i] +
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_SURF][i] +
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_DEMAND_REMOTE][i];
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN][i] = 
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_GW][i] +
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_SURF][i] +
                    out_data[cur_cell][N_OUTVAR_TYPES + OUT_WITHDRAWN_REMOTE][i];
            out_data[cur_cell][N_OUTVAR_TYPES + OUT_RETURNED][i] = wu_var[cur_cell][iSector].returned;
        }
    }
}