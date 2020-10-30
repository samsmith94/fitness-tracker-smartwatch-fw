#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrfx_spim.h"

#include "fonts.h"

#include "app_pwm.h"

#include "nrf.h"


#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

//#define ST7735_DEFAULT_ORIENTATION
//#define ST7735_ROTATE_RIGHT
#define ST7735_ROTATE_LEFT

#if defined (ST7735_DEFAULT_ORIENTATION)
// mini 160x80 display (it's unlikely you want the default orientation)
#define ST7735_IS_160X80 1
#define ST7735_XSTART 26
#define ST7735_YSTART 1
#define ST7735_WIDTH  80
#define ST7735_HEIGHT 160
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR)
#elif defined (ST7735_ROTATE_LEFT)
// mini 160x80, rotate left
#define ST7735_IS_160X80 1
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#elif defined (ST7735_ROTATE_RIGHT)
// mini 160x80, rotate right
#define ST7735_IS_160X80 1
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#else
#error "You must specify the orientation"
#endif

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

//#define ST7735_RESET_PIN        ??

#define ST7735_SPI_SCK_PIN      NRF_GPIO_PIN_MAP(0, 22)
#define ST7735_SPI_MOSI_PIN     NRF_GPIO_PIN_MAP(0, 23)

#define ST7735_DC_PIN           NRF_GPIO_PIN_MAP(0, 24)
#define ST7735_SPI_SS_PIN       NRF_GPIO_PIN_MAP(0, 25)

#define ST7735_SPI_MISO_PIN     NRF_GPIO_PIN_MAP(0, 26) //nincs használva


void init_display_pwm(void);
void pwm_ready_callback(uint32_t pwm_id);
void set_display_pwm(uint16_t duty);

void command_list(void);
void set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
ret_code_t hardware_init(void);
ret_code_t st7735_init(void);
void st7735_uninit(void);
void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void st7735_write_char(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_write_string(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);
void st7735_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7735_fill_screen(uint16_t color);
void st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
void st7735_invert_colors(bool invert);
//void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius);
void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, uint32_t color);


void rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t r;
} circle_t;

void fill_circle_draw(circle_t const * p_circle, uint32_t color);

void line_draw(uint16_t x_0, uint16_t y_0, uint16_t x_1, int16_t y_1, uint32_t color);

/******************************************************************************/


    
void st7735_sleep_in(void);
void st7735_sleep_out(void);

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t color_r;
    uint8_t color_g;
    uint8_t color_b;
} pixel_t;

typedef struct {
    uint16_t size;
    pixel_t pixel_array[200];
} widget_t;

void draw_widget(widget_t widget, uint8_t start_x, uint8_t start_y);

extern widget_t heart_widget;
extern widget_t battery_widget;
extern widget_t clock_widget;
extern widget_t steps_widget;

extern widget_t balkan_widget;

extern widget_t facebook_widget;

extern widget_t gmail_widget;

extern widget_t arrow_widget;


extern widget_t bluetooth_widget;


extern widget_t clk_widget;

extern widget_t tim_widget;


const extern  uint16_t test_img_40x40[][40];