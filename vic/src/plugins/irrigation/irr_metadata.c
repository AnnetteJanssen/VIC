#include <vic.h>

void
irr_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;
    
    int                        OUT_IRR_REQUIREMENT;
    int                        OUT_IRR_NEED;
    int                        OUT_IRR_DEFICIT;
    int                        OUT_IRR_SHORTAGE;
    int                        OUT_IRR_POND_STORAGE;
    int                        OUT_IRR_LEFTOVER;
    
    OUT_IRR_REQUIREMENT = list_search_id(outvar_types, "OUT_IRR_REQUIREMENT");
    OUT_IRR_NEED = list_search_id(outvar_types, "OUT_IRR_NEED");
    OUT_IRR_DEFICIT = list_search_id(outvar_types, "OUT_IRR_DEFICIT");
    OUT_IRR_SHORTAGE = list_search_id(outvar_types, "OUT_IRR_SHORTAGE");
    OUT_IRR_POND_STORAGE = list_search_id(outvar_types, "OUT_IRR_POND_STORAGE");
    OUT_IRR_LEFTOVER = list_search_id(outvar_types, "OUT_IRR_LEFTOVER");

    strcpy(out_metadata[OUT_IRR_REQUIREMENT].varname, "OUT_IRR_REQUIREMENT");
    strcpy(out_metadata[OUT_IRR_REQUIREMENT].long_name, "irrigation_requirement");
    strcpy(out_metadata[OUT_IRR_REQUIREMENT].standard_name,
           "irrigation_requirement");
    strcpy(out_metadata[OUT_IRR_REQUIREMENT].units, "mm");
    strcpy(out_metadata[OUT_IRR_REQUIREMENT].description,
           "Current difference between soil moisture and field capacity, if soil moisture is less than irrigation point");
    
    strcpy(out_metadata[OUT_IRR_NEED].varname, "OUT_IRR_NEED");
    strcpy(out_metadata[OUT_IRR_NEED].long_name, "irrigation_need");
    strcpy(out_metadata[OUT_IRR_NEED].standard_name,
           "irrigation_need");
    strcpy(out_metadata[OUT_IRR_NEED].units, "mm");
    strcpy(out_metadata[OUT_IRR_NEED].description,
           "Added difference between soil moisture and field capacity, if soil moisture is less than irrigation point");
    
    strcpy(out_metadata[OUT_IRR_POND_STORAGE].varname, "OUT_IRR_POND_STORAGE");
    strcpy(out_metadata[OUT_IRR_POND_STORAGE].long_name, "irrigation_pond_storage");
    strcpy(out_metadata[OUT_IRR_POND_STORAGE].standard_name,
           "irrigation_pond_storage");
    strcpy(out_metadata[OUT_IRR_POND_STORAGE].units, "mm");
    strcpy(out_metadata[OUT_IRR_POND_STORAGE].description,
           "Moisture storage in irrigation pond");
    
    strcpy(out_metadata[OUT_IRR_LEFTOVER].varname, "OUT_IRR_LEFTOVER");
    strcpy(out_metadata[OUT_IRR_LEFTOVER].long_name, "irrigation_leftover_water");
    strcpy(out_metadata[OUT_IRR_LEFTOVER].standard_name,
           "irrigation_leftover_water");
    strcpy(out_metadata[OUT_IRR_LEFTOVER].units, "mm");
    strcpy(out_metadata[OUT_IRR_LEFTOVER].description,
           "Excess water leftover after irrigation up to maximum capacity");
    
    strcpy(out_metadata[OUT_IRR_SHORTAGE].varname, "OUT_IRR_SHORTAGE");
    strcpy(out_metadata[OUT_IRR_SHORTAGE].long_name, "irrigation_shortage");
    strcpy(out_metadata[OUT_IRR_SHORTAGE].standard_name,
           "irrigation_shortage");
    strcpy(out_metadata[OUT_IRR_SHORTAGE].units, "mm");
    strcpy(out_metadata[OUT_IRR_SHORTAGE].description,
           "Current difference between soil moisture and stress point, if soil moisture is less than stress point");
    
    strcpy(out_metadata[OUT_IRR_DEFICIT].varname, "OUT_IRR_DEFICIT");
    strcpy(out_metadata[OUT_IRR_DEFICIT].long_name, "irrigation_deficit");
    strcpy(out_metadata[OUT_IRR_DEFICIT].standard_name,
           "irrigation_deficit");
    strcpy(out_metadata[OUT_IRR_DEFICIT].units, "mm");
    strcpy(out_metadata[OUT_IRR_DEFICIT].description,
           "Added difference between soil moisture and stress point, if soil moisture is less than stress point");
        
    out_metadata[OUT_IRR_REQUIREMENT].nelem = 1;
    out_metadata[OUT_IRR_NEED].nelem = 1;
    out_metadata[OUT_IRR_POND_STORAGE].nelem = 1;
    out_metadata[OUT_IRR_LEFTOVER].nelem = 1;
    out_metadata[OUT_IRR_DEFICIT].nelem = 1;
    out_metadata[OUT_IRR_SHORTAGE].nelem = 1;
}

void
irr_state_metadata(void)
{  
}
