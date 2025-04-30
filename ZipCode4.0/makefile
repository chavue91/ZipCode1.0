# Makefile for B+Tree implementation

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

# Object files
OBJS = main.o CSVBuffer.o BPlusTree.o

# Target executables
TARGETS = zipcode zipcodes Bzipcodes

# Default target
all: $(TARGETS)

# Original zipcode program with B+Tree integration
zipcode: main.o CSVBuffer.o BPlusTree.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Object file rules
main.o: main.cpp CSVBuffer.h BPlusTree.h
	$(CXX) $(CXXFLAGS) -c $<

CSVBuffer.o: CSVBuffer.cpp CSVBuffer.h HeaderBuffer.h
	$(CXX) $(CXXFLAGS) -c $<

BPlusTree.o: BPlusTree.cpp BPlusTree.h CSVBuffer.h
	$(CXX) $(CXXFLAGS) -c $<

# Clean rule
clean:
	rm -f $(OBJS) $(TARGETS) *.dat

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build all executables"
	@echo "  zipcode       - Build the zipcode program"
	@echo "  clean         - Remove all object files and executables"

.PHONY: all clean help