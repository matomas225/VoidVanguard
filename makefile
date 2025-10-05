# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lSDL2 -lSDL2_mixer -lm

# Windows cross-compilation (adjust SDL2_PATH to your SDL2 folder)
CC_WIN = x86_64-w64-mingw32-gcc
SDL2_PATH = /home/matomas/SDL2-windows/x86_64-w64-mingw32
CFLAGS_WIN = -Wall -Wextra -std=c99 -I$(SDL2_PATH)/include
LDFLAGS_WIN = -L$(SDL2_PATH)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lm -mwindows

# Source files
SRCS = main.c enemy.c dieMenu.c projectile.c mainMenu.c soundMenu.c upgradeMenu.c
OBJS = $(SRCS:.c=.o)
TARGET = VoidVanguard

# Windows objects and target
OBJS_WIN = $(SRCS:.c=_win.o)
TARGET_WIN = VoidVanguard.exe

# Resource file for Windows
resources.o: resources.rc
	x86_64-w64-mingw32-windres resources.rc -o resources.o

# Default target (Linux)
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Windows target
windows: $(TARGET_WIN)

$(TARGET_WIN): $(OBJS_WIN) resources.o
	$(CC_WIN) $(OBJS_WIN) resources.o -o $(TARGET_WIN) $(LDFLAGS_WIN)

%_win.o: %.c
	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET) $(OBJS_WIN) $(TARGET_WIN) resources.o

# Run the game (Linux)
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run windows

