CC = g++
CFLAGS = -g -Wall -Wextra

LINKER_FLAGS = 

SRC = $(shell find src -name "*.cpp")
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
