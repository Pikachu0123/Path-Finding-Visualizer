CC = g++

SOURCES = main.cpp

TARGET = main.exe

CFLAGS = -o $(TARGET) -lraylib -lm -lpthread -ldl -lrt -lX11

all: $(TARGET) run

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
