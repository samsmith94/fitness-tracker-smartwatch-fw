#ifndef CALENDAR_H
#define CALENDAR_H


#include <stdint.h>
#include <stdbool.h>
#include "time.h"

#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Change the following defines to change the RTC timer used or the interrupt priority
#define CAL_RTC                 NRF_RTC0
#define CAL_RTC_IRQn            RTC0_IRQn
#define CAL_RTC_IRQHandler      RTC0_IRQHandler
#define CAL_RTC_IRQ_Priority    3

extern struct tm *_time;
extern char time_buff[50];
extern char date_buff[50];
extern char *week_days[7];


// Initializes the calendar library. Run this before calling any other functions. 
void nrf_cal_init(void);

// Enables a callback feature in the calendar library that can call a function automatically at the specified interval (seconds).
void nrf_cal_set_callback(void (*callback)(void), uint32_t interval);

// Sets the date and time stored in the calendar library. 
// When this function is called a second time calibration data will be automatically generated based on the error in time since the
// last call to the set time function. To ensure good calibration this function should not be called too often 
// (depending on the accuracy of the 32 kHz clock it should be sufficient to call it between once a week and once a month). 
void nrf_cal_set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second);

// Returns the uncalibrated time as a tm struct. For more information about the tm struct and the time.h library in general please refer to:
// http://www.tutorialspoint.com/c_standard_library/time_h.htm
struct tm *nrf_cal_get_time(void);

// Returns the calibrated time as a tm struct. If no calibration data is available it will return the uncalibrated time.
struct tm *nrf_cal_get_time_calibrated(void);

// Returns a string for printing the date and time. Turn the calibration on/off by setting the calibrate parameter. 
char *nrf_cal_get_time_string(bool calibrated);



void print_current_time(void);
void calendar_updated(void);

void set_date_and_time(void);


void calendar_init(void);

#endif