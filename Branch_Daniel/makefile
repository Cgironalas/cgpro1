CC=gcc

SOURCE= pro1.c
OUTPUT=pro1
OBJECT=pro1.o
LDFLAGS= -lglut -lGL -lGLU -lm
CFLAGS = -I/usr/include/GL


$(OUTPUT): $(OBJECT)
	$(CC) $(OBJECT) -o $(OUTPUT) $(LDFLAGS) $(CFLAGS) 

$(OBJECT): $(SOURCE)
	$(CC) -c $(SOURCE) -o pro1.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f pro1
