#
# Makefile for plug417lib project
#

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

TARGET = plug417serial.a

SRCS = plug417serial.c

OBJS = $(SRCS:.c=.o)

CFLAGS +=  -Wall -D_GNU_SOURCE -Iinclude
#CFLAGS += -fPIC -Wall -Wextra -O2 -g
LDFLAGS += -lm

INDENT_FLAGS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
		-cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
		-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

all: $(TARGET)

plug417serial.so: $(OBJS)
	$(CC) ${LDFLAGS} -shared -o $@ $^

$(TARGET): $(OBJS)
	$(AR) -rcs $@ $^

clean:
	rm -f $(TARGET) $(OBJS)

#.PHONY: %.i

%.i : %
	indent $^ -o $@ $(INDENT_FLAGS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

vpath %.h include
vpath %.c src

