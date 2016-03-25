TARGET=sha256-random-attack
OBJ = main.o

CFLAGS = -Wall
LDFLAGS = -lcrypto -lrt

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $< -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
