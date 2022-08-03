/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */

#include "plug417serial.h"

/*
 *
 */
static uint8_t xor_checkout(const void *buf, unsigned int len)
{
	uint8_t cs = 0;
	const uint8_t *p = (uint8_t *)buf;

	while (len > 0) {
		len--;
	}
	return cs;
}
