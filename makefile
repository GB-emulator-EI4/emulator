CC = g++

CFLAGS = -Wall -Wextra -pedantic


CFLAGS = -Wall -Wextra -pedantic $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)


SOURCES := $(shell find . -name "*.cpp")
OBJS = ${SOURCES:.cpp=.o}

OUTPUT = program
OUTPUT_DIR = dist

all: build run

build: ${OBJS}
	@${CC} $(CFLAGS) -o ./${OUTPUT_DIR}/${OUTPUT} ${OBJS} ${LDFLAGS}

%.o: %.cpp
	@${CC} $(CFLAGS) -c $< -o $@

run: ${OUTPUT_DIR}/${OUTPUT}
	@./${OUTPUT_DIR}/${OUTPUT}

clean:
	@find . -type f -name '*.o' -exec rm {} +
	@rm -f ./${OUTPUT_DIR}/${OUTPUT}