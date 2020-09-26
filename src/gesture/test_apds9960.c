#include "test_apds9960.h"

void test_ENABLE(void)
{
    NRF_LOG_INFO("__________________________________________________");
    NRF_LOG_INFO("%s:", __FUNCTION__);

    uint8_t reset = 0;
    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &reset, 1);
    NRF_LOG_INFO("__________________________________________________");

    set_power_on(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_power_on(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);

    set_als_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_als_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    
    set_proximity_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_proximity_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    
    set_wait_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_wait_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);

    set_als_interrupt_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_als_interrupt_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);

    set_proximity_interrupt_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_proximity_interrupt_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);

    set_gesture_enabled(true);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    set_gesture_enabled(false);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
}

void test_ATIME(void)
{
    NRF_LOG_INFO("__________________________________________________");
    NRF_LOG_INFO("%s:", __FUNCTION__);

    uint8_t reset = 0;
    i2c_write(APDS9960_ADDR, APDS9960_REG_ATIME, &reset, 1);
    NRF_LOG_INFO("__________________________________________________");

    set_adc_integration_time(712);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    NRF_LOG_INFO("%d", (uint16_t)get_adc_integration_time());
}

void test_WTIME(void);

void test_AILTL(void)
{
    NRF_LOG_INFO("__________________________________________________");
    NRF_LOG_INFO("%s:", __FUNCTION__);

    uint8_t reset = 0;
    i2c_write(APDS9960_ADDR, APDS9960_REG_AILTL, &reset, 1);
    NRF_LOG_INFO("__________________________________________________");

    set_als_interrupt_low_threshold(128);
    NRF_LOG_INFO("****************************************");
    nrf_delay_ms(50);
    NRF_LOG_INFO("%d", get_als_interrupt_low_threshold());
}