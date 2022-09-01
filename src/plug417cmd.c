/*
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plug417serial.h"
#include "plug417cmd.h"
/*
 * a:b:c ...
 *
 * Example:
 *  n=1:w=10:h=22
 */
static const char *parse_elm(const char *buf, char *elm, char *val, int size)
{
	const char *p = buf;

	*val = '\0';
	while (*p != ':' && *p != '=' && *p != '\0' && size != 0) {
		*elm++ = *p++;
		size--;
	}
	*elm = '\0';

	if (*p == ':') {
		p++;
		return p;
	}
	if (*p == '=')
		p++;

	while (*p != ':' && *p != '=' && *p != '\0' && size != 0) {
		*val++ = *p++;
		size--;
	}
	*val = '\0';

	if (*p == ':' || *p == '=')
		p++;
	
	return p;
}

/*
 *
 */
struct plug417_sub_cmd {
	const char *cmd;
	const char *alias;
	int (*handler)(struct plug417_serial *, unsigned int);
	int (*handler_ext)(struct plug417_serial *, unsigned int, unsigned int);
	int set;
};

static const struct plug417_sub_cmd plug417_cmd_digital_video[] = {
	{"extsync", "s", plug417_set_external_synchronization, NULL, -1},
	{"port", "p", plug417_set_digital_port_parallel_type, NULL, -1},
	{"cmos", "c", plug417_set_cmos_content, NULL, -1},
	{"interface", "i", plug417_set_cmos_interface, NULL, -1},
	{"rate", "r", plug417_set_digital_frame_rate, NULL, -1},
	{"mipi", "m", plug417_set_mipi_on, NULL, -1},
	{"scene", "x", plug417_set_scene_compentation, NULL, -1},
	{"shutter", "z", plug417_set_shutter_compentation, NULL, -1},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_small_icon[] = {
	{"num", "n", NULL, NULL, -1},
	{"on", "", NULL, plug417_set_small_icon_on, 1},
	{"off", "", NULL, plug417_set_small_icon_on, 0},
	{"x", "", NULL, plug417_set_small_icon_x, -1},
	{"y", "", NULL, plug417_set_small_icon_y, -1},
	{"width", "w", NULL, plug417_set_small_icon_width, -1},
	{"transparency", "t", plug417_set_small_icon_transparency, NULL, -1},
	{NULL},
};


static const struct plug417_sub_cmd plug417_cmd_menu_bar[] = {
	{"on", "", plug417_set_menu_bar_on, NULL, 1},
	{"off", "", plug417_set_menu_bar_on, NULL, 0},
	{"location", "y", plug417_set_menu_bar_location, NULL, -1},
	{"transparency", "t", plug417_set_menu_bar_transparency, NULL, -1},
	{NULL, NULL, NULL, NULL},
};

static const struct plug417_sub_cmd plug417_cmd_layer[] = {
	{"on", "", plug417_set_layer_on, NULL, 1},
	{"off", "", plug417_set_layer_on, NULL, 0},
	{"transparency", "t", plug417_set_layer_transparency, NULL, -1},
	{NULL, NULL, NULL, NULL},
};

static const struct plug417_sub_cmd plug417_cmd_half_pixel_cursor[] = {
	{"on", "", plug417_set_half_pixel_cursor_on, NULL, 1},
	{"off", "", plug417_set_half_pixel_cursor_on, NULL, 0},
	{"x", "", plug417_set_half_pixel_x, NULL, -1},
	{"y", "", plug417_set_half_pixel_y, NULL, -1},
	{"color", "c", plug417_set_half_pixel_color, NULL, -1},
	{NULL, NULL, NULL, NULL},
};

static const struct plug417_sub_cmd plug417_cmd_hotspot_tracking[] = {
	{"on", "", plug417_set_hotspot_tracking_cursor_on, NULL, 1},
	{"off", "", plug417_set_hotspot_tracking_cursor_on, NULL, 0},
	{"upper", "u", plug417_set_hotspot_tracking_upper_limit, NULL, -1},
	{"lower", "l", plug417_set_hotspot_tracking_lower_limit, NULL, -1},
	{"r", "", plug417_set_hotspot_tracking_color_r, NULL, -1},
	{"g", "", plug417_set_hotspot_tracking_color_g, NULL, -1},
	{"b", "", plug417_set_hotspot_tracking_color_b, NULL, -1},
	{NULL, NULL, NULL, NULL},
};

static const struct plug417_sub_cmd plug417_cmd_area_analisys[] = {
	{"mode", "m", plug417_set_area_analisys_mode, NULL, -1},
	{"x", "", plug417_set_area_x, NULL, -1},
	{"y", "", plug417_set_area_y, NULL, -1},
	{"width", "w", plug417_set_area_width, NULL, -1},
	{"height", "h", plug417_set_area_height, NULL, -1},
	{"r", "", plug417_set_area_color_r, NULL, -1},
	{"g", "", plug417_set_area_color_g, NULL, -1},
	{"b", "", plug417_set_area_color_b, NULL, -1},
	{"alarm", "a", plug417_set_area_high_temperature_alarm, NULL, -1},
	{"threshold", "t", plug417_set_area_high_temperature_alarm_threshold, NULL, -1},
	{NULL},
};

struct plug417_cmd {
	const char *name;
	const char *cmd;
	const struct plug417_sub_cmd *sub;
};

static const struct plug417_cmd plug417_cmd[] = {
	{"Digital video", "digit", plug417_cmd_digital_video},
	{"Small icon", "icon", plug417_cmd_small_icon},
	{"Menu bar", "menu", plug417_cmd_menu_bar},
	{"Layer", "layer", plug417_cmd_layer},
	{"Half pixel", "hpcursor", plug417_cmd_half_pixel_cursor},
	{"Area Analysis", "area", plug417_cmd_area_analisys},
	{"Hotspot tracking", "hptrack", plug417_cmd_hotspot_tracking},
	{NULL},
};


/*
 *
 */
static int plug417_cmd_handler(struct plug417_serial *s, const char *name,
		const struct plug417_sub_cmd *cmd, const char *c)
{
	char parm[64];
	char val[64];
	int err = 0;
	int set[16];
	int i;

	for (i = 0; i < sizeof(set) / sizeof(set[0]); i++)
		set[i] = -1;

	while (*c != '\0') {
		c = parse_elm(c, parm, val, sizeof(parm));
		debug(PLUG417_CMD_PARSE_DEBUG, "P = %s, V = %s\n", parm, val);

		i = 0;
		while (cmd[i].cmd) {
			if (!strcmp(parm, cmd[i].cmd) || !strcmp(parm, cmd[i].alias)) {
				if (cmd[i].set < 0)
					set[i] = strtol(val, NULL, 0);
				else
					set[i] = cmd[i].set;
			}
			i++;
		}
	}
	debug(PLUG417_CMD_PARSE_DEBUG, "%s\n", name);

	i = 0;
	while (cmd[i].cmd) {
		if (set[i] >= 0) {
			if (cmd[i].handler) {
				if ((err = cmd[i].handler(s, set[i])) < 0)
					return err;
			} else if (cmd[i].handler_ext) {
				if ((err = cmd[i].handler_ext(s, set[0], set[i])) < 0)
					return err;
			}
		}
		i++;
	}
	return err;
}

/*
 *
 */
static void plug417_set_help(struct plug417_serial *s)
{
	const struct plug417_cmd *c = plug417_cmd;
	const struct plug417_sub_cmd *sub;

	while (c->name) {
		printf("%s: %s\n", c->name, c->cmd);
		sub = c->sub;
		printf("Sub commands:\n");
		while (sub->cmd) {
			printf("\t%s, %s\n", sub->cmd, sub->alias);
			sub++;
		}
		c++;
	}
}

/*
 *
 */
int plug417_set_command(struct plug417_serial *s, const char *cmd)
{
	char parm[64];
	char val[64];
	int err;
	const struct plug417_cmd *c = plug417_cmd;

	cmd = parse_elm(cmd, parm, val, sizeof(parm));
	debug(PLUG417_CMD_PARSE_DEBUG, "P = %s, V = %s\n", parm, val);

	if (!strcmp(parm, "help")) {
		plug417_set_help(s);
		return 0;
	}

	while (c->name) {
		if (!strcmp(parm, c->cmd))
			err = plug417_cmd_handler(s, c->name, c->sub, cmd);

		c++;
	}

	return err;
}


