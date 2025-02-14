CC = g++
CPPFLAGS = -g -Wall -O2
LDFLAGS =

OBJ = CplexSolver.o LSSolver.o TSPSolution.o main.o

CPX_BASE    = /opt/ibm/ILOG/CPLEX_Studio2211
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_linux/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl

%.o: %.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)
		
clean:
		rm -rf $(OBJ) main

.PHONY: clean
