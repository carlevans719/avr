#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_PORT_DDR        DDRA
#define LED_PORT_OUTPUT     PORTA
#define LED_BIT             0

void setup() {
    cli(); /* disable interrupts */

    LED_PORT_DDR |= _BV(LED_BIT);   /* make the LED bit an output */
    LED_PORT_OUTPUT |= _BV(LED_BIT); /* turn LED on */

    wdt_enable(WDTO_1S); /* enable 1s watchdog timer */

    char   i;
    i = 250;
    while(--i) {
        wdt_reset();
        _delay_ms(4);
    }

    LED_PORT_OUTPUT &= ~_BV(LED_BIT); /* turn the LED off */

    sei(); /* enable interrupts */
}

int main() {
    setup();

    while (1) {
        wdt_reset();
    }

    return 0;
}
