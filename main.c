#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SERVO_PORT_DDR      DDRA
#define SERVO_PORT_OUTPUT   PORTA
#define SERVO_BIT           0
#define SENSE_PORT_DDR      DDRA
#define SENSE_PORT_OUTPUT   PORTA
#define SENSE_BIT           1
#define SENSE_BITMASK       0b00000010

volatile char pinChanged;
volatile char registerValue;

#define SERVO_HIGH() SERVO_PORT_OUTPUT |= _BV(SERVO_BIT)
#define SERVO_LOW() SERVO_PORT_OUTPUT &= ~_BV(SERVO_BIT)

void setup() {
    cli(); /* disable interrupts */

    SENSE_PORT_DDR &= ~_BV(SENSE_BIT); /* set sense pin as input */
    SENSE_PORT_OUTPUT |= _BV(SENSE_BIT); /* activate internal pull-up resistor for sense pin */
    PCMSK0 |= _BV(PCINT1); /* enable  PCINT1 pin change interrupt */
    GIMSK |= _BV(PCIE0); /* enable pin change interrupt 0 (PCINT[7:0]) */

    SERVO_PORT_DDR |= _BV(SERVO_BIT);   /* make the servo bit an output */

    wdt_enable(WDTO_1S); /* enable 1s watchdog timer */

    sei(); /* enable interrupts */
}

ISR(PCINT0_vect) {
    pinChanged = 1;
}

void turnServoMax() {
    for(int i=0; i<50; i++) {
        SERVO_HIGH();
        _delay_us(600);
        SERVO_LOW();
        _delay_us(19400);
    }
}

void turnServoMin() {
    for(int i=0; i<50; i++) {
        SERVO_HIGH();
        _delay_us(2400);
        SERVO_LOW();
        _delay_us(17600);
    }
}

int main() {
    setup();

    registerValue = PINA & SENSE_BITMASK;

    if (registerValue > 0) {
        turnServoMin();
    } else {
        turnServoMax();
    }
    pinChanged = 0;
    registerValue = 0;

    for (int i = 0; i < 500; i++) {
        wdt_reset();
        _delay_ms(5);
    }

    while (1) {
        if (pinChanged == 1) {
            cli();

            registerValue = PINA & SENSE_BITMASK;

            if (registerValue > 0) {
                turnServoMin();
            } else {
                turnServoMax();
            }

            pinChanged = 0;
            registerValue = 0;

            for (int i = 0; i < 500; i++) {
                wdt_reset();
                _delay_ms(5);
            }

            sei();
        }

        wdt_reset();
    }

    return 0;
}
