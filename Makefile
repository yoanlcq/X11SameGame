CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -O3
LDLIBS = -Wl,-rpath=\$$ORIGIN/lib -L./lib -lgraph
EXE = SameGame
OFILES = build/main.o \
         build/chronodef.o \
         build/rectangledef.o \
         build/grilledef.o \
         build/jeudef.o \
         build/zonedef.o \
         build/confdef.o \
         build/menudef.o


all : $(EXE)

build/confdef.o : src/confdef.c src/confdef.h
	$(CC) $(CFLAGS) -o build/confdef.o -c src/confdef.c

build/chronodef.o : src/chronodef.c src/chronodef.h
	$(CC) $(CFLAGS) -o build/chronodef.o -c src/chronodef.c

build/rectangledef.o : src/rectangledef.c src/rectangledef.h
	$(CC) $(CFLAGS) -o build/rectangledef.o -c src/rectangledef.c

build/grilledef.o : src/grilledef.c src/grilledef.h src/rectangledef.h
	$(CC) $(CFLAGS) -o build/grilledef.o -c src/grilledef.c

build/zonedef.o : src/zonedef.c src/zonedef.h src/rectangledef.h
	$(CC) $(CFLAGS) -o build/zonedef.o -c src/zonedef.c

build/jeudef.o : src/jeudef.c src/jeudef.h src/grilledef.h src/zonedef.h src/chronodef.h src/rectangledef.h
	$(CC) $(CFLAGS) -o build/jeudef.o -c src/jeudef.c

build/menudef.o : src/menudef.c src/menudef.h
	$(CC) $(CFLAGS) -o build/menudef.o -c src/menudef.c

build/main.o : src/univ.h src/main.c src/menudef.h src/jeudef.h src/grilledef.h src/zonedef.h src/chronodef.h src/rectangledef.h
	$(CC) $(CFLAGS) -o build/main.o -c src/main.c

$(EXE) : $(OFILES)
	$(CC) $(CFLAGS) -o $(EXE) $(OFILES) $(LDFLAGS) $(LDLIBS)


clean :
	rm -f ${OFILES}
mrproper : clean all
re : clean all
.PHONY : all clean mrproper
