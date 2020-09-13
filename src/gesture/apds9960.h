#ifndef APDS9960_H
#define APDS9960_H

#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"

#include "../driver/i2c.h"

/******************************************************************************/
#define BIT_SET(reg,pos) ((reg)|=(1<<pos))
#define BIT_CLEAR(reg,pos) ((reg)&=~(1<<(pos)))
#define BIT_CHECK(reg,pos) (!!((reg)&(1<<(pos))))

#define MULTIBIT_SET(config,mask,value) ((config)=(((config)&((0xff)^(mask)))|((value)&(mask))))
//#define MULTIBIT_GET(config,mask,shift) (config=(config&=mask)>>shift)
#define MULTIBIT_GET(config,mask) (config=(config&=mask))
/******************************************************************************/

/* Slave Address **************************************************************/
#define APDS9960_ADDR			(0x39)

/* Register Addresses *********************************************************/
#define APDS9960_REG_ENABLE     (0x80)
#define APDS9960_REG_ATIME      (0x81)
#define APDS9960_REG_WTIME      (0x83)
#define APDS9960_REG_AILTL      (0x84)
#define APDS9960_REG_AILTH      (0x85)
#define APDS9960_REG_AIHTL      (0x86)
#define APDS9960_REG_AIHTH      (0x87)
#define APDS9960_REG_PILT       (0x89)
#define APDS9960_REG_PIHT       (0x8B)
#define APDS9960_REG_PERS       (0x8C)
#define APDS9960_REG_CONFIG1    (0x8D)		//TODO: ITT KELL LENNIE EGY LOWPOW bitnek is de nem látom
#define APDS9960_REG_PPULSE     (0x8E)
#define APDS9960_REG_CONTROL    (0x8F)
#define APDS9960_REG_CONFIG2    (0x90)
#define APDS9960_REG_ID         (0x92)
#define APDS9960_REG_STATUS     (0x93)
#define APDS9960_REG_CDATAL     (0x94)
#define APDS9960_REG_CDATAH     (0x95)
#define APDS9960_REG_RDATAL     (0x96)
#define APDS9960_REG_RDATAH     (0x97)
#define APDS9960_REG_GDATAL     (0x98)
#define APDS9960_REG_GDATAH     (0x99)
#define APDS9960_REG_BDATAL     (0x9A)
#define APDS9960_REG_BDATAH     (0x9B)
#define APDS9960_REG_PDATA      (0x9C)
#define APDS9960_REG_POFFSET_UR (0x9D)
#define APDS9960_REG_POFFSET_DL (0x9E)
#define APDS9960_REG_CONFIG3    (0x9F)
#define APDS9960_REG_GPENTH		(0xA0)
#define APDS9960_REG_GEXTH		(0xA1)
#define APDS9960_REG_GCONFIG1	(0xA2)
#define APDS9960_REG_GCONFIG2	(0xA3)
#define APDS9960_REG_GOFFSET_U	(0xA4)
#define APDS9960_REG_GOFFSET_D	(0xA5)
#define APDS9960_REG_GOFFSET_L	(0xA7)
#define APDS9960_REG_GOFFSET_R	(0xA9)
#define APDS9960_REG_GPULSE		(0xA6)
#define APDS9960_REG_GCONFIG3	(0xAA)
#define APDS9960_REG_GCONFIG4	(0xAB)
#define APDS9960_REG_GFLVL		(0xAE)
#define APDS9960_REG_GSTATUS	(0xAF)
#define APDS9960_REG_IFORCE     (0XE4)
#define APDS9960_REG_PICLEAR    (0xE5)
#define APDS9960_REG_CICLEAR    (0xE6)
#define APDS9960_REG_AICLEAR    (0xE7)
#define APDS9960_REG_GFIFO_U	(0xFC)
#define APDS9960_REG_GFIFO_D	(0xFD)
#define APDS9960_REG_GFIFO_L	(0xFE)
#define APDS9960_REG_GFIFO_R	(0xFF)

/* Bit Positions **************************************************************/
/* APDS9960_REG_ENABLE */
#define APDS9960_GEN			6
#define APDS9960_PIEN	        5
#define APDS9960_AIEN			4
#define APDS9960_WEN			3
#define APDS9960_PEN	        2
#define APDS9960_AEN			1
#define APDS9960_PON            0
/* APDS9960_REG_PERS */
#define APDS9960_PPERS      	4
#define APDS9960_APERS			0
/* APDS9960_REG_CONFIG1 */
#define APDS9960_WLONG			0
/* APDS9960_REG_PPULSE */
#define APDS9960_PPLEN	        6
#define APDS9960_PPULSE			0
/* APDS9960_REG_CONTROL */
#define APDS9960_LDRIVE	        6
#define APDS9960_PGAIN	        2
#define APDS9960_AGAIN			0
/* APDS9960_REG_CONFIG2 */
#define APDS9960_PSIEN	        7
#define APDS9960_CPSIEN			6
#define APDS9960_LEDBOOST		4
/* APDS9960_REG_STATUS */
#define APDS9960_CPSAT	        7
#define APDS9960_PGSAT	        6
#define APDS9960_PINT	        5
#define APDS9960_AINT			4
#define APDS9960_GINT			2
#define APDS9960_PVALID	        1
#define APDS9960_AVALID			0
/* APDS9960_REG_CONFIG3 */
#define APDS9960_PCMP	        5
#define APDS9960_PMSK_U			3
#define APDS9960_PMSK_D			2
#define APDS9960_PMSK_L			1
#define APDS9960_PMSK_R			0
/* APDS9960_REG_GCONFIG1 */
#define APDS9960_GFIFOTH		6
#define APDS9960_GEXMSK			2
#define APDS9960_GEXPERS		0
/* APDS9960_REG_GCONFIG2 */
#define APDS9960_GGAIN			5
#define APDS9960_GLDRIVE		3
#define APDS9960_GWTIME			0
/* APDS9960_REG_GPULSE */
#define APDS9960_GPLEN			6
#define APDS9960_GPULSE			0
/* APDS9960_REG_GCONFIG3 */
#define APDS9960_GDIMS			0
/* APDS9960_REG_GCONFIG4 */
#define APDS9960_GIEN			1
#define APDS9960_GMODE			0
/* APDS9960_REG_GSTATUS */
#define APDS9960_GFOV			1
#define APDS9960_GVALID			0

/* Proximity Control Bit Masks ************************************************/
/* APDS9960_REG_PERS */
#define APDS9960_PPERS_MASK     0b11110000
#define APDS9960_APERS_MASK		0b00001111

/* APDS9960_REG_PPULSE */
#define APDS9960_PPULSE_MASK    0b00111111
#define APDS9960_PPLEN_MASK     0b11000000

/* APDS9960_REG_CONTROL */
#define APDS9960_LDRIVE_MASK    0b11000000
#define APDS9960_PGAIN_MASK     0b00001100
#define APDS9960_AGAIN_MASK		0b00000011

/* APDS9960_REG_CONFIG2 */
#define APDS9960_LEDBOOST_MASK	0b00110000

/* APDS9960_REG_CONFIG3 */
//#define APDS9960_PMSK_MASK	0b00001111

/* APDS9960_REG_GCONFIG1 */
#define APDS9960_GFIFOTH_MASK	0b11000000
#define APDS9960_GEXMSK_MASK	0b00111100
#define APDS9960_GEXPERS_MASK	0b00000011

/* APDS9960_REG_GCONFIG2 */
#define APDS9960_GGAIN_MASK		0b01100000
#define APDS9960_GLDRIVE_MASK	0b00011000
#define APDS9960_GWTIME_MASK	0b00000111

/* APDS9960_REG_GPULSE */
#define APDS9960_GPLEN_MASK		0b11000000
#define APDS9960_GPULSE_MASK	0b00111111

/* APDS9960_REG_GCONFIG3 */
#define APDS9960_GDIMS_MASK		0b00000011

/* Enums **********************************************************************/
typedef enum {
    APDS9960_EVERY_PROX_CYCLE = 0 << APDS9960_PPERS,
	APDS9960_ANY_PROX_VALUE_OUT_OF_THRES_RANGE = 1 << APDS9960_PPERS,
	APDS9960_2_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 2 << APDS9960_PPERS,
	APDS9960_3_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 3 << APDS9960_PPERS,
	APDS9960_4_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 4 << APDS9960_PPERS,
	APDS9960_5_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 5 << APDS9960_PPERS,
	APDS9960_6_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 6 << APDS9960_PPERS,
	APDS9960_7_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 7 << APDS9960_PPERS,
	APDS9960_8_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 8 << APDS9960_PPERS,
	APDS9960_9_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 9 << APDS9960_PPERS,
	APDS9960_10_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 10 << APDS9960_PPERS,
	APDS9960_11_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 11 << APDS9960_PPERS,
	APDS9960_12_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 12 << APDS9960_PPERS,
	APDS9960_13_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 13 << APDS9960_PPERS,
	APDS9960_14_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 14 << APDS9960_PPERS,
	APDS9960_15_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE = 15 << APDS9960_PPERS
} apds9960_ppers_t;

typedef enum {
	APDS9960_PULSE_LENGTH_4_US = 0 << APDS9960_PPLEN,
	APDS9960_PULSE_LENGTH_8_US = 1 << APDS9960_PPLEN,
	APDS9960_PULSE_LENGTH_16_US = 2 << APDS9960_PPLEN,
	APDS9960_PULSE_LENGTH_32_US = 3 << APDS9960_PPLEN
} apds9960_pplen_t;

typedef enum {
	APDS9960_GLED_CURRENT_100_MA = 0 << APDS9960_LDRIVE,
	APDS9960_GLED_CURRENT_50_MA = 1 << APDS9960_LDRIVE,
	APDS9960_GLED_CURRENT_25_MA = 2 << APDS9960_LDRIVE,
	APDS9960_GLED_CURRENT_12_5_MA = 3 << APDS9960_LDRIVE,
} apds9960_ldrive_t;

typedef enum {
	APDS9960_P_GAIN_1X = 0 << APDS9960_PGAIN,
	APDS9960_P_GAIN_2X = 1 << APDS9960_PGAIN,
	APDS9960_P_GAIN_4X = 2 << APDS9960_PGAIN,
	APDS9960_P_GAIN_8X = 3 << APDS9960_PGAIN
} apds9960_pgain_t;

typedef enum {
	APDS9960_A_GAIN_1X = 0 << APDS9960_AGAIN,
	APDS9960_A_GAIN_4X = 1 << APDS9960_AGAIN,
	APDS9960_A_GAIN_16X = 2 << APDS9960_AGAIN,
	APDS9960_A_GAIN_64X = 3 << APDS9960_AGAIN
} apds9960_again_t;

typedef enum {
	APDS9960_LED_BOOST_CURRENT_100_PERCENT = 0 << APDS9960_LEDBOOST,
	APDS9960_LED_BOOST_CURRENT_150_PERCENT = 1 << APDS9960_LEDBOOST,
	APDS9960_LED_BOOST_CURRENT_200_PERCENT = 2 << APDS9960_LEDBOOST,
	APDS9960_LED_BOOST_CURRENT_300_PERCENT = 3 << APDS9960_LEDBOOST
} apds9960_ledboost_t;

typedef enum {
	APDS9960_INTERRUPT_AFTER_1_DATASET_ADDED_TO_FIFO = 0 << APDS9960_GFIFOTH,
	APDS9960_INTERRUPT_AFTER_4_DATASET_ADDED_TO_FIFO = 1 << APDS9960_GFIFOTH,
	APDS9960_INTERRUPT_AFTER_8_DATASET_ADDED_TO_FIFO = 2 << APDS9960_GFIFOTH,
	APDS9960_INTERRUPT_AFTER_16_DATASET_ADDED_TO_FIFO = 3 << APDS9960_GFIFOTH
} apds9960_gfifoth_t;

typedef enum {
	APDS9960_ALL_DATA_INCLUDED_IN_SUM = 0b0000 << APDS9960_GEXMSK,
	APDS9960_R_DATA_NOT_INCLUDED_IN_SUM = 0b0001 << APDS9960_GEXMSK,
	APDS9960_L_DATA_NOT_INCLUDED_IN_SUM = 0b0010 << APDS9960_GEXMSK,
	APDS9960_D_DATA_NOT_INCLUDED_IN_SUM = 0b0100 << APDS9960_GEXMSK,
	APDS9960_U_DATA_NOT_INCLUDED_IN_SUM = 0b1000 << APDS9960_GEXMSK,
	APDS9960_L_AND_D_DATA_NOT_INCLUDED_IN_SUM = 0b0110 << APDS9960_GEXMSK,
	//????????????
	APDS9960_ALL_DATA_NOT_INCLUDED_IN_SUM = 0b1111 << APDS9960_GEXMSK
} apds9960_gexmsk_t;

typedef enum {
	APDS9960_EXIT_STATE_MACHINE_AFTER_1ST_GESTURE_END = 0 << APDS9960_GEXPERS,
	APDS9960_EXIT_STATE_MACHINE_AFTER_2ND_GESTURE_END = 1 << APDS9960_GEXPERS,
	APDS9960_EXIT_STATE_MACHINE_AFTER_4TH_GESTURE_END = 2 << APDS9960_GEXPERS,
	APDS9960_EXIT_STATE_MACHINE_AFTER_7TH_GESTURE_END = 3 << APDS9960_GEXPERS
} apds9960_gexpers_t;

typedef enum {
	APDS9960_G_GAIN_1X = 0 << APDS9960_GGAIN,
	APDS9960_G_GAIN_2X = 1 << APDS9960_GGAIN,
	APDS9960_G_GAIN_4X = 2 << APDS9960_GGAIN,
	APDS9960_G_GAIN_8X = 3 << APDS9960_GGAIN
} apds9960_ggain_t;

typedef enum {
	APDS9960_LED_CURRENT_100_MA = 0 << APDS9960_GLDRIVE,
	APDS9960_LED_CURRENT_50_MA = 1 << APDS9960_GLDRIVE,
	APDS9960_LED_CURRENT_25_MA = 2 << APDS9960_GLDRIVE,
	APDS9960_LED_CURRENT_12_5_MA = 3 << APDS9960_GLDRIVE
} apds9960_gldrive_t;

typedef enum {
	APDS9960_G_WAIT_TIME_0_MS = 0 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_2_8_MS = 1 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_5_6_MS = 2 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_8_4_MS = 3 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_14_MS = 4 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_22_4_MS = 5 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_30_8_MS = 6 << APDS9960_GWTIME,
	APDS9960_G_WAIT_TIME_39_2_MS = 7 << APDS9960_GWTIME
} apds9960_gwtime_t;

typedef enum {
	APDS9960_G_PULSE_LENGTH_4_US = 0 << APDS9960_GPLEN,
	APDS9960_G_PULSE_LENGTH_8_US = 1 << APDS9960_GPLEN,
	APDS9960_G_PULSE_LENGTH_16_US = 2 << APDS9960_GPLEN,
	APDS9960_G_PULSE_LENGTH_32_US = 3 << APDS9960_GPLEN
} apds9960_gplen_t;

typedef enum {
	APDS9960_BOTH_PAIRS_ACTICE = 0 << APDS9960_GDIMS,
	APDS9960_UP_DOWN_ACTICE = 1 << APDS9960_GDIMS,
	APDS9960_LEFT_RIGHT_ACTICE = 2 << APDS9960_GDIMS
} apds9960_gdims_t;


/******************************************************************************/

typedef union {
    struct {
        uint8_t up;
        uint8_t down;
        uint8_t left;
        uint8_t right;
    };
    uint8_t gesture_dataset[4];
} gesture_fifo_t;


typedef union {
	//jó a sorrend??? struktúrán belül? 
	struct {
		uint16_t cdata;
		uint16_t rdata;
		uint16_t gdata;
		uint16_t bdata;
	};
	uint8_t crgb[8];
} crgb_data_t;


/* Functions ******************************************************************/
/* APDS9960_REG_ENABLE */
void set_power_on(bool);
bool is_power_on(void);

void set_als_enabled(bool);
bool is_als_enabled(void);

void set_proximity_enabled(bool);
bool is_proximity_enabled(void);

//void set_wait_enabled(bool);
//bool is_wait_enabled(void);

void set_als_interrupt_enabled(bool);
bool is_als_interrupt_enabled(void);

void set_proximity_interrupt_enabled(bool);
bool is_proximity_interrupt_enabled(void);

void set_gesture_enabled(bool);
bool is_gesture_enabled(void);




/* APDS9960_REG_ATIME */
//void set_adc_integration_time();	mi legyen a param

/* APDS9960_REG_WTIME */
//void set_wait_time();	mi legyen a param

/* APDS9960_REG_AILTL & APDS9960_REG_AILTH */
void set_als_interrupt_low_threshold(uint16_t);
uint16_t get_als_interrupt_low_threshold(void);

/* APDS9960_REG_AIHTL & APDS9960_REG_AIHTH */
void set_als_interrupt_high_threshold(uint16_t);
uint16_t get_als_interrupt_high_threshold(void);



/* APDS9960_REG_PILT */
void set_proximity_interrupt_low_threshold(uint8_t);
uint8_t get_proximity_interrupt_low_threshold(void);

/* APDS9960_REG_PIHT */
void set_proximity_interrupt_high_threshold(uint8_t);
uint8_t get_proximity_interrupt_high_threshold(void);

/* APDS9960_REG_PERS */
void set_proximity_interrupt_persistence(apds9960_ppers_t);
apds9960_ppers_t get_proximity_interrupt_persistence(void);

void set_als_interrupt_persistence(uint8_t);
uint8_t get_als_interrupt_persistence(void);

/* APDS9960_REG_CONFIG1 */
void set_wait_long_enabled(bool);
bool is_wait_long_enabled(void);

/* APDS9960_REG_PPULSE */
void set_proximity_pulse_length(apds9960_pplen_t);
apds9960_pplen_t get_proximity_pulse_length(void);

void set_proximity_pulse_count(uint8_t);
uint8_t get_proximity_pulse_count(void);

/* APDS9960_REG_CONTROL */
void set_proximity_led_drive_strength(apds9960_ldrive_t);
apds9960_ldrive_t get_proximity_led_drive_strength(void);

void set_proximity_gain(apds9960_pgain_t);
apds9960_pgain_t get_proximity_gain(void);

void set_als_and_color_gain(apds9960_again_t);
apds9960_again_t get_als_and_color_gain(void);

/* APDS9960_REG_CONFIG2 */
void set_proximity_saturation_interrupt_enabled(bool);
bool is_proximity_saturation_interrupt_enabled(void);

void set_c_photodiode_saturation_interrupt_enabled(bool);
bool is_c_photodiode_saturation_interrupt_enabled(void);

void set_led_boost(apds9960_ledboost_t);
apds9960_ledboost_t get_led_boost(void);

/* APDS9960_REG_ID */
uint8_t get_device_id(void);

/* APDS9960_REG_STATUS */
///TODO!!!!

//CDATAL...BDATAH
//void get_crgb_data();

/* APDS9960_REG_PDATA */
uint8_t get_proximity_data(void);

/* APDS9960_REG_POFFSET_UR */
//set_proximity_offset_up_and_right();		MI LEGYEN A PARAMÉTER?
//get_proximity_offset_up_and_right();		MI LEGYEN A PARAMÉTER?

/* APDS9960_REG_POFFSET_DL */
//set_proximity_offset_down_and_left();		MI LEGYEN A PARAMÉTER?
//get_proximity_offset_down_and_left();		MI LEGYEN A PARAMÉTER?

/* APDS9960_REG_CONFIG3 */
//TODO: PCMP, SAI plussz ez:
//set_proximity_mask_enable();






/* APDS9960_REG_GPENTH */
void set_gesture_proximity_enter_threshold(uint8_t);
uint8_t get_gesture_proximity_enter_threshold(void);

/* APDS9960_REG_GEXTH */
void set_gesture_proximity_exit_threshold(uint8_t);
uint8_t get_gesture_proximity_exit_threshold(void);




/* APDS9960_REG_GCONFIG1 */
void set_gesture_fifo_threshold(apds9960_gfifoth_t);
apds9960_gfifoth_t get_gesture_fifo_threshold(void);

void set_gesture_exit_mask(apds9960_gexmsk_t);
apds9960_gexmsk_t get_gesture_exit_mask(void);

void set_gesture_exit_persistence(apds9960_gexpers_t);
apds9960_gexpers_t get_gesture_exit_persistence(void);




/* APDS9960_REG_GCONFIG2 */
void set_gesture_gain(apds9960_ggain_t);
apds9960_ggain_t get_gesture_gain(void);

void set_gesture_led_drive_strength(apds9960_gldrive_t);
apds9960_gldrive_t get_gesture_led_drive_strength(void);

void set_gesture_wait_time(apds9960_gwtime_t);
apds9960_gwtime_t get_gesture_wait_time(void);





/* APDS9960_REG_GOFFSET_U */
void set_gesture_offset_up(int8_t);
int8_t get_gesture_offset_up(void);

/* APDS9960_REG_GOFFSET_D */
void set_gesture_offset_down(int8_t);
int8_t get_gesture_offset_down(void);

/* APDS9960_REG_GOFFSET_L */
void set_gesture_offset_left(int8_t);
int8_t get_gesture_offset_left(void);

/* APDS9960_REG_GOFFSET_R */
void set_gesture_offset_right(int8_t);
int8_t get_gesture_offset_right(void);




/* APDS9960_REG_GPULSE */
void apds9960_set_gesture_pulse_length(apds9960_gplen_t);
apds9960_gplen_t apds9960_get_gesture_pulse_length(void);

void set_gesture_pulse_count(uint8_t);
uint8_t get_gesture_pulse_count(void);

/* APDS9960_REG_GCONFIG3 */
void set_gesture_dimension_select(apds9960_gdims_t);
apds9960_gdims_t get_gesture_dimension_select(void);

/* APDS9960_REG_GCONFIG4 */
void set_gesture_interrupt_enabled(bool);
bool is_gesture_interrupt_enabled(void);

//GMODE?? todo

/* APDS9960_REG_GFLVL */
uint8_t get_gesture_fifo_level(void);

/* APDS9960_REG_GSTATUS */
//TODO...

/* APDS9960_REG_IFORCE */
void force_an_interrupt(void);

/* APDS9960_REG_PICLEAR */
void clear_proximity_interrupt(void);

/* APDS9960_REG_CICLEAR */
void clear_c_channel_interrupt(void);

/* APDS9960_REG_AICLEAR */
void clear_all_non_gesture_interrupt(void);



//TODO:
/* APDS9960_REG_GFIFO_U */
/* APDS9960_REG_GFIFO_D */
/* APDS9960_REG_GFIFO_L */
/* APDS9960_REG_GFIFO_R */
//gesture_fifo_t get_gesture_fifo(void);



/******************************************************************************/






typedef enum {
	ADPS9960_INVALID, APDS9960_UP, APDS9960_DOWN, APDS9960_LEFT, APDS9960_RIGHT
} apds9960_gesture_received_t;


extern uint8_t gest_cnt;
extern uint8_t u_count;
extern uint8_t d_count;
extern uint8_t l_count;
extern uint8_t r_count;

void gesture_init(void);
void apds9960_reset_counts(void);
void apds9960_gesture_to_uart(apds9960_gesture_received_t gesture_received);
apds9960_gesture_received_t apds9960_read_gesture(void);


/******************************************************************************/

#endif
