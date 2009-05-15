#	$OpenBSD: Makefile,v 1.4 2004/02/01 15:19:57 jmc Exp $
#	@(#)Makefile	8.1 (Berkeley) 6/8/93

DIR=	usd/07.mail
SRCS=	mail0.nr mail1.nr mail2.nr mail3.nr mail4.nr mail5.nr mail6.nr \
	mail7.nr mail8.nr mail9.nr maila.nr
MACROS=	-me
TBL=tbl

all: manual.ps manual.txt

manual.ps: ${SRCS}
	${TBL} ${SRCS} | groff ${MACROS} -Tps > $@

manual.txt: ${SRCS}
	${TBL} ${SRCS} | groff ${MACROS} -Tascii  > $@

clean :
	-rm -f manual.ps manual.txt
