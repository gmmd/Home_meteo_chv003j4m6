#include "lcd1602_i2c.h"
#include "debug.h"

static uint8_t backlight_state = LCD_BACKLIGHT;

// I2C write byte function
static void I2C_WriteByte (uint8_t addr, uint8_t data) {
    // Wait until I2C is not busy
    while (I2C_GetFlagStatus (LCD_I2C, I2C_FLAG_BUSY));

    // Generate start condition
    I2C_GenerateSTART (LCD_I2C, ENABLE);
    while (!I2C_CheckEvent (LCD_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // Send address for write
    I2C_Send7bitAddress (LCD_I2C, addr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent (LCD_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send data
    I2C_SendData (LCD_I2C, data);
    while (!I2C_CheckEvent (LCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Generate stop condition
    I2C_GenerateSTOP (LCD_I2C, ENABLE);
}

// Send 4 bits to LCD via I2C
static void LCD_I2C_Send4Bit (uint8_t data) {
    uint8_t byte = data | backlight_state;

    // Send with EN high
    I2C_WriteByte (LCD_I2C_ADDR, byte | LCD_EN);
    Delay_Us (1);

    // Send with EN low
    I2C_WriteByte (LCD_I2C_ADDR, byte & ~LCD_EN);
    Delay_Us (50);
}

// Send 8 bits to LCD (two 4-bit operations)
static void LCD_I2C_Send8Bit (uint8_t data, uint8_t rs) {
    uint8_t upper = (data & 0xF0);
    uint8_t lower = ((data << 4) & 0xF0);

    if (rs) {
        upper |= LCD_RS;
        lower |= LCD_RS;
    }

    LCD_I2C_Send4Bit (upper);
    LCD_I2C_Send4Bit (lower);
}

// Initialize I2C peripheral
static void I2C_GPIO_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd (LCD_I2C_CLK, ENABLE);
    RCC_APB2PeriphClockCmd (LCD_I2C_GPIO_CLK, ENABLE);

    // Configure I2C pins
    GPIO_InitStructure.GPIO_Pin = LCD_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (LCD_I2C_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LCD_I2C_SDA_PIN;
    GPIO_Init (LCD_I2C_SDA_PORT, &GPIO_InitStructure);

    // Configure I2C
    I2C_InitStructure.I2C_ClockSpeed = 100000;  // 100kHz
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init (LCD_I2C, &I2C_InitStructure);
    I2C_Cmd (LCD_I2C, ENABLE);
}

// Initialize LCD via I2C
void LCD_I2C_Init (void) {
    I2C_GPIO_Init();

    // Wait for LCD to power up
    Delay_Ms (50);

    // Initialize LCD in 4-bit mode
    LCD_I2C_Send4Bit (0x30);
    Delay_Ms (5);

    LCD_I2C_Send4Bit (0x30);
    Delay_Ms (1);

    LCD_I2C_Send4Bit (0x30);
    Delay_Ms (1);

    LCD_I2C_Send4Bit (0x20);  // Set 4-bit mode
    Delay_Ms (1);

    // Function set: 4-bit mode, 2 lines, 5x8 dots
    LCD_I2C_Command (LCD_FUNCTION_SET);

    // Display off
    LCD_I2C_Command (LCD_DISPLAY_OFF);

    // Clear display
    LCD_I2C_Command (LCD_CLEAR);

    // Entry mode set
    LCD_I2C_Command (LCD_ENTRY_MODE);

    // Display on, cursor off, blink off
    LCD_I2C_Command (LCD_DISPLAY_ON);

    // Turn on backlight
    backlight_state = LCD_BACKLIGHT;
}

// Send command to LCD
void LCD_I2C_Command (uint8_t cmd) {
    LCD_I2C_Send8Bit (cmd, 0);  // RS = 0 for command

    if (cmd == LCD_CLEAR || cmd == LCD_HOME)
        Delay_Ms (2);
    else
        Delay_Ms (1);
}

// Send data to LCD
void LCD_I2C_Data (uint8_t data) {
    LCD_I2C_Send8Bit (data, 1);  // RS = 1 for data
    Delay_Ms (1);
}

// Print a character
void LCD_I2C_PrintChar (char c) {
    LCD_I2C_Data (c);
}

// Print a string
void LCD_I2C_PrintString (char *str) {
    while (*str) {
        LCD_I2C_PrintChar (*str++);
    }
}

// Set cursor position
void LCD_I2C_SetCursor (uint8_t row, uint8_t col) {
    uint8_t address;

    if (row == 0)
        address = 0x80 + col;  // First line
    else
        address = 0xC0 + col;  // Second line

    LCD_I2C_Command (address);
}

// Clear display
void LCD_I2C_Clear (void) {
    LCD_I2C_Command (LCD_CLEAR);
}

// Return cursor to home position
void LCD_I2C_Home (void) {
    LCD_I2C_Command (LCD_HOME);
}

// Control backlight
void LCD_I2C_Backlight (uint8_t state) {
    if (state)
        backlight_state = LCD_BACKLIGHT;
    else
        backlight_state = 0;

    // Send current backlight state
    I2C_WriteByte (LCD_I2C_ADDR, backlight_state);
}
