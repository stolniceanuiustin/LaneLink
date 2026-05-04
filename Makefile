CC = gcc
CFLAGS = -Wall -g
LIBS = -lm -lSDL2

TARGET = LaneLink

all: $(TARGET)

y.tab.c y.tab.h: parser.y
	yacc -d parser.y

lex.yy.c: lexer.l y.tab.h
	lex lexer.l


$(TARGET): lex.yy.c y.tab.c IntersectionGraph.c Visualizer.c main.c 
	$(CC) $(CFLAGS) lex.yy.c y.tab.c IntersectionGraph.c Visualizer.c main.c -o $(TARGET) $(LIBS) -ll -lfl

clean:
	rm -f $(TARGET) lex.yy.c y.tab.c y.tab.h