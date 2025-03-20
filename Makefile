# Variables
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
BUILD_DIR = build
SFML_INCLUDE = ~/SFML-3.0.0/include
SFML_LIB = ~/SFML-3.0.0/lib
LD_LIBRARY_PATH = ~/SFML-3.0.0/lib
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = $(BUILD_DIR)/sfml-app

# Règle par défaut
all: $(TARGET)

# Compilation des fichiers objet
$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(BUILD_DIR)
	g++ -c $< -I $(SFML_INCLUDE) -o $@

# Lien des fichiers objets
$(TARGET): $(OBJ)
	g++ $^ -o $@ -L $(SFML_LIB) $(SFML_LIBS)

# Exécution de l'application
run: $(TARGET)
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) && ./$(TARGET)

# Nettoyage
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
