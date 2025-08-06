#include "dht11.h"
#include "debug.h"

// Improved microsecond delay function
static void DHT11_Delay_us (uint32_t us) {
    uint32_t i;
    // More accurate delay calculation for 48MHz system clock
    // Each loop iteration takes approximately 4 cycles
    // 48MHz / 4 = 12M loops per second = 12 loops per microsecond
    for (i = 0; i < us * 12; i++) {
        __NOP();
    }
}

// Set DHT11 pin as output
static void DHT11_Pin_Output (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (DHT11_GPIO_PORT, &GPIO_InitStructure);
}

// Set DHT11 pin as input with pull-up
static void DHT11_Pin_Input (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Input with pull-up
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (DHT11_GPIO_PORT, &GPIO_InitStructure);
}

// Set DHT11 pin high
static void DHT11_Pin_High (void) {
    GPIO_SetBits (DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

// Set DHT11 pin low
static void DHT11_Pin_Low (void) {
    GPIO_ResetBits (DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

// Read DHT11 pin state
static uint8_t DHT11_Pin_Read (void) {
    return GPIO_ReadInputDataBit (DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

// Initialize DHT11
void DHT11_Init (void) {
    // Enable GPIO clock
    RCC_APB2PeriphClockCmd (DHT11_GPIO_CLK, ENABLE);

    // Configure pin as output and set high
    DHT11_Pin_Output();
    DHT11_Pin_High();

    // Wait for sensor to stabilize (DHT11 needs at least 1 second)
    Delay_Ms (2000);

    printf ("DHT11 initialized on PC4\r\n");
}

// Read data from DHT11 with improved timing and error checking
DHT11_Result_t DHT11_Read (DHT11_Data_t *data) {
    uint8_t buffer[5] = {0};
    uint8_t byte_idx, bit_idx;
    uint32_t timeout;
    uint8_t checksum;

    // Clear data structure
    data->humidity_int = 0;
    data->humidity_dec = 0;
    data->temperature_int = 0;
    data->temperature_dec = 0;
    data->checksum = 0;

    // Step 1: Send start signal
    DHT11_Pin_Output();
    DHT11_Pin_Low();
    Delay_Ms (20);  // Hold low for at least 18ms

    DHT11_Pin_High();
    DHT11_Delay_us (40);  // Hold high for 20-40us

    // Step 2: Switch to input and wait for DHT11 response
    DHT11_Pin_Input();

    // Wait for DHT11 to pull line low (start of response)
    timeout = 0;
    while (DHT11_Pin_Read() == 1) {
        DHT11_Delay_us (1);
        timeout++;
        if (timeout > 100) {
            DHT11_Pin_Output();
            DHT11_Pin_High();
            printf ("DHT11: No response\r\n");
            return DHT11_ERROR_START;
        }
    }

    // Wait for DHT11 to pull line high (80us low response)
    timeout = 0;
    while (DHT11_Pin_Read() == 0) {
        DHT11_Delay_us (1);
        timeout++;
        if (timeout > 100) {
            DHT11_Pin_Output();
            DHT11_Pin_High();
            printf ("DHT11: Response timeout 1\r\n");
            return DHT11_ERROR_START;
        }
    }

    // Wait for DHT11 to pull line low again (80us high response)
    timeout = 0;
    while (DHT11_Pin_Read() == 1) {
        DHT11_Delay_us (1);
        timeout++;
        if (timeout > 100) {
            DHT11_Pin_Output();
            DHT11_Pin_High();
            printf ("DHT11: Response timeout 2\r\n");
            return DHT11_ERROR_START;
        }
    }

    // Step 3: Read 40 bits of data (5 bytes)
    for (byte_idx = 0; byte_idx < 5; byte_idx++) {
        for (bit_idx = 0; bit_idx < 8; bit_idx++) {
            // Wait for bit start (line goes high)
            timeout = 0;
            while (DHT11_Pin_Read() == 0) {
                DHT11_Delay_us (1);
                timeout++;
                if (timeout > 100) {
                    DHT11_Pin_Output();
                    DHT11_Pin_High();
                    printf ("DHT11: Bit start timeout\r\n");
                    return DHT11_ERROR_DATA;
                }
            }

            // Measure high time to determine bit value
            // Wait 30us then check if still high
            DHT11_Delay_us (30);

            buffer[byte_idx] <<= 1;
            if (DHT11_Pin_Read() == 1) {
                buffer[byte_idx] |= 1;  // Bit is 1

                // Wait for line to go low
                timeout = 0;
                while (DHT11_Pin_Read() == 1) {
                    DHT11_Delay_us (1);
                    timeout++;
                    if (timeout > 100) {
                        DHT11_Pin_Output();
                        DHT11_Pin_High();
                        printf ("DHT11: Bit end timeout\r\n");
                        return DHT11_ERROR_DATA;
                    }
                }
            }
            // If line is low after 30us, bit is 0 (already set by left shift)
        }
    }

    // Step 4: Restore pin state
    DHT11_Pin_Output();
    DHT11_Pin_High();

    // Step 5: Verify checksum
    checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
    if (checksum != buffer[4]) {
        printf ("DHT11: Checksum error. Calculated: %d, Received: %d\r\n", checksum, buffer[4]);
        printf ("Data: %d %d %d %d %d\r\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
        return DHT11_ERROR_CHECKSUM;
    }

    // Step 6: Fill data structure
    data->humidity_int = buffer[0];
    data->humidity_dec = buffer[1];
    data->temperature_int = buffer[2];
    data->temperature_dec = buffer[3];
    data->checksum = buffer[4];

    printf ("DHT11: Read success - T:%d.%d H:%d.%d\r\n",
            data->temperature_int, data->temperature_dec,
            data->humidity_int, data->humidity_dec);

    return DHT11_OK;
}
