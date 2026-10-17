# Makefile for the Front End (CSCI 3060U course project)
#
#   make              build the Front End (frontend, or frontend.exe on Windows)
#   make test         build, then run every test in TestCases/ with scripts/run_tests.sh (run_tests.bat on Windows)
#   make clean        delete the compiled program and object files
#   make CXX=g++      build with a different compiler (default: clang++ on Windows, g++ elsewhere)

CXXFLAGS = -std=c++17 -Wall -I src
SOURCES = $(wildcard src/*.cpp src/*/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

ifeq ($(OS),Windows_NT)
    CXX = clang++
    TARGET = frontend.exe
    LDFLAGS = -static
    RM = cmd /c del /q
    RUN_TESTS = cmd /c scripts\run_tests.bat frontend.exe
    FIX_PATH = $(subst /,\,$(1))
else
    CXX = g++
    TARGET = frontend
    LDFLAGS =
    RM = rm -f
    RUN_TESTS = bash scripts/run_tests.sh ./frontend
    FIX_PATH = $(1)
endif

.PHONY: all test clean

all: $(TARGET)

# Links every object file into the Front End program.
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compiles one .cpp file; -MMD -MP also records which headers it uses, so editing a .h rebuilds the right files.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Runs the full requirements test suite against the program.
test: $(TARGET)
	$(RUN_TESTS)

clean:
	-$(RM) $(call FIX_PATH,$(TARGET) $(OBJECTS) $(OBJECTS:.o=.d))

-include $(OBJECTS:.o=.d)