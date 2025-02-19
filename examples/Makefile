# Makefile for building the example program

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -I../include

# Linker flags
LDFLAGS = -L../lib -lpl_search

# Source files
SRCS = send_more_money.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
EXEC = send_more_money

# Build the executable
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

# Dependencies
send_more_money.o: send_more_money.cpp