CC = g++
CFLAGS = -g -Wall -Wextra
LINKER_FLAGS = 

# src/tests holds the old CPU-only unit tests, written against CPU's former
# direct-array memory model. They don't compile against the Bus-based CPU
# and aren't wired into a runner yet, so they're excluded from the default
# build until they're ported (see notes.md).
SRC = $(shell find src -name "*.cpp" -not -path "src/tests/*")
OBJ = $(SRC:.cpp=.o)
BIN = bin
NAME = nesemu

.PHONY: all clean

all: dirs build

dirs:
	mkdir -p ./$(BIN)

build: dirs $(OBJ)
	$(CC) -o $(BIN)/$(NAME) $(filter %.o,$^) -w $(LINKER_FLAGS) $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
