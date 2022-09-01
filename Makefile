#
# Makefile for plug417serial project
#

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

TARGETS = plug417serial.a plug417serial.so plug417ctrl

SRCS = plug417serial.c plug417print.c plug417cmd.c

OBJS = $(SRCS:.c=.o)

CFLAGS +=  -Wall -D_GNU_SOURCE -Iinclude
CFLAGS += -DDEBUG
#CFLAGS += -fPIC -Wall -Wextra -O2 -g
LDFLAGS += -lm

INDENT_FLAGS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
		-cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
		-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

all: $(TARGETS)

plug417serial.so: $(OBJS)
	$(CC) ${LDFLAGS} -shared -o $@ $^

plug417serial.a: $(OBJS)
	$(AR) -rcs $@ $^

plug417ctrl: $(OBJS) plug417ctrl.o
	$(CC) ${LDFLAGS} -o $@ $^

clean:
	rm -f $(TARGETS) $(OBJS)

#.PHONY: %.i

%.i : %
	indent $^ -o $@ $(INDENT_FLAGS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

vpath %.h include
vpath %.c src

