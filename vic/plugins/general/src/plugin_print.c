#include <vic_driver_shared_all.h>
#include "plugin.h"


void
plugin_print_decomposition(size_t   mpi_size,
                           int    **mpi_map_local_array_sizes)
{
    size_t i;
    
    fprintf(LOG_DEST, "mpi decomposition size:\n");
    for(i = 0; i < mpi_size; i++){
        fprintf(LOG_DEST, "\tnode %zu            : %d\n", i,
                (*mpi_map_local_array_sizes)[i]);
    }
}

void
plugin_print_global_param(plugin_global_param_struct *gp)
{
    fprintf(LOG_DEST, "plugin global_param:\n");
    fprintf(LOG_DEST, "\trout_steps_per_day  : %zu\n", gp->rout_steps_per_day);
    fprintf(LOG_DEST, "\trout_dt             : %.4f\n", gp->rout_dt);
}

void
plugin_print_options(plugin_option_struct *op)
{
    size_t i;
    
    fprintf(LOG_DEST, "plugin option:\n");
    fprintf(LOG_DEST, "\tDECOMPOSITION        : %d\n",
            op->DECOMPOSITION);
    fprintf(LOG_DEST, "\tROUTING              : %s\n",
            op->ROUTING ? "true" : "false");
    fprintf(LOG_DEST, "\tWATERUSE             : %s\n",
            op->WATERUSE ? "true" : "false");
    fprintf(LOG_DEST, "\tFORCE_ROUTING        : %s\n",
            op->FORCE_ROUTING ? "true" : "false");
    
    fprintf(LOG_DEST, "\tUH_LENGTH            : %d\n",
            op->UH_LENGTH);
    fprintf(LOG_DEST, "\tNWUTYPES             : %d\n",
            op->NWUTYPES);
    fprintf(LOG_DEST, "\tNWURECEIVING         : %d\n",
            op->NWURECEIVING);
    for(i = 0; i < WU_NSECTORS; i++){
        fprintf(LOG_DEST, "\tWU_INPUT             : %d\n",
                op->WU_INPUT[i]);
    }
}

void
plugin_print_parameters(plugin_parameters_struct *pa)
{
    fprintf(LOG_DEST, "plugin parameters:\n");
}