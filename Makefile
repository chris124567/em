CFLAGS :=  -ansi -flto -Ofast -Wconversion -Wundef -Wstrict-prototypes -Wsign-conversion   -Wno-error -pedantic -Wall -Wextra -march=native -fstack-protector-all  `sdl2-config --cflags --libs` -Wno-unused-parameter  -std=c89 -lSDL -I/usr/include/SDL2 -Waddress  -Warray-bounds  -Wpedantic -Wsign-conversion -Wuninitialized -Wunreachable-code -Wswitch-default -Wswitch -Wcast-align -Wmissing-include-dirs -Winit-self -Wdouble-promotion
CC=c89
INSTALLDIR := /usr/local/bin
CPARGS := -fv
.POSIX all: main 

clean:
	@rm main

#install:
#	@echo Cleaning existing install..
#	@rm ${INSTALLDIR}/kpmpd ${CPARGS}
#	@rm /usr/share/bash-completion/completions/kpmpd ${CPARGS}
#	@echo Installing
#	@cp main ${INSTALLDIR}/kpmpd ${CPARGS}
#
#	@cp misc/kpmpd /usr/share/bash-completion/completions/kpmpd ${CPARGS}
