CC = gcc
CFLAGS = -Wall -g -Iinclude -Ibuild
LIBS = -lm -ll -lfl

TARGET = LaneLink

SRCS = src/main.c src/IntersectionGraph.c
OBJS = $(SRCS:src/%.c=build/%.o) build/lex.yy.o build/y.tab.o

all: build $(TARGET)

build:
	mkdir -p build

build/y.tab.c build/y.tab.h: src/parser.y | build
	yacc -d src/parser.y -b build/y

build/lex.yy.c: src/lexer.l build/y.tab.h | build
	lex -o build/lex.yy.c src/lexer.l

build/y.tab.o: build/y.tab.c | build
	gcc $(CFLAGS) -c $< -o $@

build/lex.yy.o: build/lex.yy.c | build
	gcc $(CFLAGS) -c $< -o $@

build/%.o: src/%.c | build
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

clean:
	rm -rf build $(TARGET)
