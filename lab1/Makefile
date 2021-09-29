CC=g++
CFLAGS=-I.
TARGET=xw27882passlinker
DEPS = $(TARGET).h
OBJ = $(TARGET).o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o ./bin/$@ $^ $(CFLAGS)

run: ./bin/$(TARGET)
	@./bin/$(TARGET)

.PHONY: clean

clean:
	@rm -f *.o

