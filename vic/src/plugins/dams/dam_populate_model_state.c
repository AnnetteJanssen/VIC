#include <vic.h>

void
dam_generate_default_state(void)
{
    extern domain_struct       local_domain;
    extern dam_con_struct     *local_dam_con;
    extern dam_var_struct     *local_dam_var;
    extern dam_con_struct     *global_dam_con;
    extern dam_var_struct     *global_dam_var;
    extern dmy_struct         *dmy;
    extern size_t              current;
    extern parameters_struct   param;

    size_t                     i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_var[i].storage = local_dam_con[i].capacity * param.DAM_ALPHA;
        local_dam_var[i].op_year = dmy[current].month;
        global_dam_var[i].storage = global_dam_con[i].capacity * param.DAM_ALPHA;
        global_dam_var[i].op_year = dmy[current].month;
    }
}
