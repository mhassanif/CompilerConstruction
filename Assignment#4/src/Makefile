CC = gcc
CFLAGS = -Wall -g
LEX = flex
YACC = bison
YACCFLAGS = -d

all: json2relcsv

json2relcsv: lex.yy.c parser.tab.c ast.c schema.c main.c
	$(CC) $(CFLAGS) -o json2relcsv lex.yy.c parser.tab.c ast.c schema.c main.c

lex.yy.c: scanner.l parser.tab.h
	$(LEX) scanner.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) $(YACCFLAGS) parser.y

clean:
	rm -f json2relcsv lex.yy.c parser.tab.c parser.tab.h *.csv
