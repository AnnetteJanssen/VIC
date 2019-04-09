#include <stddef.h>

double
array_average(double *array,
              size_t  repetitions,
              size_t  length,
              size_t  offset,
              size_t  skip)
{
    size_t i;
    size_t j;

    double average = 0.0;
    
    if(repetitions == 0 || length == 0){
        return average;
    }

    for (i = 0; i < repetitions; i++) {
        for (j = 0; j < length; j++) {
            average += array[offset + i * (length + skip) + j]; 
        }
    }
    average /= (repetitions * length);

    return average;
}
