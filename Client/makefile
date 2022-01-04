CXX		:= gcc

BIN		:= bin
SRC		:= src
INCLUDE	:= include

LIBRARIES	:=
EXECUTABLE	:= client


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	$(CXX) -g -pthread -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
