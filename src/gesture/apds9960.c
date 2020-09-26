#include "apds9960.h"


//így legyen? :
uint8_t get_device_status(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_STATUS, &value, 1);

    return value;
}

//ezt nem is érdemes meghívni, hiszen csak GFLVL>GFIFOTH vagy GVALID van, de ekkor a GINT is igaz lesz
// vagy get_device_status GINT-en belül lenne a get_gesture status
uint8_t get_gesture_status(void)
{
    uint8_t value;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GSTATUS, &value, 1);

    return value;
}

// a fentieken kívül lehetne olyan ami egyenként lekérdez, ha nem interrupt ábbal ahsználjuk, pl: is_proximity_valid()....

#define APDS9960_INT_PIN    NRF_GPIO_PIN_MAP(0, 15)

//ACTIVE LOW INTERRUPT!!!! apds9960 adatap szerint

//ez egy közös függvény kéne hogy legyen? ?? mármint hogy ne csak apds9960, hanem bármi más is...
//mégsem biztos a gpio_init miatt
void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == APDS9960_INT_PIN) {
        switch(action) {
        case NRF_GPIOTE_POLARITY_HITOLO:
            NRF_LOG_INFO("HIGH TO LOW INTERRUPT");

            int i = 0;
            while (i < 8) {
                gesture_fifo_t gesture_fifo;
                gesture_fifo = get_gesture_fifo();
                NRF_LOG_INFO("FIFO[%d] Up: %d | Down: %d | Left: %d | Right: %d", gesture_fifo.up, gesture_fifo.down, gesture_fifo.left, gesture_fifo.right, i);
                i++;
            }
            
            //apds9960_irq_handler();
            break;
        case NRF_GPIOTE_POLARITY_LOTOHI:
            NRF_LOG_INFO("LOW TO HIGH INTERRUPT");
            break;
        default:
            break;
        }
    }
    //lehet nem is kell a fenti sok if, hiszen be van állítva, hogy ehhez a lábhoz milyen esemény tartozik
    // tehát lehet, hogy ami most apds9960_irq_handler()-ben van, az csak simán itt lenen
    
    
    /*
    [else??] if (pin == LSM6DSOX_INT1_PIN) {
        switch(action) {
        case NRF_GPIOTE_POLARITY_LOTOHI:
            lsm6dsox_irq_handler();
            break;
        default:
            break;
        }
    }
    */
}

//void apds9960_gpio_init(void)???
void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);

    

    in_config.pull = NRF_GPIO_PIN_PULLUP;       //ez biztos kell? hiszen van külső pullup resistor!!!
    err_code = nrf_drv_gpiote_in_init(APDS9960_INT_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(APDS9960_INT_PIN, true);
}

/******************************************************************************/


void prox(void)
{
    NRF_LOG_INFO("hello");
}
als_interrupt_event_handler_t als_interrupt_event_handler;
proximity_interrupt_event_handler_t proximity_interrupt_event_handler = prox;
gesture_interrupt_event_handler_t gesture_interrupt_event_handler;


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

void apds9960_irq_handler(void)
{
    uint8_t status;
    uint8_t gesture_status;

    status = get_device_status();

    if ((status & (1 << APDS9960_PINT)) && proximity_interrupt_event_handler) {
        
        //kiolvasni a proximiti adatot...
        uint8_t proximity = get_proximity_data();

        NRF_LOG_INFO("Proximity: %d", proximity);
        //aztán handler: proximity_interrupt_event_handler(...);
        clear_proximity_interrupt();
    }

    if ((status & (1 << APDS9960_AINT)) && als_interrupt_event_handler) {
        
        //kiolvasni az rgb c adatot
        //aztán meghívni a handlert, átadva a crgb paramétert: als_interrupt_event_handler(...);

        clear_c_channel_interrupt();
    }

    if ((status & (1 << APDS9960_GINT)) && gesture_interrupt_event_handler) {
        //nem kell clear csak ki kell olvasni a fifo-t
        //kiolvasni a gesture fifo-t, (inkább a gfov ágban..): gesture_interrupt_event_handler(...);
        
        
        
        gesture_status = get_gesture_status();
        if (gesture_status & (1 << APDS9960_GFOV)) {
            //
        }
        if (gesture_status & (1 << APDS9960_GVALID)) {
            //
        }
    }
}

///és ugyanezt a sémát lehetne alkalmazni a többire, bár nyilván a gesture a legbonyolultabb

/******************************************************************************/
/******************************************************************************/
void set_power_on(bool value)
{
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
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
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
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
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_PEN)) {
		return true;
	} else {
		return false;
	}
}

void set_wait_enabled(bool value)
{
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_WEN);
    } else {
        BIT_CLEAR(config, APDS9960_WEN);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);
}

bool is_wait_enabled(void)
{
    NRF_LOG_INFO("%s:", __FUNCTION__);
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_ENABLE, &config, 1);

	if (BIT_CHECK(config, APDS9960_WEN)) {
		return true;
	} else {
		return false;
	}
}

void set_als_interrupt_enabled(bool value)
{
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
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
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
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
    NRF_LOG_INFO("%s(%d):", __FUNCTION__, value);
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
    NRF_LOG_INFO("%s:", __FUNCTION__);
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
void set_als_adc_integration_time(float time_ms)
{
    // convert ms into 2.78ms increments

    time_ms /= 2.78;
    time_ms = 256 - round(time_ms);
    if (time_ms > 255) {
        time_ms = 255;
    }
    if (time_ms < 0) {
        time_ms = 0;
    }

    uint8_t atime = (uint8_t)time_ms;
    //ez vajon jó???
    i2c_write(APDS9960_ADDR, APDS9960_REG_ATIME, &atime, 1);
}

//értem, hogy float de jobb lenne uint16_t...
float get_als_adc_integration_time(void)
{
    float time_ms;

    uint8_t atime;

    // jó ez a floatba olvasás?
    i2c_read(APDS9960_ADDR, APDS9960_REG_ATIME, (uint8_t *)&atime, 1);

    // convert to units of 2.78 ms
    time_ms = 256 - atime;
    time_ms *= 2.78;
    return time_ms;
}

/* APDS9960_REG_WTIME */
//na itt probléma van, mert amúgy hasonló az atime-hoz, de a wlong módosít
//void set_wait_time()
//void get_wait_time()

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
    uint8_t high_threshold[2];
    high_threshold[0] = value;
    high_threshold[1] = value << 8;
    i2c_write(APDS9960_ADDR, APDS9960_REG_AIHTL, &high_threshold[0], 2);
}

uint16_t get_als_interrupt_high_threshold(void)
{
    uint16_t value;
    uint8_t high_threshold[2];

    i2c_read(APDS9960_ADDR, APDS9960_REG_AIHTL, &high_threshold[0], 2);

    value = high_threshold[0] | high_threshold[1] << 8;
    return value;
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

void set_wait_long_enabled(bool value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG1, &config, 1);

    if (value) {
        BIT_SET(config, APDS9960_WLONG);
    } else {
        BIT_CLEAR(config, APDS9960_WLONG);
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONFIG1, &config, 1);
}

bool is_wait_long_enabled(void)
{
	uint8_t config = 0;

	i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG1, &config, 1);

	if (BIT_CHECK(config, APDS9960_WLONG)) {
		return true;
	} else {
		return false;
	}
}

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

    MULTIBIT_SET(config, APDS9960_PPULSE_MASK, (value-1));

    i2c_write(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);
}

uint8_t get_proximity_pulse_count(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_PPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_PPULSE_MASK);

    return (config+1);
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


void set_led_boost(apds9960_ledboost_t value)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

    MULTIBIT_SET(config, APDS9960_LEDBOOST_MASK, value);

    i2c_write(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);
}

apds9960_ledboost_t get_led_boost(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_CONFIG2, &config, 1);

    MULTIBIT_GET(config, APDS9960_LEDBOOST_MASK);

    return config;
}

/******************************************************************************/

uint8_t get_device_id(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_ID, &config, 1);

    return config;
}

/******************************************************************************/
//status.......

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

void set_proximity_offset_up_and_right(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_POFFSET_UR, &config, 1);
}

int8_t get_proximity_offset_up_and_right(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_POFFSET_UR, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}

void set_proximity_offset_down_and_left(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_POFFSET_DL, &config, 1);
}
int8_t get_proximity_offset_down_and_left(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_POFFSET_DL, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}

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

void set_gesture_offset_down(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_GOFFSET_D, &config, 1);
}

int8_t get_gesture_offset_down(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GOFFSET_D, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}

void set_gesture_offset_left(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_GOFFSET_L, &config, 1);
}

int8_t get_gesture_offset_left(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GOFFSET_L, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}

void set_gesture_offset_right(int8_t value)
{
    uint8_t config = 0;

    if (value >= 0) {
        config = value;
    } else {
        config = 128-value;
    }

    i2c_write(APDS9960_ADDR, APDS9960_REG_GOFFSET_R, &config, 1);
}

int8_t get_gesture_offset_right(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GOFFSET_R, &config, 1);

    if (config >= 129) {
        return (128-config);
    } else {
        return config;
    }
}

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

    MULTIBIT_SET(config, APDS9960_GPULSE_MASK, (value-1));

    i2c_write(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);
}

uint8_t get_gesture_pulse_count(void)
{
    uint8_t config = 0;

    i2c_read(APDS9960_ADDR, APDS9960_REG_GPULSE, &config, 1);

    MULTIBIT_GET(config, APDS9960_GPULSE_MASK);

    return (config+1);
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

void clear_proximity_interrupt(void)
{
    i2c_send(APDS9960_ADDR, APDS9960_REG_PICLEAR, 0);
}

void clear_c_channel_interrupt(void)
{
    i2c_send(APDS9960_ADDR, APDS9960_REG_CICLEAR, 0);
}

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

uint8_t gest_cnt = 0;
uint8_t u_count = 0;
uint8_t d_count = 0;
uint8_t l_count = 0;
uint8_t r_count = 0;

void gesture_init(void)
{
    uint8_t temp = 252;

    //atime
    i2c_write(APDS9960_ADDR, 0x81, &temp, 1);

    //again
	i2c_read(APDS9960_ADDR, 0x8F, &temp, 1);
	temp |= (1 << 0);
	i2c_write(APDS9960_ADDR, 0x8F, &temp, 1);

	//gconf4
	i2c_read(APDS9960_ADDR, 0xAB, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0xAB, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 6);
	temp &= ~(1 << 2);
	temp &= ~(1 << 1);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 5);
	temp &= ~(1 << 4);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	//aiclear
	temp = 0;
	i2c_write(APDS9960_ADDR, 0xE7, &temp, 1);

	//disable-enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
	nrf_delay_ms(10);
	temp |= (1 << 0);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
    nrf_delay_ms(10);

	//gconf3 gdims
	temp = 0;
	//i2c_write(APDS9960_ADDR, 0xAA, &temp, 1);
    //csak bal-jobb:
    temp = 0b00000010;
    //csak fel-le
    //temp= 0b00000001;
    i2c_write(APDS9960_ADDR, 0xAA, &temp, 1);

	//gconf1 gfifith
	i2c_read(APDS9960_ADDR, 0xA2, &temp, 1);
	temp |= 0b01000000;
	i2c_write(APDS9960_ADDR, 0xA2, &temp, 1);

	//gconf2 ggain
	i2c_read(APDS9960_ADDR, 0xA3, &temp, 1);
	temp |= 0b01100000;
	i2c_write(APDS9960_ADDR, 0xA3, &temp, 1);

	//gpenth
	temp = 50;
	i2c_write(APDS9960_ADDR, 0xA0, &temp, 1);

	//gplulse
	temp = 0b11001001;
	i2c_write(APDS9960_ADDR, 0xA6, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp |= (1 << 6);
	temp |= (1 << 2);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	uint8_t id;
	i2c_read(APDS9960_ADDR, 0x92, &id, 1);
	NRF_LOG_INFO("gesture initialized, %d", id);
}

void apds9960_reset_counts(void)
{
	gest_cnt = 0;
	u_count = 0;
	d_count = 0;
	l_count = 0;
	r_count = 0;
}

void apds9960_gesture_to_uart(apds9960_gesture_received_t gesture_received)
{
	switch (gesture_received) {
	case APDS9960_UP:
		NRF_LOG_INFO("UP");
		break;
	case APDS9960_DOWN:
		NRF_LOG_INFO("DOWN");
		break;
	case APDS9960_LEFT:
		NRF_LOG_INFO("LEFT");
		break;
	case APDS9960_RIGHT:
		NRF_LOG_INFO("RIGHT");
		break;
	default:
		break;
	}
}

apds9960_gesture_received_t apds9960_read_gesture(void)
{
	//printf("read gesture function %d\r\n", HAL_GetTick());
	uint8_t to_read;
	uint8_t buf[256];
	unsigned long t = 0;
	apds9960_gesture_received_t gesture_received;

	while (1) {
		int up_down_diff = 0;
		int left_right_diff = 0;
		gesture_received = 0;

		/* is gesture valid */
		uint8_t temp;
		i2c_read(APDS9960_ADDR, 0xAF, &temp, 1);

		if (!(temp & (1 << 0))) {
			return ADPS9960_INVALID;
		}

		nrf_delay_ms(30);

		/* getting the nember of samples to be read */
		temp = 0;
		i2c_read(APDS9960_ADDR, 0xAE, &temp, 1);
		//printf("Samples to read: %d\r\n", temp);

		to_read = temp;

		// bytesRead is unused but produces sideffects needed for readGesture to work

		//APDS9960_GFIFO_U
		i2c_read(APDS9960_ADDR, 0xFC, buf, to_read);

		if (abs((int) buf[0] - (int) buf[1]) > 13)
			up_down_diff += (int) buf[0] - (int) buf[1];

		if (abs((int) buf[2] - (int) buf[3]) > 13)
			left_right_diff += (int) buf[2] - (int) buf[3];

		if (up_down_diff != 0) {
			if (up_down_diff < 0) {
				if (d_count > 0) {
					gesture_received = APDS9960_UP;
				} else
					u_count++;
			} else if (up_down_diff > 0) {
				if (u_count > 0) {
					gesture_received = APDS9960_DOWN;
				} else
					d_count++;
			}
		}

		if (left_right_diff != 0) {
			if (left_right_diff < 0) {
				if (r_count > 0) {
					gesture_received = APDS9960_LEFT;
				} else
					l_count++;
			} else if (left_right_diff > 0) {
				if (l_count > 0) {
					gesture_received = APDS9960_RIGHT;
				} else
					r_count++;
			}
		}

		if (up_down_diff != 0 || left_right_diff != 0)
			//t = HAL_GetTick();
            t = NRF_RTC0->COUNTER / 8;

		//if (gesture_received || HAL_GetTick() - t > 300) {
        if (gesture_received || (NRF_RTC0->COUNTER / 8) - t > 300) {
			apds9960_reset_counts();
			return gesture_received;
		}
	}
}

//LECSWERÉLVE A FENTI REGISZTES KÓDOKAT RENDES NEVESÍTETT FÜGGVÉYNKERE:
void new_gesture_init(void)
{
    //a kövi rész az adafruittól van:
    /**************************************************************************/
    set_als_adc_integration_time(10);

	set_als_and_color_gain(APDS9960_A_GAIN_4X);

	//gconf4: EZ BIZONY A GMODE, utánaolvasni:
    uint8_t temp;
	i2c_read(APDS9960_ADDR, 0xAB, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0xAB, &temp, 1);

	set_gesture_enabled(false);
    set_proximity_enabled(false);
    set_als_enabled(false);

    set_proximity_interrupt_enabled(false);
    set_als_interrupt_enabled(false);

    clear_all_non_gesture_interrupt();

	//disable-enable
    set_power_on(false);
	nrf_delay_ms(10);
	set_power_on(true);
    nrf_delay_ms(10);

	//gconf3 gdims
    apds9960_gdims_t gdims = APDS9960_BOTH_PAIRS_ACTICE;
    //apds9960_gdims_t gdims = APDS9960_UP_DOWN_ACTICE;
    //apds9960_gdims_t gdims = APDS9960_LEFT_RIGHT_ACTICE;
    set_gesture_dimension_select(gdims);

	//gconf1 gfifith
    apds9960_gfifoth_t gfifoth = APDS9960_INTERRUPT_AFTER_4_DATASET_ADDED_TO_FIFO;
    set_gesture_fifo_threshold(gfifoth);

	//gconf2 ggain
    apds9960_ggain_t ggain = APDS9960_G_GAIN_4X;
    set_gesture_gain(ggain);

	//gpenth
    set_gesture_proximity_enter_threshold(50);

	//gplulse
    apds9960_gplen_t gplen = APDS9960_G_PULSE_LENGTH_32_US;
    apds9960_set_gesture_pulse_length(gplen);

    //ez valójában 10-et jelent! nem kéne átírni a függvéynt hogy ő adjon hozzá?
    set_gesture_pulse_count(10);
    
    //eddig tart az adarfruit
    /**************************************************************************/

    //ez nem tudom mit keres itt, hiszen ez a gplen és a pulsecount
    //csak itt 16_US ezzel a beállítással:
    /*
	temp = 0b11001001;
	i2c_write(APDS9960_ADDR, 0xA6, &temp, 1);
    */

	//enable
    /*
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp |= (1 << 6);
	temp |= (1 << 2);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
    */
    // a fenti megfelelője:
    set_proximity_enabled(true);
    set_gesture_enabled(true);

    //ez az id vizsgálat lehetne a legelején is, sőt ott célszerűbb volna talán
    /*
	uint8_t id;
	i2c_read(APDS9960_ADDR, 0x92, &id, 1);
	NRF_LOG_INFO("gesture initialized, %d", id);
    */
    uint8_t id = get_device_id();
    if (id == 0xAB) {
        NRF_LOG_INFO("Gesture initialized successfully.");
    } else {
        NRF_LOG_INFO("Gesture initialization failed.");
    }

    set_gesture_fifo_threshold(APDS9960_INTERRUPT_AFTER_1_DATASET_ADDED_TO_FIFO);
    set_gesture_interrupt_enabled(true);
   //MÉG NÉHÁNY INTERRUPTOT ÉS BE LEHETNE ÁLLÍTANI....
}

apds9960_gesture_received_t new_apds9960_read_gesture(void)
{
    //gesture_fifo_t gesture_fifo_buffer[32];

	uint8_t to_read;
	uint8_t buf[256];
	unsigned long t = 0;
	apds9960_gesture_received_t gesture_received;

	while (1) {
		int up_down_diff = 0;
		int left_right_diff = 0;
		gesture_received = 0;

		/* is gesture valid */
        //még nincs implementálva...., azt sem találtam ki hogy legyen a státuszlekérdezés
        /*
        if (!is_gesture_valid()) {
            return ADPS9960_INVALID;
        }
        */
		uint8_t temp;
		i2c_read(APDS9960_ADDR, 0xAF, &temp, 1);

		if (!(temp & (1 << 0))) {
			return ADPS9960_INVALID;
		}

		nrf_delay_ms(30);

		/* getting the number of samples to be read */
        to_read = get_gesture_fifo_level();

		// bytesRead is unused but produces sideffects needed for readGesture to work

		//APDS9960_GFIFO_U
		i2c_read(APDS9960_ADDR, 0xFC, buf, to_read);

		if (abs((int) buf[0] - (int) buf[1]) > 13)
			up_down_diff += (int) buf[0] - (int) buf[1];

		if (abs((int) buf[2] - (int) buf[3]) > 13)
			left_right_diff += (int) buf[2] - (int) buf[3];

		if (up_down_diff != 0) {
			if (up_down_diff < 0) {
				if (d_count > 0) {
					gesture_received = APDS9960_UP;
				} else
					u_count++;
			} else if (up_down_diff > 0) {
				if (u_count > 0) {
					gesture_received = APDS9960_DOWN;
				} else
					d_count++;
			}
		}

		if (left_right_diff != 0) {
			if (left_right_diff < 0) {
				if (r_count > 0) {
					gesture_received = APDS9960_LEFT;
				} else
					l_count++;
			} else if (left_right_diff > 0) {
				if (l_count > 0) {
					gesture_received = APDS9960_RIGHT;
				} else
					r_count++;
			}
		}

		if (up_down_diff != 0 || left_right_diff != 0)
			//t = HAL_GetTick();
            t = NRF_RTC0->COUNTER / 8;

		//if (gesture_received || HAL_GetTick() - t > 300) {
        if (gesture_received || (NRF_RTC0->COUNTER / 8) - t > 300) {
			apds9960_reset_counts();
			return gesture_received;
		}
	}
}
/******************************************************************************/


void proximity_init(void)
{
    /*
    gpio_init();

    set_power_on(false);
    

    set_proximity_interrupt_low_threshold(0);
    set_proximity_interrupt_high_threshold(175);
    set_proximity_interrupt_persistence(APDS9960_4_CONSECUTIVE_PROX_VALUE_OUT_OF_RANGE);

    set_proximity_interrupt_enabled(true);

    set_proximity_enabled(true);
    */
}
