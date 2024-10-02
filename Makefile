# Compiler
CC = g++

# Source files
SOURCES = main.cpp

# Output executable name
TARGET = main.exe

# Compiler flags
CFLAGS = -o $(TARGET) -lraylib -lm -lpthread -ldl -lrt -lX11

# Default target
all: $(TARGET) run

# Build the target executable
$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS)

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)
