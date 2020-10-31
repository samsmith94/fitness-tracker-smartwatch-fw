#include "utility.h"

char RTT_String[20] = {0};

bool RTT_GetKey(void)
{
    int key;
    static int rx_index = 0;
    memset(RTT_String, '\0', 20);

    do
    {
        key = SEGGER_RTT_GetKey();
        if (key > 0)
        {
            RTT_String[rx_index] = (char)key;
            rx_index++;
        }
    } while ((key != '\n'));
    if (key == '\n')
    {
        RTT_String[rx_index-1] = '\0';
        //SEGGER_RTT_printf(0, "Received: %s", RTT_String);
        rx_index = 0;
        return true;
    }
    return false;
}

void print_ascii_art(void)
{
    //NRF_LOG_INFO("Hello World");

    //http://patorjk.com/software/taag/#p=display&f=Doom&t=Samu%20Sung%20Band

    //Doom

    //http://www.network-science.de/ascii/
    //smamslant:
    char welcome[] = "\n\
   ____                  ____                 ___               __\n\
  / __/__ ___ _  __ __  / __/_ _____  ___ _  / _ )___ ____  ___/ /\n\
 _\\ \\/ _ `/  ' \\/ // / _\\ \\/ // / _ \\/ _ `/ / _  / _ `/ _ \\/ _  / \n\
/___/\\_,_/_/_/_/\\_,_/ /___/\\_,_/_//_/\\_, / /____/\\_,_/_//_/\\_,_/  \n\
                                    /___/                         ";
    NRF_LOG_INFO("%s", welcome);

    nrf_delay_ms(2000);
}


