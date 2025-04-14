# Makefile for Zip Code Project 3.0

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# All source files
SRCS = main.cpp CSVBuffer.cpp DumpUtility.cpp InsertRecord.cpp DeleteRecord.cpp HeaderBuffer3.cpp BlockBuffer.cpp RecordBuffer.cpp

# Header dependencies (not compiled)
HDRS = CSVBuffer.h DumpUtility.h HeaderBuffer3.h BlockBuffer.h RecordBuffer.h HeaderAndBlockStructures.h

# Executables
MAIN = myProgram
INSERT = InsertRecord
DELETE = DeleteRecord

all: $(MAIN) $(INSERT) $(DELETE)

$(MAIN): main.cpp CSVBuffer.o DumpUtility.o
	$(CXX) $(CXXFLAGS) -o $@ main.cpp CSVBuffer.o DumpUtility.o

InsertRecord: InsertRecord.cpp BlockBuffer.o HeaderBuffer3.o
	$(CXX) $(CXXFLAGS) -o $@ InsertRecord.cpp BlockBuffer.o HeaderBuffer3.o

DeleteRecord: DeleteRecord.cpp BlockBuffer.o HeaderBuffer3.o
	$(CXX) $(CXXFLAGS) -o $@ DeleteRecord.cpp BlockBuffer.o HeaderBuffer3.o

CSVBuffer.o: CSVBuffer.cpp CSVBuffer.h HeaderBuffer.h
	$(CXX) $(CXXFLAGS) -c CSVBuffer.cpp

DumpUtility.o: DumpUtility.cpp DumpUtility.h BlockBuffer.h RecordBuffer.h
	$(CXX) $(CXXFLAGS) -c DumpUtility.cpp

BlockBuffer.o: BlockBuffer.cpp BlockBuffer.h HeaderAndBlockStructures.h
	$(CXX) $(CXXFLAGS) -c BlockBuffer.cpp

HeaderBuffer3.o: HeaderBuffer3.cpp HeaderBuffer3.h HeaderAndBlockStructures.h
	$(CXX) $(CXXFLAGS) -c HeaderBuffer3.cpp

RecordBuffer.o: RecordBuffer.cpp RecordBuffer.h
	$(CXX) $(CXXFLAGS) -c RecordBuffer.cpp

clean:
	rm -f *.o $(MAIN) $(INSERT) $(DELETE)