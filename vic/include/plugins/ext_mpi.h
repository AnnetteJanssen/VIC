#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <mpi.h>

enum {
    MPI_DECOMPOSITION_BASIN,
    MPI_DECOMPOSITION_RANDOM,
    MPI_DECOMPOSITION_FILE,
    MPI_NDECOMPOSITIONS
};

bool mpi_get_global_parameters(char *cmdstr);
void mpi_validate_global_parameters(void);

#endif
