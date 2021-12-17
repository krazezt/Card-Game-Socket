CXX		:= gcc

BIN		:= bin
SRC		:= src
INCLUDE	:= include

LIBRARIES	:=
EXECUTABLE	:= server


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	$(CXX) -g -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
