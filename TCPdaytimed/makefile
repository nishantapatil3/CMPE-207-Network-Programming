

INCLUDE =

CLNTS = TCPdaytime
SERVS = TCPdaytimed

DEFS = -D_BSD_SOURCE
CFLAGS = -W -pedantic -ansi -g ${DEFS} ${INCLUDE}

HDR =
CSRC = TCPdaytime.c
CXSRC = connectTCP.c connectsock.c errexit.c

SSRC = TCPdaytimed.c
SXSRC = passiveTCP.c passivesock.c

CXOBJ = connectTCP.o connectsock.o errexit.o
SXOBJ = passiveTCP.o passivesock.o errexit.o

PROGS = ${CLNTS} ${SERVS}

all: ${PROGS}

${CLNTS}: ${CXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${CXOBJ}

${SERVS}: ${SXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ}

clients: ${CLNTS}
servers: ${SERVS}

clean: FRC
	rm -f Makefile.bak a.out core errs lint.errs ${PROGS} *.o

TCPdaytime: TCPdaytime.o
TCPdaytimed: TCPdaytimed.o

FRC:
	
# DO NOT DELETE THIS LINE - maketd DEPENDS ON IT
