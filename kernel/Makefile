OUTPUT = kernel.bin

override CC = x86_64-elf-gcc
override LD = x86_64-elf-ld
override ASM = nasm

CFLAGS = -Wall -Wextra \
	-std=gnu11 \
 	-ffreestanding -nostdlib \
	-fshort-wchar -mno-red-zone \
	-fno-PIC

INCLUDE_PATHS = -I src/ -I src/graphics -I src/memory -I src/arch/x86_64 -I src/arch/x86_64/cpu -I src/arch/x86_64/io \
				-I src/input -I src/drivers -I src/drivers/input -I src/shell

LINKFLAGS = -T linker.ld -static \
	-Bsymbolic

LIBS = -lgcc

ASMFLAGS = -f elf64

BUILD_DIR = ../bin

.PHONY: all kernel

all: kernel

C_SOURCES := $(shell find src -type f -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/x86_64/%.o, $(C_SOURCES))
ASM_SOURCES := $(shell find src -type f -name "*.asm")
ASM_OBJECTS := $(patsubst src/%.asm, $(BUILD_DIR)/kernel/x86_64/%_asm.o, $(ASM_SOURCES))

C_HEADERS := $(shell find src -type f -name "*.h")
ASM_HEADERS := $(shell find src -type f -name "*.inc")

kernel: $(BUILD_DIR)/kernel/$(OUTPUT)

$(BUILD_DIR)/kernel/$(OUTPUT): $(C_OBJECTS) $(ASM_OBJECTS)
	$(LD) $(LINKFLAGS) -o $(BUILD_DIR)/kernel/$(OUTPUT) $^

$(BUILD_DIR)/kernel/x86_64/arch/x86_64/cpu/interrupts.o: src/arch/x86_64/cpu/interrupts.c src/arch/x86_64/cpu/interrupts.h
	@ mkdir -p $(dir $@)
	$(CC) -Wall -Wextra -std=gnu11 -mno-red-zone -mgeneral-regs-only -ffreestanding $(INCLUDE_PATHS) -c -o $@ $<

$(BUILD_DIR)/kernel/x86_64/%.o: src/%.c $(C_HEADERS)
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(BUILD_DIR)/kernel/x86_64/%_asm.o: src/%.asm $(ASM_HEADERS)
	@ mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<


