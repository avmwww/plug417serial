/*
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <endian.h>

#include "plug417serial.h"
#include "plug417cmd.h"

#define DEFAULT_DEVICE_NAME		"/dev/ttyACM0"

struct plug417 {
	int color;
	int mirror;
	int query;
	int page;
	int test_screen;
	int cmos_content;
	int cmos_interace;
	int brightness;
	const char *device;
	const char *command;
};

static void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ", %s\n", strerror(errno));
	va_end(ap);
	exit(EXIT_FAILURE);
}

static void usage(char **argv)
{
	printf("Usage: %s <options>\n", argv[0]);
	printf("\t-d --device <path>\tSerial device name, default %s\n", DEFAULT_DEVICE_NAME);
	printf("\t-g --get <0..%d\tQuery page, default action if parameters not specified print sensor status\n",
			PLUG417_PAGE_MAX);
	printf("\t-s --set <0..5>\tSet functional classification\n");
	printf("\t-p --page <0..5>\tPage\n");
	printf("\t-e --cmos_i <0..%d>\tSet CMOS interface type\n", PLUG417_COMMAND_CMOS_INTERFACE_MAX);
	printf("\t-f --cmos_c <0..%d>\tSet CMOS content\n", PLUG417_COMMAND_CMOS_CONTENT_MAX);
	printf("\t-c --color <0..%d>\tSet pseudo color\n", PLUG417_COMMAND_COLOR_MAX);
	printf("\t-m --mirror <0..%d>\tSet/Reset image mirroring\n", PLUG417_COMMAND_MIRROR_MAX);
	printf("\t-t --test <0..%d>\tSet test screen\n", PLUG417_COMMAND_TEST_SCREEN_MAX);
	printf("\t-r --command <command>\tType help for extended help usage for this switch\n");
	printf("\t-v --verbose <0..99>\tPrint verbose debug information\n");
	printf("\t-h --help\tUsage help\n");
	exit(EXIT_SUCCESS);
}


/*
 *
 */
static struct option plug417_options[] = {
	{"brightness", required_argument, 0,  'b' },
	{"color",      required_argument, 0,  'c' },
	{"device",     required_argument, 0,  'd' },
	{"cmos_i",     required_argument, 0,  'e' },
	{"cmos_c",     required_argument, 0,  'f' },
	{"get",        required_argument, 0,  'g' },
	{"mirror",     required_argument, 0,  'm' },
	{"page",       required_argument, 0,  'p' },
	{"command",    required_argument, 0,  'r' },
	{"set",        required_argument, 0,  's' },
	{"test",       required_argument, 0,  't' },
	{"verbose",    required_argument, 0,  'v' },
	{"help",       no_argument,       0,  'h' },
	{0,            0,                 0,   0  }
};

/*
 *
 */
static int parse_opt(int argc, char **argv, struct plug417 *plug)
{
	int c;
	int optindex = 0;

	while ((c = getopt_long(argc, argv, "b:c:d:e:f:g:m:p:r:t:v:h", plug417_options, &optindex)) != -1) {
		switch (c) {
			case 'v':
				plug417serial_debug_level_set(strtol(optarg, NULL, 0));
				break;
			case 'b':
				plug->brightness = strtol(optarg, NULL, 0);
				break;
			case 'p':
				plug->page = strtol(optarg, NULL, 0);
				break;
			case 'g':
				plug->query = strtol(optarg, NULL, 0);
				break;
			case 'd':
				plug->device = optarg;
				break;
			case 'c':
				plug->color = strtol(optarg, NULL, 0);
				break;
			case 'e':
				plug->cmos_interace = strtol(optarg, NULL, 0);
				break;
			case 'f':
				plug->cmos_content = strtol(optarg, NULL, 0);
				break;
			case 'm':
				plug->mirror = strtol(optarg, NULL, 0);
				break;
			case 't':
				plug->test_screen = strtol(optarg, NULL, 0);
				break;
			case 'r':
				plug->command = optarg;
				break;
			case 'h':
				usage(argv);
				break;
			case 0:
			default:
				usage(argv);
				break;
		}
	}
#if 0
	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
		return -1;
	}
#endif

	return 0;
}

/*
 *
 */
int main(int argc, char **argv)
{
	struct plug417_serial *ps;
	struct plug417_status st;
	struct plug417 *plug;

	plug = malloc(sizeof(struct plug417));
	if (!plug)
		fatal("Memory low\n");

	memset(plug, 0, sizeof(struct plug417));

	/* Default settings */
	plug->device = DEFAULT_DEVICE_NAME;
	plug->color = -1;
	plug->mirror = -1;
	plug->test_screen = -1;
	plug->query = -1;
	plug->cmos_content = -1;
	plug->cmos_interace = -1;
	plug->brightness = -1;

	if (argc  < 2)
		plug->query = 0;

	parse_opt(argc, argv, plug);

	ps = plug417_open(plug->device);

	if (!ps)
		fatal("Cannot open port %s", plug->device);

	/* Timeout 1 sec */
	ps->timeout = 1000000;

	if (plug->query >= 0) {
		if (plug->query == 0) {
			if (plug417_query_status(ps, &st) == 0)
				plug417_print_status(ps, &st);
		} else {
			plug417_query(ps, plug->query, plug->page);
			plug417_query_reply_print(ps);
		}
	}

	if (plug->color >= 0)
		plug417_set_pseaudo_color(ps, plug->color);

	if (plug->mirror >= 0)
		plug417_set_mirror_image(ps, plug->mirror);

	if (plug->test_screen >= 0)
		plug417_set_test_screen(ps, plug->test_screen);

	if (plug->cmos_interace >= 0)
		plug417_set_cmos_interface(ps, plug->cmos_interace);

	if (plug->cmos_content >= 0)
		plug417_set_cmos_content(ps, plug->cmos_content);

	if (plug->brightness >= 0)
		plug417_set_brightness(ps, plug->brightness);

	if (plug->command)
		plug417_set_command(ps, plug->command);

	plug417_close(ps);

	free(plug);

	exit(EXIT_SUCCESS);
}

