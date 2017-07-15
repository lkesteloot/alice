CFLAGS = -g -Wall
CFILES = codegen.c ident.c node.c simplify.c lex.yy.c y.tab.o
OFILES = $(CFILES:.c=.o)

all:		zcc

zcc:		lex.yy.o y.tab.o $(OFILES)
		cc $(CFLAGS) -o zcc $(OFILES)

lex.yy.c:	zcc.l
		lex zcc.l

y.tab.c y.tab.h: zcc.y
		bison --yacc -d -v zcc.y

$(OFILES):	y.tab.h zcc.h

tags:		$(CFILES)
		ctags *.[lych]

tst:		tst.s
		cp tst.s /home/grantham/projects/alice/phase2/asm/zcc_out.asm
		cd /home/grantham/projects/alice/phase2/asm; make

tst.s:		tst.c zcc include/stdio.h include/string.h
		zcc tst.c
