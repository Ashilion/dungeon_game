# Variables
SRC = $(shell find src -name '*.cpp')
OBJ = $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
BUILD_DIR = build
SFML_INCLUDE = ~/SFML-3.0.0/include
SFML_LIB = ~/SFML-3.0.0/lib
LD_LIBRARY_PATH = ~/SFML-3.0.0/lib
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = $(BUILD_DIR)/sfml-app

# Default rule
all: $(TARGET)

# Compilation of object files
$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	g++ -c $< -I $(SFML_INCLUDE) -o $@

# Linking object files
$(TARGET): $(OBJ)
	g++ $^ -o $@ -L $(SFML_LIB) $(SFML_LIBS)

# Run the application
run: $(TARGET)
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) && ./$(TARGET)

# Clean up
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
