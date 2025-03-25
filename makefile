CC = g++

CFLAGS = -Wall -Wextra -pedantic -g -fno-omit-frame-pointer -O2
LDFLAGS = -lSDL2 -lm

SOURCES := $(shell find . -name "*.cpp")
OBJS = ${SOURCES:.cpp=.o}

OUTPUT = program
OUTPUT_DIR = dist

all: build run

build: ${OBJS}
	@mkdir -p ${OUTPUT_DIR}
	@${CC} $(CFLAGS) -o ./${OUTPUT_DIR}/${OUTPUT} ${OBJS} ${LDFLAGS}

%.o: %.cpp
	@${CC} $(CFLAGS) -c $< -o $@

run: ${OUTPUT_DIR}/${OUTPUT}
	@./${OUTPUT_DIR}/${OUTPUT}

profile: ${OUTPUT_DIR}/${OUTPUT}
	@echo "Running program with perf..."
	@sudo perf record -F 99 -g -- ./${OUTPUT_DIR}/${OUTPUT}
	@echo "Generating flame graph..."
	@sudo perf script | FlameGraph/stackcollapse-perf.pl > out.folded
	@FlameGraph/flamegraph.pl out.folded > flamegraph.svg
	@echo "Flame graph generated: flamegraph.svg"

clean:
	@find . -type f -name '*.o' -exec rm {} +
	@rm -f ./${OUTPUT_DIR}/${OUTPUT} out.folded perf.data perf.data.old flamegraph.svg
