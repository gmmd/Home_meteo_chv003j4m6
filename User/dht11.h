#ifndef __DHT11_H
#define __DHT11_H

#include "ch32v00x.h"

// DHT11 connected to PD6 (change as needed - make sure this pin exists on CH32V003)
// For CH32V003J4M6, better to use PC4 or PA2
#define DHT11_GPIO_PORT GPIOC
#define DHT11_GPIO_PIN GPIO_Pin_4
#define DHT11_GPIO_CLK RCC_APB2Periph_GPIOC

// DHT11 return codes
typedef enum {
    DHT11_OK = 0,
    DHT11_ERROR_TIMEOUT,
    DHT11_ERROR_CHECKSUM,
    DHT11_ERROR_START,
    DHT11_ERROR_DATA
} DHT11_Result_t;

// DHT11 data structure
typedef struct
{
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
    uint8_t checksum;
} DHT11_Data_t;

// Function prototypes
void DHT11_Init (void);
DHT11_Result_t DHT11_Read (DHT11_Data_t *data);

#endif /* __DHT11_H */
