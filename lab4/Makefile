CC=g++
CFLAGS=-I.
TARGET = iosched
OBJ_DIR   = objects
SRC       = $(call rwildcard, src, *.cpp, *.h)
NUM = 0
ALGO = j
FLAGS = 
INPUTFILE = ./lab4_assign/input$(NUM)
OUTPUTFILE = out_$(NUM)_$(ALGO)

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) \
  $(filter $(subst *,%,$2),$d))

OBJECTS  = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -std=c++11 $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) -std=c++11 $(CFLAGS) -o $(TARGET) $^

build:
	@mkdir -p $(OBJ_DIR)

run: $(TARGET)
	./$(TARGET) -s$(ALGO) $(FLAGS) $(INPUTFILE)

diff:
	@make -s run >  ./myoutputs/$(OUTPUTFILE)
	diff myoutputs/$(OUTPUTFILE) ./lab4_assign/$(OUTPUTFILE) > diff.txt

.PHONY: all build clean debug release

clean:
	-@rm -rvf $(OBJ_DIR)/* 
	$(RM) $(TARGET) $(TARGET).exe