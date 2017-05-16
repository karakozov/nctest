 
PHONY = clean all
TARGET_NAME = nctest

all: $(TARGET_NAME)

ROOT_DIR = $(shell pwd)

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc

INCDIR := .
INCLUDE := $(addprefix -I, $(INCDIR))
CFLAGS := -D__linux__ -D__USE_NCURSES__ -g -Wall $(INCLUDE)
SRCFILE := $(wildcard *.cpp) 
OBJFILE := $(patsubst %.cpp,%.o, $(SRCFILE))

LDFLAGS += -lstdc++ -lncurses -lpthread

$(TARGET_NAME): $(OBJFILE)
	$(LD) $(notdir $^) -o $(TARGET_NAME) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -MD $<
	
include $(wildcard *.d)

clean:
	rm -f *.o *.d *~ core $(TARGET_NAME)
distclean:
	rm -f *.o *.d *~ core $(TARGET_NAME)

src:
	@echo $(SRCFILE)
	@echo $(OBJFILE)
