#include "apds9960.h"




//PINT, AINT, GIEN

/*
PINT után PICLEAR kell
AINT után CICLEAR
GINT után semmi
*/


//így legyen? :
uint8_t get_device_status(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_STATUS, &value, 1);

    return value;
}

uint8_t get_gesture_status(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GSTATUS, &value, 1);

    return value;
}
//vagy egyenként lekérni??? pl. PVALID?

void interrupt_handler(void)
{
    uint8_t status;

    status = get_device_status();

    if (status & (1 << APDS9960_PINT)) {
        //PICLEAR vagy AICLEAR
    }

    if (status & (1 << APDS9960_AINT)) {
        //CICLEAR vagy AICLEAR
    }

    if (status & (1 << APDS9960_GINT)) {
        //nem kell clear csak ki kell olvasni a fifo-t
    }
}



/*
CPSAT       - mi legyen a függvény neve?
PGSAT       - mi legyen a függvény neve?
is_proximity_valid();
is_als_valid();
*/

/*
GFOV        - is_gesture_fifo_overflow()    - jó név???
GVALID      - is_gesture_fifo_data_valid()
*/





//#define APDS9960_INT_PIN    NRF_GPIO_PIN_MAP(0, ?)
//ideiglenesen:
#define APDS9960_INT_PIN    0

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == APDS9960_INT_PIN) {
        switch(action) {
        case NRF_GPIOTE_POLARITY_LOTOHI:
            //
            break;
        case NRF_GPIOTE_POLARITY_HITOLO:
            //
            break;
        case NRF_GPIOTE_POLARITY_TOGGLE:
            //
            break;
        default:
            break;
        }
    }
}


void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    in_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(APDS9960_INT_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(APDS9960_INT_PIN, true);
}



/******************************************************************************/
//próba egy másik irq handlerre:


typedef void (*als_interrupt_event_handler_t)(uint16_t clear, uint16_t red, uint16_t green, uint16_t blue);

als_interrupt_event_handler_t als_interrupt_event_handler;


// túl sok a paraméter, lehetne struktúrába esetleg...
bool set_als_interrupt_handle_cb(uint16_t low_threshold, uint16_t high_threshold, als_interrupt_event_handler_t handler)
{
    //TODO:  ellenőrizni, hogy a thresholdok be vannak-e állítva? vagy hogy sikeres volt a beállítás
    set_proximity_enabled(true);

    set_als_interrupt_low_threshold(low_threshold);
    set_als_interrupt_high_threshold(high_threshold);
    
    set_proximity_interrupt_enabled(true);

    als_interrupt_event_handler = &handler;
}

void int_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    bool proximity_interrupt_firing = false;
    bool als_interrupt_firing = false;
    bool gesture_interrupt_firing = false;

    //itt le kéne kérdezni a státus regisztert

    if (als_interrupt_firing && als_interrupt_event_handler) {
        
        //kiolvasni a crgb-t
        //get_crgb_dats()

        //aztán meghívni a handlert, átadva a crgb paramétert:
        //als_interrupt_event_handler(...);

    }

    //a legvégén törölni kell az interruptokat (clear all...)

}

///és ugyanezt a sémát lehetne alkalmazni a többire, bár nyilván a gesture a legbonyolultabb


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void set_power_on(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (value) {
		BIT_SET(config, APDS9960_PON);
	} else {
		BIT_CLEAR(config, APDS9960_PON);
	}

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_power_on(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_PON)) {
		return true;
	} else {
		return false;
	}
}

void set_als_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_AEN);
    } else {
        BIT_CLEAR(config, APDS9960_AEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_als_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_AEN)) {
		return true;
	} else {
		return false;
	}
}

void set_proximity_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_PEN);
    } else {
        BIT_CLEAR(config, APDS9960_PEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_proximity_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_PEN)) {
		return true;
	} else {
		return false;
	}
}

//void set_wait_enabled(bool);
//bool is_wait_enabled(void);

void set_als_interrupt_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_AIEN);
    } else {
        BIT_CLEAR(config, APDS9960_AIEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_als_interrupt_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_AIEN)) {
		return true;
	} else {
		return false;
	}
}

void set_proximity_interrupt_enabled(bool value)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (value) {
		BIT_SET(config, APDS9960_PIEN);
	} else {
		BIT_CLEAR(config, APDS9960_PIEN);
	}

	i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_proximity_interrupt_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_PIEN)) {
		return true;
	} else {
		return false;
	}
}

void set_gesture_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_GEN);
    } else {
        BIT_CLEAR(config, APDS9960_GEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_gesture_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_GEN)) {
		return true;
	} else {
		return false;
	}
}

/******************************************************************************/

/* APDS9960_REG_ATIME */
//void set_adc_integration_time();	mi legyen a param

/* APDS9960_REG_WTIME */
//void set_wait_time();	mi legyen a param


/******************************************************************************/

void set_als_interrupt_low_threshold(uint16_t value)
{
    uint8_t low_threshold[2];
    low_threshold[0] = value;
    low_threshold[1] = value << 8;
    i2c_write(APDS9960_ADDR, APDS9960_REG_AILTL, &low_threshold[0], 2);
}

uint16_t get_als_interrupt_low_threshold(void)
{
    uint16_t value;
    uint8_t low_threshold[2];

    i2c_read(APDS9960_ADDR, APDS9960_REG_AILTL, &low_threshold[0], 2);

    value = low_threshold[0] | low_threshold[1] << 8;
    return value;
}

void set_als_interrupt_high_threshold(uint16_t value)
{
	//
}

uint16_t get_als_interrupt_high_threshold(void)
{
	//
}

/******************************************************************************/

void set_proximity_interrupt_low_threshold(uint8_t value)
{
    i2c_write(APDS9960_ADDR, APDS9960_REG_PILT, &value, 1);
}

uint8_t get_proximity_interrupt_low_threshold(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PILT, &value, 1);

    return value;
}

/******************************************************************************/

void set_proximity_interrupt_high_threshold(uint8_t value)
{
    i2c_write(APDS9960_ADDR, APDS9960_REG_PIHT, &value, 1);
}

uint8_t get_proximity_interrupt_high_threshold(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PIHT, &value, 1);

    return value;
}

/******************************************************************************/


void set_proximity_interrupt_persistence(apds9960_ppers_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);

    MULTIBIT_SET(config, APDS9960_PPERS_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);
}

apds9960_ppers_t get_proximity_interrupt_persistence(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);

    MULTIBIT_GET(config, APDS9960_PPERS_MASK);

    return config;
}

/******************************************************************************/

void set_als_interrupt_persistence(uint8_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);

    MULTIBIT_SET(config, APDS9960_APERS_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);
}

uint8_t get_als_interrupt_persistence(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PERS, &config, 1);

    MULTIBIT_GET(config, APDS9960_APERS_MASK);

    return config;
}

/******************************************************************************/

//void set_wait_long_enabled(bool);
//bool is_wait_long_enabled(void);

/******************************************************************************/


void set_proximity_pulse_length(apds9960_pplen_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);

    MULTIBIT_SET(config, APDS9960_PPLEN_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);
}

apds9960_pplen_t get_proximity_pulse_length(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_PPLEN_MASK);

    return config;
}

void set_proximity_pulse_count(uint8_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);

    MULTIBIT_SET(config, APDS9960_PPULSE_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);
}

uint8_t get_proximity_pulse_count(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_PPULSE_MASK);

    return config;
}

/******************************************************************************/


void set_proximity_led_drive_strength(apds9960_ldrive_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_SET(config, APDS9960_LDRIVE_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);
}

apds9960_ldrive_t get_proximity_led_drive_strength(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_GET(config, APDS9960_LDRIVE_MASK);

    return config;
}






void set_proximity_gain(apds9960_pgain_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_SET(config, APDS9960_PGAIN_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);
}

apds9960_pgain_t get_proximity_gain(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_GET(config, APDS9960_PGAIN_MASK);

    return config;
}





void set_als_and_color_gain(apds9960_again_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_SET(config, APDS9960_AGAIN_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);
}

apds9960_again_t get_als_and_color_gain(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONTROL, &config, 1);

    MULTIBIT_GET(config, APDS9960_AGAIN_MASK);

    return config;
}




/******************************************************************************/

void set_proximity_saturation_interrupt_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

	if (value) {
		BIT_SET(config, APDS9960_PSIEN);
	} else {
		BIT_CLEAR(config, APDS9960_PSIEN);
	}

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);
}

bool is_proximity_saturation_interrupt_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

	if (BIT_CHECK(config, APDS9960_PSIEN)) {
		return true;
	} else {
		return false;
	}
}



void set_c_photodiode_saturation_interrupt_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_CPSIEN);
    } else {
        BIT_CLEAR(config, APDS9960_CPSIEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);
}

bool is_c_photodiode_saturation_interrupt_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

	if (BIT_CHECK(config, APDS9960_CPSIEN)) {
		return true;
	} else {
		return false;
	}
}


//void set_led_boost(apds9960_ledboost_t);
//apds9960_ledboost_t get_led_boost(void);


/******************************************************************************/

uint8_t get_device_id(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ID, &config, 1);

    return config;
}

/******************************************************************************/
//status

/******************************************************************************/
//cdata....
/******************************************************************************/

uint8_t get_proximity_data(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_PDATA, &config, 1);

    return config;
}

/******************************************************************************/

//set_proximity_offset_up_and_right();		MI LEGYEN A PARAMÉTER?
//get_proximity_offset_up_and_right();		MI LEGYEN A PARAMÉTER?

//set_proximity_offset_down_and_left();		MI LEGYEN A PARAMÉTER?
//get_proximity_offset_down_and_left();		MI LEGYEN A PARAMÉTER?



/******************************************************************************/
//TODO: PCMP, SAI plussz ez:
//set_proximity_mask_enable();












/******************************************************************************/
/* GESTURE ********************************************************************/
/******************************************************************************/

void set_gesture_proximity_enter_threshold(uint8_t value)
{
    i2c_write(APDS9960_ADDR, APDS9960_REG_GPENTH, &value, 1);
}

uint8_t get_gesture_proximity_enter_threshold(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPENTH, &value, 1);

    return value;
}

/******************************************************************************/

void set_gesture_proximity_exit_threshold(uint8_t value)
{
    i2c_write(APDS9960_ADDR, APDS9960_REG_GEXTH, &value, 1);
}

uint8_t get_gesture_proximity_exit_threshold(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GEXTH, &value, 1);

    return value;
}

/******************************************************************************/

void set_gesture_fifo_threshold(apds9960_gfifoth_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_SET(config, APDS9960_GFIFOTH_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);
}

apds9960_gfifoth_t get_gesture_fifo_threshold(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_GET(config, APDS9960_GFIFOTH_MASK);

    return config;
}

void set_gesture_exit_mask(apds9960_gexmsk_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_SET(config, APDS9960_GEXMSK_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);
}

apds9960_gexmsk_t get_gesture_exit_mask(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_GET(config, APDS9960_GEXMSK_MASK);

    return config;
}

void set_gesture_exit_persistence(apds9960_gexpers_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_SET(config, APDS9960_GEXPERS_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);
}

apds9960_gexpers_t get_gesture_exit_persistence(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG1, &config, 1);

    MULTIBIT_GET(config, APDS9960_GEXPERS_MASK);

    return config;
}

/******************************************************************************/

void set_gesture_gain(apds9960_ggain_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_SET(config, APDS9960_GGAIN_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);
}

apds9960_ggain_t get_gesture_gain(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_GET(config, APDS9960_GGAIN_MASK);

    return config;
}

void set_gesture_led_drive_strength(apds9960_gldrive_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_SET(config, APDS9960_GLDRIVE_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);
}

apds9960_gldrive_t get_gesture_led_drive_strength(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_GET(config, APDS9960_GLDRIVE_MASK);

    return config;
}

void set_gesture_wait_time(apds9960_gwtime_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_SET(config, APDS9960_GWTIME_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);
}

apds9960_gwtime_t get_gesture_wait_time(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG2, &config, 1);

    MULTIBIT_GET(config, APDS9960_GWTIME_MASK);

    return config;
}

/******************************************************************************/


// le vannak tesztelve, jók ezek az offsetes int8-as get és set függvények
void set_gesture_offset_up(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_GOFFSET_U, &config, 1);
}

int8_t get_gesture_offset_up(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GOFFSET_U, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}
/*
void set_gesture_offset_down(int8_t value);
int8_t get_gesture_offset_down(void);

void set_gesture_offset_left(int8_t value);
int8_t get_gesture_offset_left(void);

void set_gesture_offset_right(int8_t value);
int8_t get_gesture_offset_right(void);
*/


/******************************************************************************/


void apds9960_set_gesture_pulse_length(apds9960_gplen_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);

    MULTIBIT_SET(config, APDS9960_GPLEN_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);
}

apds9960_gplen_t apds9960_get_gesture_pulse_length(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_GPLEN_MASK);

    return config;
}

void set_gesture_pulse_count(uint8_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);

    MULTIBIT_SET(config, APDS9960_GPULSE_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);
}

uint8_t get_gesture_pulse_count(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_GPULSE_MASK);

    return config;
}

/******************************************************************************/


void set_gesture_dimension_select(apds9960_gdims_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG3, &config, 1);

    MULTIBIT_SET(config, APDS9960_GDIMS_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG3, &config, 1);
}

apds9960_gdims_t get_gesture_dimension_select(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG3, &config, 1);

    MULTIBIT_GET(config, APDS9960_GDIMS_MASK);

    return config;
}



/******************************************************************************/


void set_gesture_interrupt_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG4, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_GIEN);
    } else {
        BIT_CLEAR(config, APDS9960_GIEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_GCONFIG4, &config, 1);
}

bool is_gesture_interrupt_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_GCONFIG4, &config, 1);

	if (BIT_CHECK(config, APDS9960_GIEN)) {
		return true;
	} else {
		return false;
	}
}


/******************************************************************************/


//gmode!!!
/******************************************************************************/


uint8_t get_gesture_fifo_level(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GFLVL, &value, 1);

    return value;
}

/******************************************************************************/

//GSTATUS


/******************************************************************************/



void force_an_interrupt(void)
{
    i2c_send(APDS9960_ADDR, APDS9960_REG_IFORCE, 0);
}

//void clear_proximity_interrupt(void);

//void clear_c_channel_interrupt(void);


void clear_all_non_gesture_interrupt(void)
{
    i2c_send(APDS9960_ADDR, APDS9960_REG_AICLEAR, 0);
}


/******************************************************************************/


//UDLR FIFO:

//gesture_fifo_t gesture_fifo_buffer[32];

gesture_fifo_t get_gesture_fifo(void)
{
    gesture_fifo_t gesture_fifo;
    
    uint8_t dataset[4];

    i2c_read(APDS9960_ADDR, APDS9960_REG_GFIFO_U, dataset, 4);

    memcpy(gesture_fifo.gesture_dataset, dataset, 4 * sizeof(uint8_t));
    
    return gesture_fifo;
}

/******************************************************************************/