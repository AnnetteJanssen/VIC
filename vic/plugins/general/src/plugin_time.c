#include <vic_driver_image.h>

/******************************************************************************
 * @brief  returns if in the defined season
 *****************************************************************************/
bool
between_dmy(dmy_struct start,
            dmy_struct end,
            dmy_struct current)
{
    if ((start.day_in_year < end.day_in_year) ||
        (start.day_in_year == end.day_in_year && start.dayseconds <
         end.dayseconds)) {
        if ((current.day_in_year > end.day_in_year) ||
            (current.day_in_year == end.day_in_year && current.dayseconds >
             end.dayseconds)) {
            return false;
        }
        else if ((current.day_in_year > start.day_in_year) ||
                 (current.day_in_year == start.day_in_year &&
                  current.dayseconds > start.dayseconds)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if ((current.day_in_year > start.day_in_year) ||
            (current.day_in_year == start.day_in_year && current.dayseconds >
             start.dayseconds)) {
            return true;
        }
        else if ((current.day_in_year > end.day_in_year) ||
                 (current.day_in_year == end.day_in_year &&
                  current.dayseconds > end.dayseconds)) {
            return false;
        }
        else {
            return true;
        }
    }
}

/******************************************************************************
 * @brief  returns the day in the defined season,
 * or the number of days past the season
 *****************************************************************************/
double
between_jday(double start,
             double end,
             double current)
{
    if (start < end) {
        if (current > end) {
            return end - current;
        }
        else if (current > start) {
            return current - start;
        }
        else {
            return current - start;
        }
    }
    else {
        if (current > start) {
            return current - start;
        }
        else if (current > end) {
            return end - current;
        }
        else {
            return ((DAYS_PER_JYEAR - start) + current);
        }
    }
}

/******************************************************************************
 * @brief  returns the number of days in a month
 *****************************************************************************/
unsigned short int
days_per_month(unsigned short int month,
               unsigned short int year,
                unsigned short int calendar)
{
    unsigned short int days_per_month[MONTHS_PER_YEAR] = {31,28,31,30,31,30,31,31,30,31,30,31};
    
    days_per_month[2] += leap_year(year, calendar);
    
    return(days_per_month[month - 1]);
}