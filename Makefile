.PHONY: all image bootloader kernel always clean run

all: image

BUILD_DIR = bin
GNU_EFI = gnu-efi
OVMF_DIR = OVMFbin

image: $(BUILD_DIR)/image.hdd
$(BUILD_DIR)/image.hdd: kernel bootloader
	dd if=/dev/zero of=$@ bs=512 count=93750
	mformat -i $@ ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(GNU_EFI)/x86_64/bootloader/main.efi ::/EFI/BOOT
	mcopy -i $@ startup.nsh ::
	mcopy -i $@ $(BUILD_DIR)/kernel/kernel.bin ::
	mcopy -i $@ build/default.psf ::

kernel: $(BUILD_DIR)/kernel/kernel.bin
$(BUILD_DIR)/kernel/kernel.bin: always
	@ $(MAKE) -C kernel

bootloader: $(GNU_EFI)/x86_64/bootloader/main.efi
$(GNU_EFI)/x86_64/bootloader/main.efi: always
	@ $(MAKE) -C gnu-efi
	@ $(MAKE) -C gnu-efi bootloader

always:
	@ mkdir -p $(BUILD_DIR)

clean:
	@ rm -rf bin/*

run:
	qemu-system-x86_64 -drive file=$(BUILD_DIR)/image.hdd \
	-m 256M -cpu qemu64 \
	-drive if=pflash,format=raw,unit=0,file="$(OVMF_DIR)/OVMF_CODE-pure-efi.fd",readonly=on \
	-drive if=pflash,format=raw,unit=1,file="$(OVMF_DIR)/OVMF_VARS-pure-efi.fd" \
	-net none

