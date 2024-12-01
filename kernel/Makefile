OUTPUT = kernel.bin

override CC = x86_64-elf-gcc
override LD = x86_64-elf-ld

CFLAGS = -Wall -Wextra \
	-std=gnu11 \
 	-ffreestanding -nostdlib \
	-fshort-wchar -mno-red-zone \
	-fno-PIC

CFLAGS += -I src/ -I src/graphics -I src/memory

LINKFLAGS = -T linker.ld -static \
	-Bsymbolic

LIBS = -lgcc

BUILD_DIR = ../bin

.PHONY: all kernel

all: kernel

C_SOURCES := $(shell find src -type f -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/x86_64/%.o, $(C_SOURCES))

C_HEADERS := $(shell find src -type f -name "*.h")

kernel: $(BUILD_DIR)/kernel/$(OUTPUT)

$(BUILD_DIR)/kernel/$(OUTPUT): $(C_OBJECTS)
	$(LD) $(LINKFLAGS) -o $(BUILD_DIR)/kernel/$(OUTPUT) $^

$(BUILD_DIR)/kernel/x86_64/%.o: src/%.c $(C_HEADERS)
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
