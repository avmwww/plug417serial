/*
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <endian.h>

#include "plug417serial.h"

static void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ", %s\n", strerror(errno));
	va_end(ap);
	exit(EXIT_FAILURE);
}

/*
 *
 */
int main(int argc, char **argv)
{
	struct plug417_serial *ps;
	struct plug417_status st;
	char *serial = "/dev/ttyUSB0";

	if (argc > 1)
		serial = argv[1];

	ps = plug417_open(serial);

	if (!ps)
		fatal("Cannot open port %s", serial);

	/* Timeout 1 sec */
	ps->timeout = 1000000;

	if (plug417_get_status(ps, &st) == 0)
		plug417_print_status(ps, &st);

	plug417_close(ps);

	exit(EXIT_SUCCESS);
}

