OBJECTS = main.o 

TOPDIR:=$(shell pwd)

CFLAGS=-Wall -O2

TARGET=Intel
CLIBS=-L/usr/lib -lSDL -lSDL_image -lSDL_ttf  
CFLAGS+=-I$(TOPDIR)/headers -I/usr/include/SDL -I/usr/include/libxml2 -I/usr/lib/i386-linux-gnu/ -DDEBUG=0 -D__STDC_CONSTANT_MACROS
#LIB_NAME=GraphAPI.lib

CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CC)
OBJCOPY=$(CROSS_COMPILE)objcopy
REMOVE=rm

APPLICATION_NAME=robots
TARGET=linux

all:$(OBJECTS)
	$(CC) $(INCL) $(CFLAGS) $(OBJECTS) -o $(APPLICATION_NAME) $(LIB_NAME) $(CLIBS)
	@echo Compiled $(APPLICATION_NAME) for $(TARGET)

.PHONY : all

clean:
	rm -f *.o $(APPLICATION_NAME) 

.PHONY : clean
