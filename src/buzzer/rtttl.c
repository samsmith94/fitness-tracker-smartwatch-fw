#include "rtttl.h"

//char tune[] = {"Urgent:d=8,o=6,b=500:c,e,d7,c,e,a#,c,e,a,c,e,g"};

//char tune[] = {"Triple:d=8,o=5,b=635:c,e,g,c,e,g"};

char tune[] = {"Intel:d=16,o=5,b=400:d,p,d,p,d,p"};


//jó kis értesítés hang
//char tune[] = {"Fido:d=16,o=6,b=800:f,4p,f,4p,f,4p,f,4p,c,4p,c,4p,c,4p,c,1p,1p,1p,1p"};

//ébresztőnek jó:
//char tune[] = {"Mosaic:d=8,o=6,b=400:c,e,g,e,c,g,e,g,c,g,c,e,c,g,e,g,e,c"};


//https://www.vex.net/~lawrence/ringtones.html

//https://forum.arduino.cc/index.php?topic=510297.0

//http://mines.lumpylumpy.com/Electronics/Computers/Software/Cpp/MFC/RingTones.php#.X5SO34gzaUl

//https://github.com/granadaxronos/120-SONG_NOKIA_RTTTL_RINGTONE_PLAYER_FOR_ARDUINO_UNO/blob/master/RTTTL_PLAYER/songs.h





char temp[4];

int defaultlength;
int defaultoctave;
int beatspermin;

int length;
int octave;
float basenote;
float note;

float spb = 0;
float duration;
float m = 0;
float bpm = 0;

int c = 0;
int pointer = 0;

void play_note(float duration, float frequency)
{
    //NRF_LOG_INFO("play_note()...");
    long int i, cycles;
    float half_period;
    float wavelength;

    wavelength = (1 / frequency) * 1000;
    cycles = duration / wavelength;
    half_period = wavelength / 2;

    for (i = 0; i < cycles; i++)
    {
        nrf_delay_ms(half_period*10);
        nrf_drv_gpiote_out_set(PIN_OUT);
        //nrf_gpio_pin_set(PIN_OUT);

        nrf_delay_ms(half_period*10);
        nrf_drv_gpiote_out_clear(PIN_OUT);
        //nrf_gpio_pin_clear(PIN_OUT);
    }

    return;
}

void buzzer_init(void)
{
    NRF_LOG_INFO("buzzer_init()...");
    ret_code_t err_code;

    //err_code = nrf_drv_gpiote_init();
    //APP_ERROR_CHECK(err_code);

    
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
    err_code = nrf_drv_gpiote_out_init(PIN_OUT, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_set(PIN_OUT);
    

    //nrf_gpio_cfg_output(PIN_OUT);

    /*
    while (1)
    {
        nrf_gpio_pin_toggle(PIN_OUT);
        nrf_delay_ms(100);
    }*/
}

void test_rtttl_player(void)
{
    NRF_LOG_INFO("test_rtttl_player()...");
    do
    {
        pointer++;
    } while (tune[pointer] != ':');
    pointer++;

    pointer++;
    pointer++;
    c = 0;
    do
    {
        temp[c] = tune[pointer];
        pointer++;
        c++;
    } while (tune[pointer] != ',');
    temp[c] = '\0';
    defaultlength = atoi(temp);
    NRF_LOG_INFO("Default length: %d", defaultlength);
    pointer++;

    pointer++;
    pointer++;
    c = 0;
    do
    {
        temp[c] = tune[pointer];
        pointer++;
        c++;
    } while (tune[pointer] != ',');
    temp[c] = '\0';
    defaultoctave = atoi(temp);
    NRF_LOG_INFO("Default octave: %d", defaultoctave);
    pointer++;

    pointer++;
    pointer++;
    c = 0;
    do
    {
        temp[c] = tune[pointer];
        pointer++;
        c++;
    } while (tune[pointer] != ':');
    temp[c] = '\0';
    beatspermin = atoi(temp);
    pointer++;

    do
    {

        if ((tune[pointer] == '3') && (tune[pointer + 1] == '2'))
        {
            length = 32;
            pointer = pointer + 2;
        }
        else if ((tune[pointer] == '1') && (tune[pointer + 1] == '6'))
        {
            length = 16;
            pointer = pointer + 2;
        }
        else if (tune[pointer] == '8')
        {
            length = 8;
            pointer++;
        }
        else if (tune[pointer] == '4')
        {
            length = 4;
            pointer++;
        }
        else if (tune[pointer] == '2')
        {
            length = 2;
            pointer++;
        }
        else if (tune[pointer] == '1')
        {
            length = 1;
            pointer++;
        }
        else
            length = defaultlength;

        if ((tune[pointer] == 'a') && (tune[pointer + 1] == '#'))
        {
            basenote = 466.164;
            pointer = pointer + 2;
        }
        else if ((tune[pointer] == 'c') && (tune[pointer + 1] == '#'))
        {
            basenote = 554.365;
            pointer = pointer + 2;
        }
        else if ((tune[pointer] == 'd') && (tune[pointer + 1] == '#'))
        {
            basenote = 622.254;
            pointer = pointer + 2;
        }
        else if ((tune[pointer] == 'f') && (tune[pointer + 1] == '#'))
        {
            basenote = 739.989;
            pointer = pointer + 2;
        }
        else if ((tune[pointer] == 'g') && (tune[pointer + 1] == '#'))
        {
            basenote = 830.609;
            pointer = pointer + 2;
        }
        else if (tune[pointer] == 'a')
        {
            basenote = 440.000;
            pointer++;
        }
        else if (tune[pointer] == 'b')
        {
            basenote = 493.883;
            pointer++;
        }
        else if (tune[pointer] == 'c')
        {
            basenote = 523.251;
            pointer++;
        }
        else if (tune[pointer] == 'd')
        {
            basenote = 587.330;
            pointer++;
        }
        else if (tune[pointer] == 'e')
        {
            basenote = 659.255;
            pointer++;
        }
        else if (tune[pointer] == 'f')
        {
            basenote = 698.456;
            pointer++;
        }
        else if (tune[pointer] == 'g')
        {
            basenote = 783.991;
            pointer++;
        }
        else if (tune[pointer] == 'p')
        {
            basenote = 0;
            pointer++;
        }
        else
            basenote = basenote;

        if (tune[pointer] == '4')
        {
            octave = 4;
            pointer++;
        }
        else if (tune[pointer] == '5')
        {
            octave = 5;
            pointer++;
        }
        else if (tune[pointer] == '6')
        {
            octave = 6;
            pointer++;
        }
        else if (tune[pointer] == '7')
        {
            octave = 7;
            pointer++;
        }
        else
            octave = defaultoctave;

        if (octave == 4)
        {
            note = basenote;
        }
        else if (octave == 5)
        {
            note = basenote * 2;
        }
        else if (octave == 6)
        {
            note = basenote * 4;
        }
        else if (octave == 7)
        {
            note = basenote * 8;
        }

        if (tune[pointer] == '.')
        {
            m = 1.5;
            pointer++;
        }
        else
            m = 1;

        bpm = beatspermin;
        spb = 60 / bpm;
        duration = (spb / length) * 1000 * m;

        if (note == 0)
        {
            nrf_delay_ms(duration);
        }
        else
        {
            play_note(duration, note);
        }

        nrf_delay_ms(spb * 150);

    } while (tune[pointer++] == ',');

    NRF_LOG_INFO("melody end");
}
