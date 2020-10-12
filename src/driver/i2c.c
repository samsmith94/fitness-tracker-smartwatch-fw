#include "../driver/i2c.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

//#define DEBUG_I2C

static uint8_t i2c_write_buf[2];

void i2c_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
        .scl = NRF_GPIO_PIN_MAP(0, 20),
        .sda = NRF_GPIO_PIN_MAP(0, 19),
        //.scl = NRF_GPIO_PIN_MAP(0, 7),
        //.sda = NRF_GPIO_PIN_MAP(1, 0),
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false};

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    //err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

void i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
    ret_code_t ret;

    ret = nrf_drv_twi_tx(&m_twi, slave_addr, (uint8_t *)&reg_addr, 1, false);
    if (NRF_SUCCESS != ret)
    {
        return;
    }

    //nrf_delay_ms(1);

    ret = nrf_drv_twi_rx(&m_twi, slave_addr, buff, size);

    APP_ERROR_CHECK(ret);

#ifdef DEBUG_I2C
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (buff[0] & 0x80 ? '1' : '0'), (buff[0] & 0x40 ? '1' : '0'), (buff[0] & 0x20 ? '1' : '0'), (buff[0] & 0x10 ? '1' : '0'), (buff[0] & 0x08 ? '1' : '0'), (buff[0] & 0x04 ? '1' : '0'), (buff[0] & 0x02 ? '1' : '0'), (buff[0] & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("I2C  READ(0x%02X): %s", reg_addr, bin);
    //NRF_LOG_INFO("****************************************");
#endif
}

void i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
    ret_code_t ret;

    i2c_write_buf[0] = reg_addr;
    i2c_write_buf[1] = buff[0];
    ret = nrf_drv_twi_tx(&m_twi, slave_addr, i2c_write_buf, 2, false);
    
/*
    ret = nrf_drv_twi_tx(&m_twi, slave_addr, (uint8_t *)&reg_addr, 1, true);
    if (NRF_SUCCESS != ret)
    {
        return;
    }
*/
    //nrf_delay_ms(1);

    //ret = nrf_drv_twi_tx(&m_twi, slave_addr, buff, size, false);
    APP_ERROR_CHECK(ret);

#ifdef DEBUG_I2C
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (buff[0] & 0x80 ? '1' : '0'), (buff[0] & 0x40 ? '1' : '0'), (buff[0] & 0x20 ? '1' : '0'), (buff[0] & 0x10 ? '1' : '0'), (buff[0] & 0x08 ? '1' : '0'), (buff[0] & 0x04 ? '1' : '0'), (buff[0] & 0x02 ? '1' : '0'), (buff[0] & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("I2C WRITE(0x%02X): %s", reg_addr, bin);
    //NRF_LOG_INFO("****************************************");
#endif
}

void i2c_send(uint8_t slave_addr, uint8_t reg_addr, uint8_t value)
{
    ret_code_t ret;

    i2c_write_buf[0] = reg_addr;
    i2c_write_buf[1] = value;

    ret = nrf_drv_twi_tx(&m_twi, slave_addr, i2c_write_buf, 2, false);
    APP_ERROR_CHECK(ret);
}


//APDS9960_ADDR