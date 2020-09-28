#include "st7735.h"

#define SPI_INSTANCE 1
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

static inline void spi_write(const void * data, size_t size)
{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, data, size, NULL, 0));
}

static inline void write_command(uint8_t c)
{
    nrf_gpio_pin_clear(NRF_GPIO_PIN_MAP(0, 12));
    spi_write(&c, sizeof(c));
}

static inline void write_data(uint8_t c)
{
    nrf_gpio_pin_set(NRF_GPIO_PIN_MAP(0, 12));
    spi_write(&c, sizeof(c));
}

/******************************************************************************/

void command_list(void)
{
    write_command(ST7735_SWRESET);
    nrf_delay_ms(150);
    write_command(ST7735_SLPOUT);
    nrf_delay_ms(500);

    write_command(ST7735_FRMCTR1);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_command(ST7735_FRMCTR2);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_command(ST7735_FRMCTR3);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);

    write_command(ST7735_INVCTR);
    write_data(0x07);

    write_command(ST7735_PWCTR1);
    write_data(0xA2);
    write_data(0x02);
    write_data(0x84);
    write_command(ST7735_PWCTR2);
    write_data(0xC5);
    write_command(ST7735_PWCTR3);
    write_data(0x0A);
    write_data(0x00);
    write_command(ST7735_PWCTR4);
    write_data(0x8A);
    write_data(0x2A);
    write_command(ST7735_PWCTR5);
    write_data(0x8A);
    write_data(0xEE);

    write_command(ST7735_VMCTR1);
    write_data(0x0E);

    write_command(ST7735_INVOFF);
    write_command(ST7735_MADCTL);
    write_data(ST7735_ROTATION);

    write_command(ST7735_COLMOD);
    write_data(0x05);

    //160*80
    write_command(ST7735_CASET);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x4F);
    write_command(ST7735_RASET);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x9F);
    write_command(ST7735_INVON);
    

    write_command(ST7735_GMCTRP1);
    write_data(0x02);
    write_data(0x1c);
    write_data(0x07);
    write_data(0x12);
    write_data(0x37);
    write_data(0x32);
    write_data(0x29);
    write_data(0x2d);
    write_data(0x29);
    write_data(0x25);
    write_data(0x2b);
    write_data(0x39);
    write_data(0x00);
    write_data(0x01);
    write_data(0x03);
    write_data(0x10);
    write_command(ST7735_GMCTRN1);
    write_data(0x03);
    write_data(0x1d);
    write_data(0x07);
    write_data(0x06);
    write_data(0x2e);
    write_data(0x2c);
    write_data(0x29);
    write_data(0x2d);
    write_data(0x2e);
    write_data(0x2e);
    write_data(0x37);
    write_data(0x3f);
    write_data(0x00);
    write_data(0x00);
    write_data(0x02);
    write_data(0x10);

    write_command(ST7735_NORON);
    nrf_delay_ms(10);
    write_command(ST7735_DISPON);
    nrf_delay_ms(100);

}

void set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    ASSERT(x0 <= x1);
    ASSERT(y0 <= y1);

    write_command(ST7735_CASET);
    write_data(0x00);
    write_data(x0 + ST7735_XSTART);
    write_data(0x00);
    write_data(x1 + ST7735_XSTART);
    write_command(ST7735_RASET);
    write_data(0x00);
    write_data(y0 + ST7735_YSTART);
    write_data(0x00);
    write_data(y1 + ST7735_YSTART);
    write_command(ST7735_RAMWR);
}

ret_code_t hardware_init(void)
{
    ret_code_t err_code;

    nrf_gpio_cfg_output(ST7735_DC_PIN);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

    spi_config.sck_pin  = ST7735_SPI_SCK_PIN;
    spi_config.miso_pin = ST7735_SPI_MISO_PIN;
    spi_config.mosi_pin = ST7735_SPI_MOSI_PIN;
    spi_config.ss_pin   = ST7735_SPI_SS_PIN;

    err_code = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
    return err_code;
}

ret_code_t st7735_init(void)
{
    ret_code_t err_code;

    err_code = hardware_init();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    command_list();

    return err_code;
}

void st7735_uninit(void)
{
    nrf_drv_spi_uninit(&spi);
}

void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
        return;
    }

    set_addr_window(x, y, x + 1, y + 1);
    uint8_t data[] = { color >> 8, color & 0xFF };

    nrf_gpio_pin_set(ST7735_DC_PIN);
    spi_write(data, sizeof(data));
    nrf_gpio_pin_clear(ST7735_DC_PIN);
}

void st7735_write_char(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;

	set_addr_window(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = { color >> 8, color & 0xFF };
                nrf_gpio_pin_set(ST7735_DC_PIN);
				spi_write(data, sizeof(data));
                nrf_gpio_pin_clear(ST7735_DC_PIN);
			} else {
				uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                nrf_gpio_pin_set(ST7735_DC_PIN);
				spi_write(data, sizeof(data));
                nrf_gpio_pin_clear(ST7735_DC_PIN);
			}
		}
	}
}

void ST7735_write_string(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	while (*str) {
		if (x + font.width >= ST7735_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7735_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}

		st7735_write_char(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
}

void st7735_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	// clipping
	if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
		return;
	if ((x + w - 1) >= ST7735_WIDTH)
		w = ST7735_WIDTH - x;
	if ((y + h - 1) >= ST7735_HEIGHT)
		h = ST7735_HEIGHT - y;

	set_addr_window(x, y, x + w - 1, y + h - 1);

	uint8_t data[] = { color >> 8, color & 0xFF };
	nrf_gpio_pin_set(ST7735_DC_PIN);
	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {
			spi_write(data, sizeof(data));
		}
	}
}

void st7735_fill_screen(uint16_t color)
{
	st7735_fill_rectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
		return;
	if ((x + w - 1) >= ST7735_WIDTH)
		return;
	if ((y + h - 1) >= ST7735_HEIGHT)
		return;

	set_addr_window(x, y, x + w - 1, y + h - 1);

    nrf_gpio_pin_set(ST7735_DC_PIN);
    spi_write((uint8_t*) data, sizeof(uint16_t) * w * h);
    nrf_gpio_pin_clear(ST7735_DC_PIN);
}

void st7735_invert_colors(bool invert)
{
	write_command(invert ? ST7735_INVON : ST7735_INVOFF);
}

void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, uint32_t color)
{
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		st7735_draw_pixel(x0 + x, y0 + y, color);
		st7735_draw_pixel(x0 + y, y0 + x, color);
		st7735_draw_pixel(x0 - y, y0 + x, color);
		st7735_draw_pixel(x0 - x, y0 + y, color);
		st7735_draw_pixel(x0 - x, y0 - y, color);
		st7735_draw_pixel(x0 - y, y0 - x, color);
		st7735_draw_pixel(x0 + y, y0 - x, color);
		st7735_draw_pixel(x0 + x, y0 - y, color);

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}






void rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    uint16_t lcd_width = ST7735_WIDTH;
    uint16_t lcd_height = ST7735_HEIGHT;

    if ((x >= lcd_width) || (y >= lcd_height))
    {
        return;
    }

    if (width > (lcd_width - x))
    {
        width = lcd_width - x;
    }

    if (height > (lcd_height - y))
    {
        height = lcd_height - y;
    }

    st7735_fill_rectangle(x, y, width, height, color);
}


void fill_circle_draw(circle_t const * p_circle, uint32_t color)
{
    ASSERT(p_circle != NULL);

    int16_t y = 0;
    int16_t err = 0;
    int16_t x = p_circle->r;

    if ((p_circle->x - p_circle->r > ST7735_WIDTH) || (p_circle->y - p_circle->r > ST7735_HEIGHT))
    {
        return;
    }

    while (x >= y)
    {
        if ((-y + p_circle->x < 0) || (-x + p_circle->x < 0))
        {
            rect_draw(0, (-x + p_circle->y), (y + p_circle->x + 1), 1, color);
            rect_draw(0, (-y + p_circle->y), (x + p_circle->x + 1), 1, color);
            rect_draw(0, (y + p_circle->y), (x + p_circle->x + 1), 1, color);
            rect_draw(0, (x + p_circle->y), (y + p_circle->x + 1), 1, color);
        }
        else
        {
            rect_draw((-y + p_circle->x), (-x + p_circle->y), (2 * y + 1), 1, color);
            rect_draw((-x + p_circle->x), (-y + p_circle->y), (2 * x + 1), 1, color);
            rect_draw((-x + p_circle->x), (y + p_circle->y), (2 * x + 1), 1, color);
            rect_draw((-y + p_circle->x), (x + p_circle->y), (2 * y + 1), 1, color);
        }

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void line_draw(uint16_t x_0, uint16_t y_0, uint16_t x_1, int16_t y_1, uint32_t color)
{
    uint16_t x = x_0;
    uint16_t y = y_0;
    int16_t d;
    int16_t d_1;
    int16_t d_2;
    int16_t ai;
    int16_t bi;
    int16_t xi = (x_0 < x_1) ? 1 : (-1);
    int16_t yi = (y_0 < y_1) ? 1 : (-1);
    bool swapped = false;

    d_1 = abs(x_1 - x_0);
    d_2 = abs(y_1 - y_0);

    st7735_draw_pixel(x, y, color);

    if (d_1 < d_2)
    {
        d_1 = d_1 ^ d_2;
        d_2 = d_1 ^ d_2;
        d_1 = d_2 ^ d_1;
        swapped = true;
    }

    ai = (d_2 - d_1) * 2;
    bi = d_2 * 2;
    d = bi - d_1;

    while ((y != y_1) || (x != x_1))
    {
        if (d >= 0)
        {
            x += xi;
            y += yi;
            d += ai;
        }
        else
        {
            d += bi;
            if (swapped)
            {
                y += yi;
            }
            else
            {
                x += xi;
            }
        }
        st7735_draw_pixel(x, y, color);
    }
}


/******************************************************************************/


void st7735_sleep_in(void)
{
    write_command(ST7735_SLPIN);
}
void st7735_sleep_out(void)
{
    write_command(ST7735_SLPOUT);
}

void draw_widget(widget_t widget, uint8_t start_x, uint8_t start_y)
{
    int i;
    for (i = 0; i < widget.size; i++) {
        st7735_draw_pixel(start_x + widget.pixel_array[i].x, start_y + widget.pixel_array[i].y,
        ST7735_COLOR565(widget.pixel_array[i].color_r, widget.pixel_array[i].color_g, widget.pixel_array[i].color_b));
        //ST7735_COLOR565()
    }
}


widget_t heart_widget
 = 
{
    138,{
    {7, 14, 255, 0, 0},
    {7, 13, 255, 0, 0},
    {7, 12, 255, 0, 0},
    {7, 11, 255, 0, 0},
    {7, 10, 255, 0, 0},
    {7, 9, 255, 0, 0},
    {7, 8, 255, 0, 0},
    {7, 7, 255, 0, 0},
    {7, 6, 255, 0, 0},
    {7, 5, 255, 0, 0},
    {5, 12, 255, 0, 0},
    {6, 13, 255, 0, 0},
    {4, 11, 255, 0, 0},
    {3, 10, 255, 0, 0},
    {2, 9, 255, 0, 0},
    {1, 8, 255, 0, 0},
    {3, 9, 255, 0, 0},
    {2, 8, 255, 0, 0},
    {3, 8, 255, 0, 0},
    {4, 8, 255, 0, 0},
    {5, 8, 255, 0, 0},
    {6, 8, 255, 0, 0},
    {5, 9, 255, 0, 0},
    {4, 9, 255, 0, 0},
    {5, 10, 255, 0, 0},
    {5, 11, 255, 0, 0},
    {6, 11, 255, 0, 0},
    {6, 12, 255, 0, 0},
    {4, 10, 255, 0, 0},
    {6, 10, 255, 0, 0},
    {6, 9, 255, 0, 0},
    {8, 13, 255, 0, 0},
    {9, 12, 255, 0, 0},
    {10, 11, 255, 0, 0},
    {11, 10, 255, 0, 0},
    {12, 9, 255, 0, 0},
    {9, 10, 255, 0, 0},
    {8, 9, 255, 0, 0},
    {9, 9, 255, 0, 0},
    {10, 9, 255, 0, 0},
    {11, 9, 255, 0, 0},
    {10, 10, 255, 0, 0},
    {7, 10, 255, 0, 0},
    {8, 11, 255, 0, 0},
    {8, 10, 255, 0, 0},
    {8, 12, 255, 0, 0},
    {9, 11, 255, 0, 0},
    {8, 8, 255, 0, 0},
    {9, 8, 255, 0, 0},
    {10, 8, 255, 0, 0},
    {11, 8, 255, 0, 0},
    {12, 8, 255, 0, 0},
    {13, 8, 255, 0, 0},
    {1, 7, 255, 0, 0},
    {2, 7, 255, 0, 0},
    {3, 7, 255, 0, 0},
    {4, 7, 255, 0, 0},
    {5, 7, 255, 0, 0},
    {6, 7, 255, 0, 0},
    {8, 7, 255, 0, 0},
    {9, 7, 255, 0, 0},
    {10, 7, 255, 0, 0},
    {11, 7, 255, 0, 0},
    {13, 7, 255, 0, 0},
    {12, 7, 255, 0, 0},
    {2, 6, 255, 0, 0},
    {3, 6, 255, 0, 0},
    {1, 6, 255, 0, 0},
    {1, 5, 255, 0, 0},
    {1, 4, 255, 0, 0},
    {1, 3, 255, 0, 0},
    {2, 4, 255, 0, 0},
    {2, 5, 255, 0, 0},
    {3, 5, 255, 0, 0},
    {3, 3, 255, 0, 0},
    {4, 3, 255, 0, 0},
    {4, 3, 255, 0, 0},
    {6, 5, 255, 0, 0},
    {7, 4, 255, 0, 0},
    {8, 4, 255, 0, 0},
    {8, 5, 255, 0, 0},
    {8, 6, 255, 0, 0},
    {9, 5, 255, 0, 0},
    {9, 6, 255, 0, 0},
    {10, 6, 255, 0, 0},
    {10, 5, 255, 0, 0},
    {11, 5, 255, 0, 0},
    {11, 6, 255, 0, 0},
    {12, 5, 255, 0, 0},
    {13, 5, 255, 0, 0},
    {12, 6, 255, 0, 0},
    {13, 6, 255, 0, 0},
    {2, 3, 255, 0, 0},
    {10, 3, 255, 0, 0},
    {12, 3, 255, 0, 0},
    {11, 3, 255, 0, 0},
    {11, 4, 255, 0, 0},
    {10, 4, 255, 0, 0},
    {9, 4, 255, 0, 0},
    {12, 4, 255, 0, 0},
    {13, 4, 255, 0, 0},
    {0, 6, 255, 0, 0},
    {0, 5, 255, 0, 0},
    {14, 6, 255, 0, 0},
    {14, 5, 255, 0, 0},
    {2, 2, 255, 0, 0},
    {3, 3, 255, 0, 0},
    {3, 2, 255, 0, 0},
    {5, 4, 255, 0, 0},
    {6, 4, 255, 0, 0},
    {6, 6, 255, 0, 0},
    {5, 5, 255, 0, 0},
    {5, 6, 255, 0, 0},
    {4, 6, 255, 0, 0},
    {4, 5, 255, 0, 0},
    {4, 4, 255, 0, 0},
    {3, 4, 255, 0, 0},
    {13, 3, 255, 0, 0},
    {12, 2, 255, 0, 0},
    {11, 2, 255, 0, 0},
    {4, 2, 255, 0, 0},
    {5, 3, 255, 0, 0},
    {8, 3, 255, 0, 0},
    {6, 3, 255, 0, 0},
    {9, 3, 255, 0, 0},
    {10, 2, 255, 0, 0},
    {3, 1, 255, 0, 0},
    {4, 1, 255, 0, 0},
    {5, 2, 255, 0, 0},
    {9, 2, 255, 0, 0},
    {10, 1, 255, 0, 0},
    {11, 1, 255, 0, 0},
    {14, 4, 255, 0, 0},
    {0, 4, 255, 0, 0},
    {1, 2, 255, 0, 0},
    {2, 1, 255, 0, 0},
    {13, 2, 255, 0, 0},
    {12, 1, 255, 0, 0}}
}
;



widget_t battery_widget
 = 
{
    97,{
    {0, 9, 128, 128, 64},
    {0, 8, 128, 128, 64},
    {0, 7, 128, 128, 64},
    {0, 6, 128, 128, 64},
    {0, 5, 128, 128, 64},
    {1, 4, 128, 128, 64},
    {1, 10, 128, 128, 64},
    {2, 10, 128, 128, 64},
    {3, 10, 128, 128, 64},
    {4, 10, 128, 128, 64},
    {2, 4, 128, 128, 64},
    {3, 4, 128, 128, 64},
    {4, 4, 128, 128, 64},
    {5, 4, 128, 128, 64},
    {7, 4, 128, 128, 64},
    {6, 4, 128, 128, 64},
    {6, 10, 128, 128, 64},
    {5, 10, 128, 128, 64},
    {7, 10, 128, 128, 64},
    {8, 10, 128, 128, 64},
    {9, 10, 128, 128, 64},
    {10, 10, 128, 128, 64},
    {8, 4, 128, 128, 64},
    {9, 4, 128, 128, 64},
    {10, 4, 128, 128, 64},
    {11, 4, 128, 128, 64},
    {12, 4, 128, 128, 64},
    {11, 10, 128, 128, 64},
    {12, 10, 128, 128, 64},
    {13, 9, 128, 128, 64},
    {13, 8, 128, 128, 64},
    {13, 7, 128, 128, 64},
    {13, 6, 128, 128, 64},
    {13, 5, 128, 128, 64},
    {14, 6, 128, 128, 64},
    {14, 7, 128, 128, 64},
    {14, 8, 128, 128, 64},
    {3, 9, 128, 128, 64},
    {3, 8, 128, 128, 64},
    {3, 7, 128, 128, 64},
    {3, 6, 128, 128, 64},
    {3, 5, 128, 128, 64},
    {6, 7, 128, 128, 64},
    {6, 9, 128, 128, 64},
    {6, 8, 128, 128, 64},
    {6, 6, 128, 128, 64},
    {6, 5, 128, 128, 64},
    {9, 9, 128, 128, 64},
    {9, 8, 128, 128, 64},
    {9, 7, 128, 128, 64},
    {9, 6, 128, 128, 64},
    {9, 5, 128, 128, 64},
    {12, 5, 128, 128, 64},
    {12, 6, 128, 128, 64},
    {12, 7, 128, 128, 64},
    {12, 8, 128, 128, 64},
    {12, 9, 128, 128, 64},
    {1, 9, 0, 255, 128},
    {1, 8, 0, 255, 128},
    {1, 7, 0, 255, 128},
    {1, 6, 0, 255, 128},
    {1, 5, 0, 255, 128},
    {2, 5, 0, 255, 128},
    {2, 6, 0, 255, 128},
    {2, 7, 0, 255, 128},
    {2, 8, 0, 255, 128},
    {2, 9, 0, 255, 128},
    {4, 9, 0, 255, 128},
    {5, 9, 0, 255, 128},
    {4, 8, 0, 255, 128},
    {5, 8, 0, 255, 128},
    {4, 7, 0, 255, 128},
    {5, 7, 0, 255, 128},
    {4, 6, 0, 255, 128},
    {5, 6, 0, 255, 128},
    {5, 5, 0, 255, 128},
    {4, 5, 0, 255, 128},
    {7, 5, 0, 255, 128},
    {8, 5, 0, 255, 128},
    {8, 6, 0, 255, 128},
    {7, 6, 0, 255, 128},
    {7, 7, 0, 255, 128},
    {8, 7, 0, 255, 128},
    {7, 8, 0, 255, 128},
    {7, 9, 0, 255, 128},
    {8, 9, 0, 255, 128},
    {8, 8, 0, 255, 128},
    {10, 9, 0, 255, 128},
    {11, 9, 0, 255, 128},
    {11, 8, 0, 255, 128},
    {10, 8, 0, 255, 128},
    {10, 7, 0, 255, 128},
    {11, 7, 0, 255, 128},
    {11, 6, 0, 255, 128},
    {10, 6, 0, 255, 128},
    {10, 5, 0, 255, 128},
    {11, 5, 0, 255, 128},
}
}
;



widget_t clock_widget
 = 
{
    59,{
    {7, 13, 255, 255, 255},
    {6, 13, 255, 255, 255},
    {8, 13, 255, 255, 255},
    {7, 7, 0, 0, 255},
    {7, 2, 0, 0, 255},
    {7, 12, 0, 0, 255},
    {12, 7, 0, 0, 255},
    {2, 7, 0, 0, 255},
    {1, 7, 0, 0, 255},
    {1, 6, 0, 0, 255},
    {13, 7, 0, 0, 255},
    {13, 6, 0, 0, 255},
    {12, 5, 0, 0, 255},
    {2, 5, 0, 0, 255},
    {2, 4, 0, 0, 255},
    {3, 3, 0, 0, 255},
    {12, 4, 0, 0, 255},
    {11, 3, 0, 0, 255},
    {4, 2, 0, 0, 255},
    {10, 2, 0, 0, 255},
    {1, 8, 0, 0, 255},
    {2, 9, 0, 0, 255},
    {2, 10, 0, 0, 255},
    {3, 11, 0, 0, 255},
    {13, 8, 0, 0, 255},
    {12, 9, 0, 0, 255},
    {12, 10, 0, 0, 255},
    {11, 11, 0, 0, 255},
    {10, 12, 0, 0, 255},
    {4, 12, 0, 0, 255},
    {6, 13, 0, 0, 255},
    {7, 13, 0, 0, 255},
    {8, 13, 0, 0, 255},
    {6, 1, 0, 0, 255},
    {7, 1, 0, 0, 255},
    {8, 1, 0, 0, 255},
    {5, 12, 0, 0, 255},
    {9, 12, 0, 0, 255},
    {5, 2, 0, 0, 255},
    {9, 2, 0, 0, 255},
    {12, 2, 0, 0, 255},
    {13, 1, 0, 0, 255},
    {12, 0, 0, 0, 255},
    {14, 2, 0, 0, 255},
    {2, 2, 0, 0, 255},
    {1, 1, 0, 0, 255},
    {2, 0, 0, 0, 255},
    {0, 2, 0, 0, 255},
    {7, 3, 0, 0, 255},
    {3, 7, 0, 0, 255},
    {7, 11, 0, 0, 255},
    {11, 7, 0, 0, 255},
    {8, 6, 0, 0, 255},
    {9, 5, 0, 0, 255},
    {10, 4, 0, 0, 255},
    {6, 7, 0, 0, 255},
    {7, 8, 0, 0, 255},
    {7, 6, 0, 0, 255},
    {8, 7, 0, 0, 255},
}
}
;




widget_t steps_widget
 = 
{
    71,{
    {3, 13, 0, 255, 128},
    {4, 13, 0, 255, 128},
    {5, 13, 0, 255, 128},
    {3, 12, 0, 255, 128},
    {4, 12, 0, 255, 128},
    {5, 12, 0, 255, 128},
    {4, 14, 0, 255, 128},
    {10, 14, 0, 255, 128},
    {9, 13, 0, 255, 128},
    {10, 13, 0, 255, 128},
    {11, 13, 0, 255, 128},
    {9, 12, 0, 255, 128},
    {10, 12, 0, 255, 128},
    {11, 12, 0, 255, 128},
    {3, 10, 0, 255, 128},
    {4, 10, 0, 255, 128},
    {5, 10, 0, 255, 128},
    {10, 10, 0, 255, 128},
    {11, 10, 0, 255, 128},
    {9, 10, 0, 255, 128},
    {3, 9, 0, 255, 128},
    {4, 9, 0, 255, 128},
    {5, 9, 0, 255, 128},
    {4, 8, 0, 255, 128},
    {5, 8, 0, 255, 128},
    {3, 8, 0, 255, 128},
    {9, 9, 0, 255, 128},
    {9, 8, 0, 255, 128},
    {10, 8, 0, 255, 128},
    {10, 9, 0, 255, 128},
    {11, 9, 0, 255, 128},
    {11, 8, 0, 255, 128},
    {6, 8, 0, 255, 128},
    {6, 7, 0, 255, 128},
    {6, 6, 0, 255, 128},
    {8, 8, 0, 255, 128},
    {8, 7, 0, 255, 128},
    {8, 6, 0, 255, 128},
    {2, 9, 0, 255, 128},
    {2, 7, 0, 255, 128},
    {2, 8, 0, 255, 128},
    {3, 6, 0, 255, 128},
    {2, 6, 0, 255, 128},
    {3, 5, 0, 255, 128},
    {4, 4, 0, 255, 128},
    {5, 4, 0, 255, 128},
    {5, 5, 0, 255, 128},
    {5, 6, 0, 255, 128},
    {4, 5, 0, 255, 128},
    {4, 6, 0, 255, 128},
    {4, 7, 0, 255, 128},
    {4, 7, 0, 255, 128},
    {5, 7, 0, 255, 128},
    {3, 7, 0, 255, 128},
    {9, 5, 0, 255, 128},
    {9, 4, 0, 255, 128},
    {10, 5, 0, 255, 128},
    {10, 4, 0, 255, 128},
    {11, 5, 0, 255, 128},
    {10, 6, 0, 255, 128},
    {9, 7, 0, 255, 128},
    {9, 6, 0, 255, 128},
    {10, 7, 0, 255, 128},
    {11, 6, 0, 255, 128},
    {11, 7, 0, 255, 128},
    {12, 9, 0, 255, 128},
    {12, 8, 0, 255, 128},
    {12, 7, 0, 255, 128},
    {12, 6, 0, 255, 128},
    {6, 5, 0, 255, 128},
    {8, 5, 0, 255, 128},
}
}
;






widget_t balkan_widget
 = 
{
    173,{
    {7, 14, 0, 0, 0},
    {7, 13, 0, 0, 0},
    {7, 12, 0, 0, 0},
    {7, 11, 0, 0, 0},
    {7, 10, 0, 0, 0},
    {7, 9, 0, 0, 0},
    {7, 8, 0, 0, 0},
    {7, 7, 0, 0, 0},
    {7, 6, 0, 0, 0},
    {7, 5, 0, 0, 0},
    {7, 4, 0, 0, 0},
    {7, 3, 0, 0, 0},
    {7, 2, 0, 0, 0},
    {7, 1, 0, 0, 0},
    {7, 0, 0, 0, 0},
    {6, 0, 0, 0, 0},
    {6, 1, 0, 0, 0},
    {6, 2, 0, 0, 0},
    {6, 3, 0, 0, 0},
    {6, 4, 0, 0, 0},
    {6, 5, 0, 0, 0},
    {6, 6, 0, 0, 0},
    {6, 7, 0, 0, 0},
    {6, 8, 0, 0, 0},
    {6, 9, 0, 0, 0},
    {6, 10, 0, 0, 0},
    {6, 11, 0, 0, 0},
    {6, 12, 0, 0, 0},
    {6, 13, 0, 0, 0},
    {6, 14, 0, 0, 0},
    {8, 1, 0, 0, 0},
    {8, 0, 0, 0, 0},
    {8, 2, 0, 0, 0},
    {8, 3, 0, 0, 0},
    {8, 4, 0, 0, 0},
    {8, 5, 0, 0, 0},
    {8, 6, 0, 0, 0},
    {8, 7, 0, 0, 0},
    {8, 8, 0, 0, 0},
    {8, 9, 0, 0, 0},
    {8, 10, 0, 0, 0},
    {8, 11, 0, 0, 0},
    {8, 12, 0, 0, 0},
    {8, 13, 0, 0, 0},
    {8, 14, 0, 0, 0},
    {5, 7, 0, 0, 0},
    {4, 7, 0, 0, 0},
    {3, 7, 0, 0, 0},
    {2, 7, 0, 0, 0},
    {1, 7, 0, 0, 0},
    {0, 7, 0, 0, 0},
    {0, 6, 0, 0, 0},
    {1, 6, 0, 0, 0},
    {3, 6, 0, 0, 0},
    {2, 6, 0, 0, 0},
    {4, 6, 0, 0, 0},
    {5, 6, 0, 0, 0},
    {2, 8, 0, 0, 0},
    {1, 8, 0, 0, 0},
    {0, 8, 0, 0, 0},
    {3, 8, 0, 0, 0},
    {4, 8, 0, 0, 0},
    {5, 8, 0, 0, 0},
    {9, 8, 0, 0, 0},
    {9, 7, 0, 0, 0},
    {9, 6, 0, 0, 0},
    {10, 6, 0, 0, 0},
    {10, 7, 0, 0, 0},
    {10, 8, 0, 0, 0},
    {11, 8, 0, 0, 0},
    {11, 7, 0, 0, 0},
    {11, 6, 0, 0, 0},
    {12, 6, 0, 0, 0},
    {12, 7, 0, 0, 0},
    {12, 8, 0, 0, 0},
    {13, 8, 0, 0, 0},
    {13, 7, 0, 0, 0},
    {13, 6, 0, 0, 0},
    {14, 6, 0, 0, 0},
    {14, 7, 0, 0, 0},
    {14, 8, 0, 0, 0},
    {0, 4, 0, 0, 0},
    {1, 4, 0, 0, 0},
    {2, 4, 0, 0, 0},
    {3, 4, 0, 0, 0},
    {4, 4, 0, 0, 0},
    {4, 3, 0, 0, 0},
    {4, 2, 0, 0, 0},
    {4, 1, 0, 0, 0},
    {4, 0, 0, 0, 0},
    {10, 4, 0, 0, 0},
    {10, 3, 0, 0, 0},
    {10, 2, 0, 0, 0},
    {10, 1, 0, 0, 0},
    {10, 0, 0, 0, 0},
    {11, 4, 0, 0, 0},
    {12, 4, 0, 0, 0},
    {13, 4, 0, 0, 0},
    {14, 4, 0, 0, 0},
    {1, 10, 0, 0, 0},
    {0, 10, 0, 0, 0},
    {2, 10, 0, 0, 0},
    {3, 10, 0, 0, 0},
    {4, 10, 0, 0, 0},
    {4, 14, 0, 0, 0},
    {4, 13, 0, 0, 0},
    {4, 12, 0, 0, 0},
    {4, 11, 0, 0, 0},
    {10, 14, 0, 0, 0},
    {10, 13, 0, 0, 0},
    {10, 11, 0, 0, 0},
    {10, 12, 0, 0, 0},
    {10, 10, 0, 0, 0},
    {14, 10, 0, 0, 0},
    {13, 10, 0, 0, 0},
    {12, 10, 0, 0, 0},
    {11, 10, 0, 0, 0},
    {0, 9, 255, 255, 255},
    {1, 9, 255, 255, 255},
    {2, 9, 255, 255, 255},
    {3, 9, 255, 255, 255},
    {4, 9, 255, 255, 255},
    {5, 9, 255, 255, 255},
    {5, 10, 255, 255, 255},
    {5, 11, 255, 255, 255},
    {5, 12, 255, 255, 255},
    {5, 13, 255, 255, 255},
    {5, 14, 255, 255, 255},
    {9, 14, 255, 255, 255},
    {9, 13, 255, 255, 255},
    {9, 12, 255, 255, 255},
    {9, 11, 255, 255, 255},
    {9, 10, 255, 255, 255},
    {9, 9, 255, 255, 255},
    {10, 9, 255, 255, 255},
    {11, 9, 255, 255, 255},
    {12, 9, 255, 255, 255},
    {13, 9, 255, 255, 255},
    {14, 9, 255, 255, 255},
    {0, 5, 255, 255, 255},
    {1, 5, 255, 255, 255},
    {2, 5, 255, 255, 255},
    {3, 5, 255, 255, 255},
    {4, 5, 255, 255, 255},
    {5, 5, 255, 255, 255},
    {5, 4, 255, 255, 255},
    {5, 3, 255, 255, 255},
    {5, 2, 255, 255, 255},
    {5, 1, 255, 255, 255},
    {5, 0, 255, 255, 255},
    {8, 0, 255, 255, 255},
    {9, 0, 255, 255, 255},
    {9, 1, 255, 255, 255},
    {9, 2, 255, 255, 255},
    {9, 3, 255, 255, 255},
    {9, 4, 255, 255, 255},
    {9, 5, 255, 255, 255},
    {10, 5, 255, 255, 255},
    {11, 5, 255, 255, 255},
    {12, 5, 255, 255, 255},
    {13, 5, 255, 255, 255},
    {14, 5, 255, 255, 255},
    {6, 0, 255, 255, 255},
    {0, 6, 255, 255, 255},
    {0, 8, 255, 255, 255},
    {6, 14, 255, 255, 255},
    {8, 14, 255, 255, 255},
    {14, 8, 255, 255, 255},
    {14, 6, 255, 255, 255},
    {10, 10, 255, 255, 255},
    {4, 10, 255, 255, 255},
    {4, 4, 255, 255, 255},
    {10, 4, 255, 255, 255},
}
}
;




widget_t facebook_widget
 = 
{
    176,{
    {3, 3, 45, 68, 133},
    {11, 3, 45, 68, 133},
    {3, 11, 45, 68, 133},
    {11, 11, 45, 68, 133},
    {4, 11, 45, 68, 133},
    {5, 11, 45, 68, 133},
    {6, 11, 45, 68, 133},
    {7, 11, 45, 68, 133},
    {8, 11, 45, 68, 133},
    {9, 11, 45, 68, 133},
    {10, 11, 45, 68, 133},
    {3, 10, 45, 68, 133},
    {3, 9, 45, 68, 133},
    {3, 8, 45, 68, 133},
    {3, 7, 45, 68, 133},
    {3, 6, 45, 68, 133},
    {3, 5, 45, 68, 133},
    {3, 4, 45, 68, 133},
    {4, 2, 45, 68, 133},
    {4, 3, 45, 68, 133},
    {5, 3, 45, 68, 133},
    {6, 3, 45, 68, 133},
    {7, 3, 45, 68, 133},
    {8, 3, 45, 68, 133},
    {9, 3, 45, 68, 133},
    {10, 3, 45, 68, 133},
    {11, 4, 45, 68, 133},
    {11, 5, 45, 68, 133},
    {11, 6, 45, 68, 133},
    {11, 7, 45, 68, 133},
    {11, 8, 45, 68, 133},
    {11, 9, 45, 68, 133},
    {11, 10, 45, 68, 133},
    {2, 4, 45, 68, 133},
    {10, 2, 45, 68, 133},
    {12, 4, 45, 68, 133},
    {2, 10, 45, 68, 133},
    {4, 12, 45, 68, 133},
    {10, 12, 45, 68, 133},
    {12, 10, 45, 68, 133},
    {12, 9, 45, 68, 133},
    {12, 8, 45, 68, 133},
    {12, 7, 45, 68, 133},
    {12, 6, 45, 68, 133},
    {12, 5, 45, 68, 133},
    {5, 2, 45, 68, 133},
    {6, 2, 45, 68, 133},
    {7, 2, 45, 68, 133},
    {8, 2, 45, 68, 133},
    {9, 2, 45, 68, 133},
    {2, 5, 45, 68, 133},
    {2, 6, 45, 68, 133},
    {2, 7, 45, 68, 133},
    {2, 9, 45, 68, 133},
    {2, 8, 45, 68, 133},
    {5, 12, 45, 68, 133},
    {6, 12, 45, 68, 133},
    {7, 12, 45, 68, 133},
    {8, 12, 45, 68, 133},
    {9, 12, 45, 68, 133},
    {4, 10, 45, 68, 133},
    {4, 9, 45, 68, 133},
    {4, 8, 45, 68, 133},
    {4, 7, 45, 68, 133},
    {4, 6, 45, 68, 133},
    {4, 5, 45, 68, 133},
    {4, 4, 45, 68, 133},
    {5, 4, 45, 68, 133},
    {7, 4, 45, 68, 133},
    {8, 4, 45, 68, 133},
    {10, 4, 45, 68, 133},
    {9, 4, 45, 68, 133},
    {6, 4, 45, 68, 133},
    {5, 5, 45, 68, 133},
    {6, 6, 45, 68, 133},
    {5, 6, 45, 68, 133},
    {5, 7, 45, 68, 133},
    {5, 8, 45, 68, 133},
    {5, 9, 45, 68, 133},
    {5, 10, 45, 68, 133},
    {6, 10, 45, 68, 133},
    {8, 10, 45, 68, 133},
    {7, 10, 45, 68, 133},
    {9, 10, 45, 68, 133},
    {10, 10, 45, 68, 133},
    {10, 9, 45, 68, 133},
    {10, 8, 45, 68, 133},
    {10, 7, 45, 68, 133},
    {10, 5, 45, 68, 133},
    {10, 5, 45, 68, 133},
    {9, 6, 45, 68, 133},
    {9, 5, 45, 68, 133},
    {11, 6, 45, 68, 133},
    {10, 6, 45, 68, 133},
    {6, 5, 45, 68, 133},
    {7, 5, 45, 68, 133},
    {8, 5, 45, 68, 133},
    {7, 6, 45, 68, 133},
    {8, 6, 45, 68, 133},
    {8, 7, 45, 68, 133},
    {9, 7, 45, 68, 133},
    {9, 8, 45, 68, 133},
    {9, 8, 45, 68, 133},
    {8, 9, 45, 68, 133},
    {8, 8, 45, 68, 133},
    {6, 9, 45, 68, 133},
    {7, 9, 45, 68, 133},
    {9, 9, 45, 68, 133},
    {7, 8, 45, 68, 133},
    {7, 7, 45, 68, 133},
    {6, 7, 45, 68, 133},
    {6, 8, 45, 68, 133},
    {2, 3, 45, 68, 133},
    {3, 2, 45, 68, 133},
    {2, 11, 45, 68, 133},
    {3, 12, 45, 68, 133},
    {11, 12, 45, 68, 133},
    {12, 11, 45, 68, 133},
    {12, 3, 45, 68, 133},
    {11, 2, 45, 68, 133},
    {8, 12, 255, 255, 255},
    {9, 12, 255, 255, 255},
    {8, 11, 255, 255, 255},
    {9, 11, 255, 255, 255},
    {8, 10, 255, 255, 255},
    {9, 10, 255, 255, 255},
    {8, 9, 255, 255, 255},
    {9, 9, 255, 255, 255},
    {8, 8, 255, 255, 255},
    {9, 8, 255, 255, 255},
    {8, 7, 255, 255, 255},
    {9, 7, 255, 255, 255},
    {8, 6, 255, 255, 255},
    {9, 6, 255, 255, 255},
    {8, 5, 255, 255, 255},
    {7, 8, 255, 255, 255},
    {7, 7, 255, 255, 255},
    {10, 8, 255, 255, 255},
    {10, 7, 255, 255, 255},
    {8, 4, 255, 255, 255},
    {9, 3, 255, 255, 255},
    {10, 3, 255, 255, 255},
    {9, 4, 255, 255, 255},
    {9, 5, 255, 255, 255},
    {10, 4, 255, 255, 255},
    {11, 4, 255, 255, 255},
    {11, 3, 255, 255, 255},
    {11, 8, 255, 255, 255},
    {11, 6, 255, 255, 255},
    {11, 7, 255, 255, 255},
    {6, 8, 255, 255, 255},
    {6, 6, 255, 255, 255},
    {6, 7, 255, 255, 255},
    {6, 6, 45, 68, 133},
    {11, 6, 45, 68, 133},
    {7, 13, 45, 68, 133},
    {10, 13, 45, 68, 133},
    {11, 13, 45, 68, 133},
    {12, 12, 45, 68, 133},
    {13, 11, 45, 68, 133},
    {13, 10, 45, 68, 133},
    {13, 9, 45, 68, 133},
    {13, 8, 45, 68, 133},
    {13, 7, 45, 68, 133},
    {13, 6, 45, 68, 133},
    {13, 4, 45, 68, 133},
    {13, 5, 45, 68, 133},
    {6, 13, 45, 68, 133},
    {5, 13, 45, 68, 133},
    {4, 13, 45, 68, 133},
    {3, 13, 45, 68, 133},
    {2, 12, 45, 68, 133},
    {12, 13, 45, 68, 133},
    {13, 12, 45, 68, 133},
    {12, 2, 45, 68, 133},
    {13, 3, 45, 68, 133},
}
}
;




widget_t gmail_widget
 = 
{
    140,{
    {0, 12, 233, 65, 52},
    {1, 12, 233, 65, 52},
    {1, 11, 233, 65, 52},
    {0, 11, 233, 65, 52},
    {0, 10, 233, 65, 52},
    {1, 10, 233, 65, 52},
    {0, 9, 233, 65, 52},
    {1, 9, 233, 65, 52},
    {0, 8, 233, 65, 52},
    {1, 8, 233, 65, 52},
    {0, 7, 233, 65, 52},
    {1, 7, 233, 65, 52},
    {0, 6, 233, 65, 52},
    {1, 6, 233, 65, 52},
    {0, 5, 233, 65, 52},
    {1, 5, 233, 65, 52},
    {0, 4, 233, 65, 52},
    {1, 4, 233, 65, 52},
    {1, 3, 233, 65, 52},
    {2, 3, 233, 65, 52},
    {2, 4, 233, 65, 52},
    {13, 12, 233, 65, 52},
    {12, 12, 233, 65, 52},
    {12, 11, 233, 65, 52},
    {13, 11, 233, 65, 52},
    {13, 10, 233, 65, 52},
    {12, 10, 233, 65, 52},
    {12, 9, 233, 65, 52},
    {13, 9, 233, 65, 52},
    {12, 8, 233, 65, 52},
    {13, 8, 233, 65, 52},
    {12, 7, 233, 65, 52},
    {13, 7, 233, 65, 52},
    {12, 6, 233, 65, 52},
    {13, 6, 233, 65, 52},
    {12, 5, 233, 65, 52},
    {13, 5, 233, 65, 52},
    {13, 4, 233, 65, 52},
    {12, 4, 233, 65, 52},
    {12, 3, 233, 65, 52},
    {11, 3, 233, 65, 52},
    {11, 4, 233, 65, 52},
    {6, 8, 233, 65, 52},
    {7, 8, 233, 65, 52},
    {8, 7, 233, 65, 52},
    {9, 6, 233, 65, 52},
    {10, 5, 233, 65, 52},
    {5, 7, 233, 65, 52},
    {4, 6, 233, 65, 52},
    {3, 5, 233, 65, 52},
    {3, 4, 233, 65, 52},
    {4, 5, 233, 65, 52},
    {5, 6, 233, 65, 52},
    {6, 7, 233, 65, 52},
    {7, 7, 233, 65, 52},
    {8, 6, 233, 65, 52},
    {9, 5, 233, 65, 52},
    {10, 4, 233, 65, 52},
    {0, 3, 255, 255, 255},
    {3, 3, 255, 255, 255},
    {4, 3, 255, 255, 255},
    {5, 3, 255, 255, 255},
    {6, 3, 255, 255, 255},
    {7, 3, 255, 255, 255},
    {8, 3, 255, 255, 255},
    {9, 3, 255, 255, 255},
    {10, 3, 255, 255, 255},
    {13, 3, 255, 255, 255},
    {9, 4, 255, 255, 255},
    {8, 4, 255, 255, 255},
    {7, 4, 255, 255, 255},
    {6, 4, 255, 255, 255},
    {5, 4, 255, 255, 255},
    {4, 4, 255, 255, 255},
    {5, 5, 255, 255, 255},
    {6, 5, 255, 255, 255},
    {6, 6, 255, 255, 255},
    {7, 6, 255, 255, 255},
    {7, 5, 255, 255, 255},
    {8, 5, 255, 255, 255},
    {2, 12, 255, 255, 255},
    {2, 11, 255, 255, 255},
    {2, 10, 255, 255, 255},
    {2, 9, 255, 255, 255},
    {2, 8, 255, 255, 255},
    {2, 7, 255, 255, 255},
    {2, 6, 255, 255, 255},
    {2, 5, 255, 255, 255},
    {3, 6, 255, 255, 255},
    {3, 7, 255, 255, 255},
    {3, 8, 255, 255, 255},
    {3, 9, 255, 255, 255},
    {3, 10, 255, 255, 255},
    {3, 11, 255, 255, 255},
    {3, 12, 255, 255, 255},
    {4, 12, 255, 255, 255},
    {4, 11, 255, 255, 255},
    {4, 10, 255, 255, 255},
    {4, 9, 255, 255, 255},
    {4, 8, 255, 255, 255},
    {4, 7, 255, 255, 255},
    {5, 8, 255, 255, 255},
    {5, 9, 255, 255, 255},
    {5, 10, 255, 255, 255},
    {5, 11, 255, 255, 255},
    {5, 12, 255, 255, 255},
    {6, 12, 255, 255, 255},
    {7, 12, 255, 255, 255},
    {7, 11, 255, 255, 255},
    {6, 11, 255, 255, 255},
    {6, 10, 255, 255, 255},
    {7, 10, 255, 255, 255},
    {7, 9, 255, 255, 255},
    {6, 9, 255, 255, 255},
    {8, 8, 255, 255, 255},
    {8, 9, 255, 255, 255},
    {8, 10, 255, 255, 255},
    {8, 11, 255, 255, 255},
    {8, 12, 255, 255, 255},
    {9, 12, 255, 255, 255},
    {9, 11, 255, 255, 255},
    {9, 10, 255, 255, 255},
    {9, 9, 255, 255, 255},
    {9, 8, 255, 255, 255},
    {9, 7, 255, 255, 255},
    {10, 6, 255, 255, 255},
    {10, 7, 255, 255, 255},
    {10, 8, 255, 255, 255},
    {10, 9, 255, 255, 255},
    {10, 10, 255, 255, 255},
    {10, 11, 255, 255, 255},
    {10, 12, 255, 255, 255},
    {11, 12, 255, 255, 255},
    {11, 11, 255, 255, 255},
    {11, 10, 255, 255, 255},
    {11, 9, 255, 255, 255},
    {11, 8, 255, 255, 255},
    {11, 7, 255, 255, 255},
    {11, 6, 255, 255, 255},
    {11, 5, 255, 255, 255},
}
}
;

widget_t arrow_widget
 = 
{
    199,{
    {0, 6, 75, 109, 245},
    {0, 8, 75, 109, 245},
    {1, 6, 75, 109, 245},
    {2, 6, 75, 109, 245},
    {3, 6, 75, 109, 245},
    {1, 5, 75, 109, 245},
    {3, 5, 75, 109, 245},
    {2, 5, 75, 109, 245},
    {2, 4, 75, 109, 245},
    {3, 4, 75, 109, 245},
    {3, 3, 75, 109, 245},
    {4, 3, 75, 109, 245},
    {5, 3, 75, 109, 245},
    {6, 3, 75, 109, 245},
    {4, 2, 75, 109, 245},
    {5, 2, 75, 109, 245},
    {5, 1, 75, 109, 245},
    {6, 0, 75, 109, 245},
    {6, 1, 75, 109, 245},
    {6, 2, 75, 109, 245},
    {7, 1, 75, 109, 245},
    {7, 2, 75, 109, 245},
    {7, 3, 75, 109, 245},
    {8, 3, 75, 109, 245},
    {8, 2, 75, 109, 245},
    {9, 3, 75, 109, 245},
    {10, 4, 75, 109, 245},
    {4, 4, 75, 109, 245},
    {4, 5, 75, 109, 245},
    {4, 6, 75, 109, 245},
    {1, 8, 75, 109, 245},
    {2, 8, 75, 109, 245},
    {3, 8, 75, 109, 245},
    {1, 9, 75, 109, 245},
    {2, 10, 75, 109, 245},
    {2, 9, 75, 109, 245},
    {3, 9, 75, 109, 245},
    {3, 10, 75, 109, 245},
    {3, 11, 75, 109, 245},
    {4, 10, 75, 109, 245},
    {4, 9, 75, 109, 245},
    {4, 8, 75, 109, 245},
    {5, 6, 75, 109, 245},
    {5, 5, 75, 109, 245},
    {5, 4, 75, 109, 245},
    {6, 4, 75, 109, 245},
    {7, 4, 75, 109, 245},
    {8, 4, 75, 109, 245},
    {9, 4, 75, 109, 245},
    {11, 5, 75, 109, 245},
    {12, 6, 75, 109, 245},
    {11, 6, 75, 109, 245},
    {10, 6, 75, 109, 245},
    {9, 6, 75, 109, 245},
    {8, 6, 75, 109, 245},
    {7, 6, 75, 109, 245},
    {6, 6, 75, 109, 245},
    {5, 5, 75, 109, 245},
    {6, 5, 75, 109, 245},
    {7, 5, 75, 109, 245},
    {8, 5, 75, 109, 245},
    {9, 5, 75, 109, 245},
    {10, 5, 75, 109, 245},
    {5, 8, 75, 109, 245},
    {6, 8, 75, 109, 245},
    {7, 8, 75, 109, 245},
    {8, 8, 75, 109, 245},
    {4, 11, 75, 109, 245},
    {4, 12, 75, 109, 245},
    {5, 13, 75, 109, 245},
    {6, 14, 75, 109, 245},
    {6, 13, 75, 109, 245},
    {7, 13, 75, 109, 245},
    {7, 12, 75, 109, 245},
    {6, 12, 75, 109, 245},
    {5, 12, 75, 109, 245},
    {5, 11, 75, 109, 245},
    {9, 8, 75, 109, 245},
    {10, 8, 75, 109, 245},
    {11, 8, 75, 109, 245},
    {12, 8, 75, 109, 245},
    {11, 9, 75, 109, 245},
    {10, 10, 75, 109, 245},
    {8, 11, 75, 109, 245},
    {9, 11, 75, 109, 245},
    {8, 12, 75, 109, 245},
    {6, 11, 75, 109, 245},
    {7, 11, 75, 109, 245},
    {7, 10, 75, 109, 245},
    {8, 10, 75, 109, 245},
    {9, 10, 75, 109, 245},
    {10, 9, 75, 109, 245},
    {9, 9, 75, 109, 245},
    {8, 9, 75, 109, 245},
    {7, 9, 75, 109, 245},
    {6, 9, 75, 109, 245},
    {5, 9, 75, 109, 245},
    {5, 9, 75, 109, 245},
    {6, 10, 75, 109, 245},
    {5, 10, 75, 109, 245},
    {0, 6, 255, 255, 255},
    {1, 6, 255, 255, 255},
    {1, 5, 255, 255, 255},
    {2, 5, 255, 255, 255},
    {2, 4, 255, 255, 255},
    {2, 6, 255, 255, 255},
    {3, 6, 255, 255, 255},
    {3, 5, 255, 255, 255},
    {3, 4, 255, 255, 255},
    {3, 3, 255, 255, 255},
    {4, 2, 255, 255, 255},
    {4, 3, 255, 255, 255},
    {4, 4, 255, 255, 255},
    {4, 5, 255, 255, 255},
    {4, 6, 255, 255, 255},
    {6, 0, 255, 255, 255},
    {6, 1, 255, 255, 255},
    {5, 1, 255, 255, 255},
    {7, 1, 255, 255, 255},
    {5, 2, 255, 255, 255},
    {6, 2, 255, 255, 255},
    {7, 2, 255, 255, 255},
    {8, 2, 255, 255, 255},
    {9, 3, 255, 255, 255},
    {8, 3, 255, 255, 255},
    {7, 3, 255, 255, 255},
    {6, 3, 255, 255, 255},
    {5, 3, 255, 255, 255},
    {5, 4, 255, 255, 255},
    {6, 4, 255, 255, 255},
    {7, 4, 255, 255, 255},
    {8, 4, 255, 255, 255},
    {9, 4, 255, 255, 255},
    {10, 4, 255, 255, 255},
    {11, 5, 255, 255, 255},
    {10, 5, 255, 255, 255},
    {9, 5, 255, 255, 255},
    {8, 5, 255, 255, 255},
    {7, 5, 255, 255, 255},
    {6, 5, 255, 255, 255},
    {5, 5, 255, 255, 255},
    {12, 6, 255, 255, 255},
    {11, 6, 255, 255, 255},
    {9, 6, 255, 255, 255},
    {10, 6, 255, 255, 255},
    {7, 6, 255, 255, 255},
    {8, 6, 255, 255, 255},
    {6, 6, 255, 255, 255},
    {5, 6, 255, 255, 255},
    {6, 14, 255, 255, 255},
    {5, 13, 255, 255, 255},
    {6, 13, 255, 255, 255},
    {7, 12, 255, 255, 255},
    {7, 13, 255, 255, 255},
    {8, 12, 255, 255, 255},
    {4, 12, 255, 255, 255},
    {5, 12, 255, 255, 255},
    {6, 12, 255, 255, 255},
    {0, 8, 255, 255, 255},
    {1, 8, 255, 255, 255},
    {1, 8, 255, 255, 255},
    {1, 9, 255, 255, 255},
    {2, 9, 255, 255, 255},
    {2, 8, 255, 255, 255},
    {3, 8, 255, 255, 255},
    {3, 9, 255, 255, 255},
    {2, 10, 255, 255, 255},
    {3, 10, 255, 255, 255},
    {4, 11, 255, 255, 255},
    {4, 10, 255, 255, 255},
    {3, 11, 255, 255, 255},
    {4, 9, 255, 255, 255},
    {4, 8, 255, 255, 255},
    {5, 8, 255, 255, 255},
    {5, 9, 255, 255, 255},
    {5, 10, 255, 255, 255},
    {5, 11, 255, 255, 255},
    {11, 8, 255, 255, 255},
    {12, 8, 255, 255, 255},
    {11, 9, 255, 255, 255},
    {10, 8, 255, 255, 255},
    {10, 9, 255, 255, 255},
    {10, 10, 255, 255, 255},
    {9, 10, 255, 255, 255},
    {9, 11, 255, 255, 255},
    {8, 11, 255, 255, 255},
    {8, 10, 255, 255, 255},
    {8, 9, 255, 255, 255},
    {9, 9, 255, 255, 255},
    {9, 8, 255, 255, 255},
    {8, 8, 255, 255, 255},
    {7, 8, 255, 255, 255},
    {6, 8, 255, 255, 255},
    {6, 9, 255, 255, 255},
    {7, 9, 255, 255, 255},
    {7, 10, 255, 255, 255},
    {6, 10, 255, 255, 255},
    {6, 11, 255, 255, 255},
    {7, 11, 255, 255, 255},
}
}
;

widget_t bluetooth_widget
 = 
{
    45,{
    {10, 18, 10, 60, 144},
    {10, 19, 10, 60, 144},
    {10, 17, 10, 60, 144},
    {10, 16, 10, 60, 144},
    {10, 15, 10, 60, 144},
    {10, 1, 10, 60, 144},
    {10, 2, 10, 60, 144},
    {10, 3, 10, 60, 144},
    {10, 4, 10, 60, 144},
    {10, 5, 10, 60, 144},
    {10, 6, 10, 60, 144},
    {10, 7, 10, 60, 144},
    {10, 8, 10, 60, 144},
    {10, 9, 10, 60, 144},
    {10, 10, 10, 60, 144},
    {10, 11, 10, 60, 144},
    {10, 12, 10, 60, 144},
    {10, 12, 10, 60, 144},
    {10, 13, 10, 60, 144},
    {10, 14, 10, 60, 144},
    {11, 18, 10, 60, 144},
    {12, 17, 10, 60, 144},
    {13, 16, 10, 60, 144},
    {14, 15, 10, 60, 144},
    {11, 2, 10, 60, 144},
    {12, 3, 10, 60, 144},
    {13, 4, 10, 60, 144},
    {14, 5, 10, 60, 144},
    {15, 6, 10, 60, 144},
    {15, 14, 10, 60, 144},
    {14, 13, 10, 60, 144},
    {13, 12, 10, 60, 144},
    {12, 11, 10, 60, 144},
    {11, 10, 10, 60, 144},
    {9, 8, 10, 60, 144},
    {8, 7, 10, 60, 144},
    {7, 6, 10, 60, 144},
    {14, 7, 10, 60, 144},
    {13, 8, 10, 60, 144},
    {12, 9, 10, 60, 144},
    {9, 12, 10, 60, 144},
    {8, 13, 10, 60, 144},
    {7, 14, 10, 60, 144},
    {6, 15, 10, 60, 144},
    {6, 5, 10, 60, 144},
}
}
;
