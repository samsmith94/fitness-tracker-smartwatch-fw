#include "../driver/i2c.h"

#define CRC_polynomial                              0x07

#define LC709203_ADDR								(0x16)	/* 0001011 */

#define LC709203_REG_BEFORE_RSOC					0x04
#define LC709203_REG_THERMISTOR_B					0x06
#define LC709203_REG_INITIAL_RSOC					0x07
#define LC709203_REG_CELL_TEMPERATURE				0x08
#define LC709203_REG_CELL_VOLTAGE					0x09
#define LC709203_REG_CURRENT_DIRECTION				0x0A
#define LC709203_REG_ADJUSTMENT_PACK_APPLICATION	0x0B
#define LC709203_REG_ADJUSTMENT_PACK_THERMISTOR		0x0C
#define LC709203_REG_RSOC							0x0D
#define LC709203_REG_INDICATOR_OF_EMPTY				0x0F
#define LC709203_REG_IC_VERSION						0x11
#define LC709203_REG_CHANGE_OF_THE_PARAMETER		0x12
#define LC709203_REG_ALARM_LOW_RSOC					0x13
#define LC709203_REG_ALARM_LOW_CELL_VOLTAGE			0x14
#define LC709203_REG_IC_POWER_MODE					0x15
#define LC709203_REG_STATUS_BIT						0x16
#define LC709203_REG_NUMBER_OF_THE_PARAMETER		0x1A

void lc709203_set_before_RSOC(void);

uint16_t lc709203_get_thermistor_B(void);
void lc709203_set_thermistor_B(uint16_t B_constant_K);

void lc709203_set_initial_RSOC(void);

uint16_t lc709203_get_cell_temperature(void);
void lc709203_set_cell_temperature(uint16_t cell_temperature_0_1K);

uint16_t lc709203_get_cell_voltage(void)
{
    //uint16_t cell_voltage_mV_units...
}

typedef enum {
    LC709203_AUTO_MODE,
    LC709203_CHARGE_MODE,
    LC709203_DISCHARGE_MODE
} lc709203_current_direction_t;

lc709203_current_direction_t lc709203_get_current_direction(void);
void lc709203_set_current_direction(lc709203_current_direction_t);

/******************************************************************************/

//ez uint8_t igazából:
/*
lc709203_get_adjustment_pack_application();
lc709203_set_adjustment_pack_application();
*/

uint16_t lc709203_get_adjustment_pack_thermistor(void);
void lc709203_set_adjustment_pack_thermistor(uint16_t);

uint8_t lc709203_get_RSOC(void);
void lc709203_set_RSOC(uint8_t percent);

uint16_t lc709203_get_indicator_to_empty(void);

uint16_t lc709203_get_IC_version(void);

/*
TODO: paraméterek típusai, visszatérési értékei...
lc709203_set_change_of_the_parameter();
lc709203_get_change_of_the_parameter();

lc709203_get_alarm_low_RSOC();
lc709203_set_alarm_low_RSOC();

lc709203_get_alarm_low_cell_voltage();
lc709203_set_alarm_low_cell_voltage();

lc709203_set_IC_power_mode();
lc709203_get_IC_power_mode();

lc709203_set_status_bit();
lc709203_get_status_bit();

lc709203_get_number_of_the_parameter();
*/