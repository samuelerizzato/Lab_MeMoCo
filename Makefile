CC = g++
CPPFLAGS = -g -Wall -O2
LDFLAGS =

OBJ = WeightsFileManager.o main.o

%.o: %.cpp
		$(CC) $(CPPFLAGS) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main
		
clean:
		rm -rf $(OBJ) main

.PHONY: clean
