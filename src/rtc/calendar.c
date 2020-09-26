#include "calendar.h"

static struct tm time_struct, m_tm_return_time; 
static time_t m_time, m_last_calibrate_time = 0;
static float m_calibrate_factor = 0.0f;
static uint32_t m_rtc_increment = 60;
static void (*cal_event_callback)(void) = 0;

struct tm *_time;
char time_buff[50] = "";
char date_buff[50] = "";
char *week_days[] = {"Mon, Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

void nrf_cal_init(void)
{
    // Select the 32 kHz crystal and start the 32 kHz clock
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    
    // Configure the RTC for 1 minute wakeup (default)
    CAL_RTC->PRESCALER = 0xFFF;
    CAL_RTC->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
    CAL_RTC->INTENSET = RTC_INTENSET_COMPARE0_Msk;
    CAL_RTC->CC[0] = m_rtc_increment * 8;
    CAL_RTC->TASKS_START = 1;
    NVIC_SetPriority(CAL_RTC_IRQn, CAL_RTC_IRQ_Priority);
    NVIC_EnableIRQ(CAL_RTC_IRQn);  
}

void nrf_cal_set_callback(void (*callback)(void), uint32_t interval)
{
    // Set the calendar callback, and set the callback interval in seconds
    cal_event_callback = callback;
    m_rtc_increment = interval;
    m_time += CAL_RTC->COUNTER / 8;
    CAL_RTC->TASKS_CLEAR = 1;
    CAL_RTC->CC[0] = interval * 8;  
}
 
void nrf_cal_set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
    static time_t uncal_difftime, difftime, newtime;
    time_struct.tm_year = year - 1900;
    time_struct.tm_mon = month;
    time_struct.tm_mday = day;
    time_struct.tm_hour = hour;
    time_struct.tm_min = minute;
    time_struct.tm_sec = second;   
    newtime = mktime(&time_struct);
    CAL_RTC->TASKS_CLEAR = 1;  
    
    // Calculate the calibration offset 
    if(m_last_calibrate_time != 0)
    {
        difftime = newtime - m_last_calibrate_time;
        uncal_difftime = m_time - m_last_calibrate_time;
        m_calibrate_factor = (float)difftime / (float)uncal_difftime;
    }
    
    // Assign the new time to the local time variables
    m_time = m_last_calibrate_time = newtime;
}    

struct tm *nrf_cal_get_time(void)
{
    time_t return_time;
    return_time = m_time + CAL_RTC->COUNTER / 8;
    m_tm_return_time = *localtime(&return_time);
    return &m_tm_return_time;
}

struct tm *nrf_cal_get_time_calibrated(void)
{
    time_t uncalibrated_time, calibrated_time;
    if(m_calibrate_factor != 0.0f)
    {
        uncalibrated_time = m_time + CAL_RTC->COUNTER / 8;
        calibrated_time = m_last_calibrate_time + (time_t)((float)(uncalibrated_time - m_last_calibrate_time) * m_calibrate_factor + 0.5f);
        m_tm_return_time = *localtime(&calibrated_time);
        return &m_tm_return_time;
    }
    else return nrf_cal_get_time();
}

char *nrf_cal_get_time_string(bool calibrated)
{
    static char cal_string[80];
    strftime(cal_string, 80, "%x - %H:%M:%S", (calibrated ? nrf_cal_get_time_calibrated() : nrf_cal_get_time()));
    return cal_string;
}
 
void CAL_RTC_IRQHandler(void)
{
    if(CAL_RTC->EVENTS_COMPARE[0])
    {
        CAL_RTC->EVENTS_COMPARE[0] = 0;
        
        CAL_RTC->TASKS_CLEAR = 1;
        
        m_time += m_rtc_increment;
        if(cal_event_callback) cal_event_callback();
    }
}

static bool run_time_updates = false;

void print_current_time(void)
{
    NRF_LOG_INFO("Uncalibrated time:\t%s\r\n", nrf_cal_get_time_string(false));
    NRF_LOG_INFO("Calibrated time:\t%s\r\n", nrf_cal_get_time_string(true));
}

void calendar_updated(void)
{
    if(run_time_updates)
    {
        print_current_time();
    }
}

void set_date_and_time(void)
{
	struct tm tm = { 0 };

	char time_and_date_str[21] = { 0 };
	strncpy(time_and_date_str, __DATE__, strlen(__DATE__));
	time_and_date_str[strlen(__DATE__)] = ' ';
	strncpy(&time_and_date_str[strlen(__DATE__) + 1], __TIME__, strlen(__TIME__));

	char *s = strptime(time_and_date_str, "%b %d %Y %H:%M:%S", &tm);
	if (s == NULL) {
		NRF_LOG_INFO("Cannot parse date.\n");
	}

    nrf_cal_set_time(tm.tm_year+2000, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void calendar_init(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    nrf_cal_init();
    nrf_cal_set_callback(calendar_updated, 4);

    set_date_and_time();
}