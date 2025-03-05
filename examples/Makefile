# Makefile for building the example programs

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -I../include

# Linker flags
LDFLAGS = -L../lib -lpl_search

# Source files
SRCS_SMM = send_more_money.cpp
SRCS_PL = prolog_list.cpp

# Object files
OBJS_SMM = $(SRCS_SMM:.cpp=.o)
OBJS_PL = $(SRCS_PL:.cpp=.o)

# Executable
EXEC_SMM = send_more_money
EXEC_PL = prolog_list

# Build the executable
all: $(EXEC_SMM) $(EXEC_PL)

$(EXEC_SMM): $(OBJS_SMM)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(EXEC_PL): $(OBJS_PL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)	

# Clean up
clean:
	rm -f $(OBJS_SMM) $(EXEC_SMM)
	rm -f $(OBJS_PL) $(EXEC_PL)

# Dependencies
send_more_money.o: send_more_money.cpp
prolog_list.o : prolog_list.cpp 