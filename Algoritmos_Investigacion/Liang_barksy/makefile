CC=gcc

SOURCE= main.c
OUTPUT=liang
OBJECT=liang.o
LDFLAGS= -lglut -lGL -lGLU -lm
CFLAGS = -I/usr/include/GL


$(OUTPUT): $(OBJECT)
	$(CC) $(OBJECT) -o $(OUTPUT) $(LDFLAGS) $(CFLAGS) 

$(OBJECT): $(SOURCE)
	$(CC) -c $(SOURCE) -o liang.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f liang
