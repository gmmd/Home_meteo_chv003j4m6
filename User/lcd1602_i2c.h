#ifndef __LCD1602_I2C_H
#define __LCD1602_I2C_H

#include "ch32v00x.h"

// I2C Configuration
#define LCD_I2C I2C1
#define LCD_I2C_CLK RCC_APB1Periph_I2C1
#define LCD_I2C_GPIO_CLK RCC_APB2Periph_GPIOC

#define LCD_I2C_SCL_PORT GPIOC
#define LCD_I2C_SCL_PIN GPIO_Pin_2

#define LCD_I2C_SDA_PORT GPIOC
#define LCD_I2C_SDA_PIN GPIO_Pin_1

// PCF8574 I2C LCD Address (usually 0x27 or 0x3F)
#define LCD_I2C_ADDR 0x27

// PCF8574 pin mapping for LCD
#define LCD_RS 0x01         // P0
#define LCD_EN 0x04         // P2
#define LCD_D4 0x10         // P4
#define LCD_D5 0x20         // P5
#define LCD_D6 0x40         // P6
#define LCD_D7 0x80         // P7
#define LCD_BACKLIGHT 0x08  // P3

// LCD Commands
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_DISPLAY_OFF 0x08
#define LCD_CURSOR_ON 0x0E
#define LCD_CURSOR_OFF 0x0C
#define LCD_BLINK_ON 0x0F
#define LCD_BLINK_OFF 0x0E
#define LCD_FUNCTION_SET 0x28  // 4-bit mode, 2 lines, 5x8 dots

// Function prototypes
void LCD_I2C_Init (void);
void LCD_I2C_Command (uint8_t cmd);
void LCD_I2C_Data (uint8_t data);
void LCD_I2C_PrintChar (char c);
void LCD_I2C_PrintString (char *str);
void LCD_I2C_SetCursor (uint8_t row, uint8_t col);
void LCD_I2C_Clear (void);
void LCD_I2C_Home (void);
void LCD_I2C_Backlight (uint8_t state);

#endif /* __LCD1602_I2C_H */
