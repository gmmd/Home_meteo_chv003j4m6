#include "debug.h"
#include "ch32v00x.h"
#include "dht11.h"
#include "lcd1602_i2c.h"

int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (115200);

    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("DHT11 + I2C LCD1602 Test\r\n");

    // Initialize I2C LCD1602
    LCD_I2C_Init();
    LCD_I2C_Clear();
    LCD_I2C_Backlight (1);  // Turn on backlight

    // Display welcome message
    LCD_I2C_SetCursor (0, 0);
    LCD_I2C_PrintString ("DHT11 Sensor");
    LCD_I2C_SetCursor (1, 0);
    LCD_I2C_PrintString ("Initializing...");
    Delay_Ms (3000);

    // Initialize DHT11
    DHT11_Init();

    LCD_I2C_Clear();

    uint32_t error_count = 0;
    uint32_t success_count = 0;

    while (1) {
        DHT11_Data_t dht_data;
        DHT11_Result_t result;

        printf ("Attempting DHT11 read...\r\n");
        result = DHT11_Read (&dht_data);

        if (result == DHT11_OK) {
            char temp_str[17];
            char humi_str[17];

            success_count++;

            // Check for reasonable values
            if (dht_data.temperature_int > 60 || dht_data.humidity_int > 100) {
                printf ("DHT11: Unreasonable values detected\r\n");
                LCD_I2C_SetCursor (0, 0);
                LCD_I2C_PrintString ("Invalid Data    ");
                LCD_I2C_SetCursor (1, 0);
                LCD_I2C_PrintString ("Check Sensor    ");
            } else {
                // Format temperature string
                sprintf (temp_str, "Temp:     %2d.%d%cC",
                         dht_data.temperature_int,
                         dht_data.temperature_dec,
                         0xDF);  // Degree symbol

                // Format humidity string
                sprintf (humi_str, "Humi:      %2d.%d%%",
                         dht_data.humidity_int,
                         dht_data.humidity_dec);

                // Display on LCD
                LCD_I2C_SetCursor (0, 0);
                LCD_I2C_PrintString (temp_str);
                LCD_I2C_SetCursor (1, 0);
                LCD_I2C_PrintString (humi_str);

                // Print to serial
                printf ("SUCCESS #%d - Temperature: %d.%d°„C, Humidity: %d.%d%%\r\n",
                        success_count,
                        dht_data.temperature_int, dht_data.temperature_dec,
                        dht_data.humidity_int, dht_data.humidity_dec);
            }
        } else {
            error_count++;

            // Display error on LCD
            LCD_I2C_SetCursor (0, 0);
            switch (result) {
            case DHT11_ERROR_TIMEOUT:
                LCD_I2C_PrintString ("DHT11: Timeout  ");
                printf ("DHT11 Error: Timeout\r\n");
                break;
            case DHT11_ERROR_CHECKSUM:
                LCD_I2C_PrintString ("DHT11: Checksum ");
                printf ("DHT11 Error: Checksum\r\n");
                break;
            case DHT11_ERROR_START:
                LCD_I2C_PrintString ("DHT11: No Start ");
                printf ("DHT11 Error: Start signal\r\n");
                break;
            case DHT11_ERROR_DATA:
                LCD_I2C_PrintString ("DHT11: Data Err ");
                printf ("DHT11 Error: Data reading\r\n");
                break;
            default:
                LCD_I2C_PrintString ("DHT11: Unknown  ");
                printf ("DHT11 Error: Unknown\r\n");
                break;
            }

            LCD_I2C_SetCursor (1, 0);
            LCD_I2C_PrintString ("Check Connection");

            printf ("Error count: %d, Success count: %d\r\n", error_count, success_count);
        }

        printf ("Waiting 3 seconds...\r\n\r\n");
        Delay_Ms (3000);  // Wait 3 seconds between readings
    }
}
