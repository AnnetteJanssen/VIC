#include <vic_driver_image.h>
#include <plugin.h>

void
wu_forcing(void)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_force_struct **wu_force;
    extern size_t NF;
    
    double *dvar;
    int demand_idx;
    int groundwater_idx;
    int consumption_idx;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    size_t i;
    size_t j;
    size_t k;
    int iSector;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    for(k = 0; k < WU_NSECTORS; k++){
        if(plugin_options.WU_INPUT[k] == WU_FROM_FILE){
            if(k == WU_IRRIGATION){
                demand_idx = FORCING_IRR_DEMAND;
                groundwater_idx = FORCING_IRR_GROUNDWATER;
                consumption_idx = FORCING_IRR_CONSUMPTION;
            } else if (k == WU_MUNICIPAL){
                demand_idx = FORCING_MUN_DEMAND;
                groundwater_idx = FORCING_MUN_GROUNDWATER;
                consumption_idx = FORCING_MUN_CONSUMPTION;
            } else if (k == WU_LIVESTOCK){
                demand_idx = FORCING_LIV_DEMAND;
                groundwater_idx = FORCING_LIV_GROUNDWATER;
                consumption_idx = FORCING_LIV_CONSUMPTION;
            } else if (k == WU_ENERGY){
                demand_idx = FORCING_ENG_DEMAND;
                groundwater_idx = FORCING_ENG_GROUNDWATER;
                consumption_idx = FORCING_ENG_CONSUMPTION;
            } else if (k == WU_MANUFACTURING){
                demand_idx = FORCING_MAN_DEMAND;
                groundwater_idx = FORCING_MAN_GROUNDWATER;
                consumption_idx = FORCING_MAN_CONSUMPTION;
            } else {
                log_err("Unknown water-use sector");
            }
            
            for (j = 0; j < NF; j++) {
                d3start[0] = global_param.forceskip[0] +
                             global_param.forceoffset[0] + j - NF;

                get_scatter_nc_field_double(&(plugin_filenames.forcing[demand_idx]), 
                    plugin_filenames.f_varname[demand_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].demand[j] = dvar[i];
                    }
                }

                get_scatter_nc_field_double(&(plugin_filenames.forcing[groundwater_idx]), 
                    plugin_filenames.f_varname[groundwater_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].groundwater_frac[j] = dvar[i];
                    }
                }

                get_scatter_nc_field_double(&(plugin_filenames.forcing[consumption_idx]), 
                    plugin_filenames.f_varname[consumption_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].consumption_frac[j] = dvar[i];
                    }
                }
            }
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(k = 0; k < plugin_options.NWUTYPES; k++){
            iSector = wu_con_map[i].sidx[k];
            
            if(iSector != NODATA_WU){
                wu_force[i][iSector].demand[NR] = average(wu_force[i][iSector].demand, NF);
                wu_force[i][iSector].groundwater_frac[NR] = average(wu_force[i][iSector].groundwater_frac, NF);
                wu_force[i][iSector].consumption_frac[NR] = average(wu_force[i][iSector].consumption_frac, NF);
            }
        }
    }
    
    free(dvar);
}