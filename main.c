#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdint.h>
#include "serial.h"
#include "DHT.h"

int main(void)
{
	uart_init(); //required to wright out something

    uint8_t data [4];

    initDHT();

    while(1) {
        if(fetchData(data))
        {
            int temperature = data[0];
            int humidity = data[2];
            printf_P(PSTR("Temperature: "));
            printf("%lf\n", temperature);
            printf_P(PSTR("Humidity: "));
            printf("%lf\n", humidity);
        } else {
			printf("Error while fething data\n");
		}

        _delay_ms(1000);
    }

    return 0;
}
