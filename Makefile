CC=g++

CXXFLAGS := -O3 -std=c++0x -Wall -g
LDFLAGS= $(shell pkg-config --cflags --libs libnotify)


PREFIX=/usr
BINDIR=${PREFIX}/bin
INSTALLDIR=${DESTDIR}${BINDIR}

SOURCE := batteryd.cpp
TARGET := batteryd

all: ${TARGET}

${TARGET}: ${SOURCE}
	@echo ${CC} $^ -o $@
	@${CC} ${CXXFLAGS} $^ -o $@  ${LDFLAGS}

install: 
	install -d ${INSTALLDIR}
	install -m 755 ${TARGET} ${INSTALLDIR}/${TARGET}

clean: 
	${RM} ${TARGET}

.PHONY: clean
