CC = g++

SOURCES = $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
OBJS = ${SOURCES:.cpp=.o}

all: build run

build: ${OBJS}
	@${CC} -o ./dist/emulator ${OBJS}

%.o: %.cpp
	@${CC} -c $< -o $@

run: dist/emulator
	@./dist/emulator

clean:
	@find . -type f -name '*.o' -exec rm {} +