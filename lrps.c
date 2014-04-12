#include <avr/io.h>
#include <util/delay.h>

/* inputs */

#define TV_USB_PWR  _BV(0)
#define ROUTER_BTN  _BV(1)
#define CHARGER_BTN _BV(2)

#define BUTTONS (ROUTER_BTN | CHARGER_BTN)

/* outputs */

#define PWR_SUP_ON  _BV(3)
#define HDD_USB_PWR _BV(4)
#define ROUTER_PWR  _BV(5)
#define CHARGER_PWR _BV(6)

#define POWER_OUTPUTS (HDD_USB_PWR | ROUTER_PWR | CHARGER_PWR)

/* conditions */

#define TV_USB_POWERED (PINB & TV_USB_PWR)
#define PWR_ENABLED (PORTB & POWER_OUTPUTS)

/* inline functions */

#define set_bit_if(port, bit, cond) port = (port & ~(bit)) | ((cond) ? bit : 0)

#define toggle_with_button(ibit, obit) \
	if (PINB & (ibit)) { \
		if ((button_state & (ibit)) != (ibit)) { \
			_delay_ms(25); \
			if (PINB & (ibit)) { \
				button_state |= (ibit); \
				PORTB ^= (obit); \
			} \
		} \
	} else button_state &= ~(ibit)

int main(void) {
	uint8_t button_state = 0;

	DDRB = POWER_OUTPUTS; /* power supply should be floating */
	PORTB = BUTTONS; /* enable internal pullups for buttons only */

	while (1) {
		/* HDD is turned on <=> TV USB power is detected */
		set_bit_if(PORTB, HDD_USB_PWR, TV_USB_POWERED);
		/* router button is pressed => router state changes */
		toggle_with_button(ROUTER_BTN, ROUTER_PWR);
		/* charger button is pressed => charger state changes */
		toggle_with_button(CHARGER_BTN, CHARGER_PWR);
		/* Power supply is turned on <=> any of the above is enabled */
		set_bit_if(DDRB, PWR_SUP_ON, PWR_ENABLED);
	}
}
