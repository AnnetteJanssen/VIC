#include <vic.h>

void
initialize_rout_hist(rout_hist_struct *rout_hist)
{
    rout_hist->discharge = 0.0;
}

void
initialize_rout_force(rout_force_struct *rout_force)
{
    extern size_t NF;

    size_t               i;

    for (i = 0; i < NF; i++) {
        rout_force->discharge[i] = 0.0;
    }
}

void
initialize_rout_var(rout_var_struct *rout_var)
{
    extern option_struct options;

    size_t               i;

    rout_var->moist = 0.0;
    for (i = 0; i < options.IUH_NSTEPS; i++) {
        rout_var->discharge[i] = 0.0;
    }
}

void
initialize_rout_con(rout_con_struct *rout_con)
{
    extern option_struct options;

    size_t               i;

    rout_con->Nupstream = 0;
    rout_con->downstream = MISSING_USI;

    rout_con->upstream = NULL;

    for (i = 0; i < options.IUH_NSTEPS; i++) {
        rout_con->inflow_uh[i] = 0.0;
    }
    for (i = 0; i < options.RUH_NSTEPS; i++) {
        rout_con->runoff_uh[i] = 0.0;
    }
}

void
initialize_rout_local_structures(void)
{
    extern domain_struct    local_domain;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern rout_hist_struct *rout_hist;
    extern rout_force_struct *rout_force;

    size_t                  i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_rout_con(&rout_con[i]);
        initialize_rout_var(&(rout_var[i]));
        initialize_rout_hist(&(rout_hist[i]));
        initialize_rout_force(&(rout_force[i]));
    }
}
