#include "DHT.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

void initDHT(void)
{
    /* Set LED as output */
    SET_BIT(DDRB,LED);

    /* According to the DHT11's datasheet, the sensor needs about
       1-2 seconds to get ready when first getting power, so we
       wait
     */
    _delay_ms(2000);
}


uint8_t fetchData(uint8_t* arr)
{
    uint8_t data [5];
    uint8_t cnt, check;
    int8_t i,j;

    /******************* Sensor communication start *******************/

    SET_BIT(DDRB,DHT_PIN);
    TCNT0 = 0;
    TCCR0B = 0x05;
    _delay_ms(2);
    CLEAR_BIT(DHT_PORT_OUT,DHT_PIN);
    _delay_ms(20);

    TCCR0B = 0x02;
    TCNT0 = 0;
    SET_BIT(DHT_PORT_OUT,DHT_PIN);
    CLEAR_BIT(DDRB,DHT_PIN);

    //CLEAR_BIT(DHT_PORT_OUT,DHT_PIN);

    //_delay_ms(20);

    //CLEAR_BIT(DDRB,DHT_PIN);

    //_delay_us(55);

    //TCCR0B = 0x05; //0x02
    //TCNT0 = 0;


    /* Wait for response from sensor, 20-40µs according to datasheet */
    while(IS_SET(DHT_PORT_IN,DHT_PIN))
    {
        if (TCNT0 >= 200) {
            printf("Timed out at first wait\n");

            return 0;
        }
    }

    /************************* Sensor preamble *************************/

    /* Now wait for the first response to finish, low ~80µs */
    TCNT0 = 0;
    while(!IS_SET(DHT_PORT_IN,DHT_PIN))
    {
        if (TCNT0 >= 200) {
            printf("Timed out at second wait\n");

            return 0;
        }
    }

    /********************* Data transmission start **********************/

    for (i = 0; i < 5; ++i)
    {
        for(j = 7; j >= 0; --j)
        {
            /* First there is always a 50µs low period */
            TCNT0 = 0;
            while(!IS_SET(DHT_PORT_IN,DHT_PIN))
            {
                if (TCNT0 >= 200) {
                    printf("Timed at first part in loop\n");

                    return 0;
                }
            }

            /* Then the data signal is sent. 26 to 28µs (ideally)
             indicate a low bit, and around 70µs a high bit */
            TCNT0 = 0;
            while(IS_SET(DHT_PORT_IN,DHT_PIN))
            {
                if (TCNT0 >= 280) {
                    printf("Timed at second part in loop\n");

                    return 0;
                }
            }

            /* Store the value now so that the whole checking doesn't
             move the TCNT0 forward by too much to make the data look
             bad */
            cnt = TCNT0;
            printf("%d", TCNT0);

            if (cnt >= 45 && cnt <= 70)
            {
                CLEAR_BIT(data[i],j);
                printf("Funky clear\n");
            }
            else if (cnt >= 130 && cnt <= 200)
            {
                SET_BIT(data[i],j);
                printf("%d\n", data[i]);
                printf("%d\n", j);
                printf("Funky set\n");
                return data[i], j;
            }
            else
            {
                printf("Funky count\n");


                return 0;
            }
        }
    }

    /********************* Sensor communication end *********************/

    printf("Checking data\n");
    check = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    //check = (data[i]) & 0xFF;

    printf("%d\n", check);
    if (check != data[4]) {
        printf("Funky checksum\n");
        return 0;
    }

    for(i = 0; i < 4; ++i)
    { arr[i] = data[i]; }

    printf("Successful!\n");

    return 1;
}
