#include <vic_driver_image.h>
#include <plugin.h>

// Calculates dam release similar to Hanasaki et al. 2006
// Adjusted to account for varying environmental flow requirements
// Make sure all inputs have the same units & length
void
dam_calc_opt_release(double *inflow,
                     double *demand,
                     double *efr,
                     double *release,
                     size_t  length)
{
    double inflow_avg;
    double demand_avg;
    
    size_t i;
    
    // Average and remove environmental flow requirements
    inflow_avg = 0.0;
    demand_avg = 0.0;
    for (i = 0; i < length; i++) {
        inflow_avg += inflow[i] - efr[i];
        demand_avg += demand[i];
    }
    inflow_avg /= length;
    demand_avg /= length;
    
    if (inflow_avg < 0) {
        inflow_avg = 0;
    }
    
    if (demand_avg <= 0) {
        // No water demand, reduce annual release fluctuations
        for (i = 0; i < length; i++) {
            release[i] = inflow_avg;
        }
    } else {
        if (demand_avg > inflow_avg) {
            // High water demand, supply only partially
            for (i = 0; i < length; i++) {
                release[i] = inflow_avg * (demand[i] / demand_avg);
            }
        } else {
            // Low water demand, supply all
            for (i = 0; i < length; i++) {
                release[i] = inflow_avg - demand_avg + demand[i];
            }
        }
    }
    
    // Add environmental flow requirements
    for (i = 0; i < length; i++) {
        release[i] += efr[i];
    }
}

// Calculates dam release according to Hanasaki et al. 2006
// Dam release is adjusted based on relative dam capacity
// Make sure all inputs have the same units & length
void
dam_corr_opt_release(double *inflow,
                     double *release,
                     size_t  length,
                     double  k,
                     double  c)
{
    size_t  i;
    
    if (c >= 0.5) {
        // Large dam capacity, release all
        for (i = 0; i < length; i++) {
            release[i] *= k;
        }
    } else {
        // Small dam capacity, release fraction
        for (i = 0; i < length; i++) {
            release[i] = pow(c / 0.5, 2) * k * release[i] + 
                    (1 - pow(c / 0.5, 2)) * inflow[i];
        }
    }
}

// Calculates release correction similar to van Beek et al. 2011
// This factor reduces release based on deviations from optimal storage
double
dam_corr_release(double release,
                 double cur_storage,
                 double opt_storage)
{
    return(max(0, release + (cur_storage - opt_storage) / 7));
}

// Calculates dam Kr factor according to Hanasaki et al. 2006
// This factor handles inter-annual variability in storage
// Make sure all inputs have the same units & length
double
dam_calc_k_factor(double max_capacity,
                  double cur_storage){
    extern plugin_parameters_struct plugin_param;
    
    return(cur_storage / (max_capacity * plugin_param.DAM_ALPHA));
}

// Calculates dam c factor according to Hanasaki et al. 2006
// This factor handles the relative capacity of the dam
// Make sure all inputs have the same units & length
double
dam_calc_c_factor(double *inflow,
                  double  max_capacity,
                  size_t  length)
{
    double inflow_tot;
    
    size_t i;
    
    inflow_tot = 0.0;
    for (i = 0; i < length; i++) {
        inflow_tot += inflow[i];
    }
    
    return(max_capacity / inflow_tot);
}

void
dam_calc_opt_storage(double *inflow,
                     double *release,
                     double *storage,
                     size_t  length,
                     double  cur_storage)
{
    double difference;
    
    size_t i;
    
    for (i = 0; i < length; i++) {
        difference = inflow[i] - release[i];
        
        if (i == 1) {
            storage[i] = cur_storage + difference;
        } else {
            storage[i] = storage[i - 1] + difference;
        }
    }
}

// Calculate surface area based on Kaveh et al 2013
double
dam_area(double volume, double max_volume, double max_area, double max_height){
    double N = ( 2 * max_volume ) / ( max_height * max_area );
    double area = max_area * pow(( volume / max_volume ), ((2-N)/N));
    
    if(area > max_area){
        area=max_area;
    }
    
    return area;        
}

// Calculate dam height based on Liebe et al 2005
double
dam_height(double area, double max_height){
    double height = sqrt(area) * (1 / 19.45);
    
    if(height > max_height){
        height = max_height;
    }
    
    return height;
}
