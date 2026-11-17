# Builds the Front End into ./frontend


CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
DEPFLAGS = -MMD -MP

SRC_DIR   = src
BUILD_DIR = build
TARGET    = frontend

# every .cpp, found automattically
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# src/User.cpp -> build/User.o
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Link: join all .o files into ./frontend
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile one .cpp into one .o and one .h into one .d
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: clean

# Recompile a .cpp when a header it uses changes
-include $(OBJECTS:.o=.d)