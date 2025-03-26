CC := g++

DEBUGFLAGS = -Wall -Wextra -pedantic -g -fno-omit-frame-pointer
CFLAGS := -O3 $(DEBUGFLAGS)

LDFLAGS := -lSDL2 -lm

SOURCES := $(shell find . -name "*.cpp")
OBJS = ${SOURCES:.cpp=.o}

OUTPUT := program
OUTPUT_DIR := ./dist

all: build run

build: ${OBJS}
	@mkdir -p ${OUTPUT_DIR}
	@${CC} $(CFLAGS) -o ${OUTPUT_DIR}/${OUTPUT} ${OBJS} ${LDFLAGS}

%.o: %.cpp
	@${CC} $(CFLAGS) -c $< -o $@

run: ${OUTPUT_DIR}/${OUTPUT}
	@${OUTPUT_DIR}/${OUTPUT}

profile: build ${OUTPUT_DIR}/${OUTPUT}
	@mkdir -p ${OUTPUT_DIR}/flamegraph.out
	@mkdir -p ${OUTPUT_DIR}/perf.out

	@sudo perf record --quiet -g -- ${OUTPUT_DIR}/${OUTPUT}
	@mv perf.data ${OUTPUT_DIR}/perf.out/perf.data

	@sudo perf script -i ${OUTPUT_DIR}/perf.out/perf.data | ./libs/FlameGraph/stackcollapse-perf.pl > dist/flamegraph.out/out.folded
	@./libs/FlameGraph/flamegraph.pl ${OUTPUT_DIR}/flamegraph.out/out.folded > ${OUTPUT_DIR}/flamegraph.svg

clean:
	@find . -type f -name '*.o' -exec rm {} +
	@rm -rf ./${OUTPUT_DIR}
