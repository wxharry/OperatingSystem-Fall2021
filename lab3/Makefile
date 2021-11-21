CC=g++
CFLAGS=-I.
TARGET = mmu
NUM=1
OBJ_DIR   = objects
SRC       = $(call rwildcard, src, *.cpp, *.h)
FRAMES = 4
FLAGS = -f$(FRAMES) -af -oOPFS
INPUTFILE = ./inputs/in$(NUM)
RANDOMFILE = ./inputs/rfile
OUTPUTFILE = out$(NUM)_16_f

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
	./$(TARGET) $(FLAGS) $(INPUTFILE) $(RANDOMFILE) 

diff:
	@make -s run >  ./myoutput/$(OUTPUTFILE)
	@diff myoutput/$(OUTPUTFILE) refout/$(OUTPUTFILE) > diff.txt

.PHONY: all build clean debug release

clean:
	-@rm -rvf $(OBJ_DIR)/* 
	$(RM) $(TARGET)