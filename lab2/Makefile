CC=g++
CFLAGS=-I.
DIR = src
TARGET = sched
DEPS = $(TARGET).h
OBJ = $(TARGET).o
DIR_BIN = bin
FLAG = vep
S = F
INPUTFILE = lab2_assign/input0
RANDFILE = lab2_assign/rfile
$(DIR)/%.o: $(DIR)/%.cpp $(DIR)/$(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(DIR)/$(OBJ) $(DIR_BIN)
	$(CC) -o $(DIR_BIN)/$@ $(DIR)/$(OBJ) $(CFLAGS)

$(DIR_BIN):
	mkdir $@

run: $(DIR_BIN) $(TARGET)
	$(DIR_BIN)/$(TARGET) -$(FLAG) -s$(S) $(INPUTFILE) $(RANDFILE)

.PHONY: clean

diff: run
	make run -s > myoutput
	diff myoutput lab2_assign/refout_v/out_0_$(S) > log.txt

clean:
	rm -f *.o
	rm -f $(DIR)/*.o
