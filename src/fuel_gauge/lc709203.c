#include "lc709203.h"

static uint8_t check_crc(uint8_t *rec_values, uint8_t len)
{
    uint8_t crc = 0x00;    
    uint8_t current_byte;    
    uint8_t bit;    
    for (current_byte = 0 ; current_byte < len ; current_byte++)  {
        crc ^= (rec_values[current_byte]);    
        for (bit = 8 ; bit > 0 ;bit--)  {
            if (crc & 0x80)  {
                crc = (crc << 1) ^ CRC_polynomial;                      
            } else {
                crc = (crc << 1);    
            }
        }
    }
    return crc;    
}

void lc709203_set_before_RSOC(void)
{
    static uint8_t wr_buf[3];

    wr_buf[0] = 0x55;
    wr_buf[1] = 0xAA;
    wr_buf[2] = check_crc(wr_buf, 3);

    i2c_write(LC709203_ADDR, LC709203_REG_BEFORE_RSOC, wr_buf, 3);
}

uint16_t lc709203_get_thermistor_B(void)
{
    //a read-nek hogy kéne kinéznie?...
}