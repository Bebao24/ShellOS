#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <stddef.h>

typedef struct
{
	void* BaseAddress;
	size_t BufferSize;
	unsigned int width;
	unsigned int height;
	unsigned int PixelsPerScanLine;
} GOP_Framebuffer;

GOP_Framebuffer framebuffer;
GOP_Framebuffer* InitializeGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

	if (EFI_ERROR(status))
	{
		Print(L"Failed to locate GOP!\r\n");
		return NULL;
	}
	else
	{
		Print(L"GOP located successfully!\r\n");
	}

	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.width = gop->Mode->Info->HorizontalResolution;
	framebuffer.height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* FileName, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

#define PSF_MAGIC_0 0x36
#define PSF_MAGIC_1 0x04

typedef struct
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charSize;
} PSF_Header;

typedef struct
{
    PSF_Header* fontHeader;
    void* glyphBuffer;
} PSF_Font;

PSF_Font* LoadFont(EFI_FILE* Directory, CHAR16* FontName, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* font = LoadFile(Directory, FontName, ImageHandle, SystemTable);
	if (font == NULL) return NULL;

	PSF_Header* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF_Header), (void**)&fontHeader);

	UINTN size = sizeof(PSF_Header);
	font->Read(font, &size, fontHeader);

	// Check if the file is PSF1 font
	if (fontHeader->magic[0] != PSF_MAGIC_0 || fontHeader->magic[1] != PSF_MAGIC_1)
	{
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charSize * 256;
	if (fontHeader->mode == 1)
	{
		glyphBufferSize = fontHeader->charSize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF_Header));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF_Font* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF_Font), (void**)&finishedFont);
	finishedFont->fontHeader = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* FileName, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_STATUS status;

	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL)
	{
		// Open the root directory
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	status = Directory->Open(Directory, &LoadedFile, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status != EFI_SUCCESS)
	{
		return NULL;
	}

	return LoadedFile;
}

int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++){
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}

typedef struct
{
	GOP_Framebuffer* fb;
	PSF_Font* font;
} BootInfo;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);

	EFI_FILE* kernel = LoadFile(NULL, L"kernel.bin", ImageHandle, SystemTable);
	if (kernel == NULL)
	{
		Print(L"kernel.bin not found!\r\n");
		return EFI_NOT_FOUND;
	}
	else
	{
		Print(L"kernel.bin found!\r\n");
	}

	// Read the ELF header to verify if it is ELF64
	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		kernel->Read(kernel, &size, &header);
	}

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
	{
		Print(L"kernel.bin header isn't ELF64\r\n");
		return EFI_INVALID_PARAMETER;
	}
	else
	{
		Print(L"kernel.bin verified!\r\n");
	}

	Elf64_Phdr* phdrs;
	{
		kernel->SetPosition(kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		kernel->Read(kernel, &size, phdrs);
	}

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	)
	{
		switch (phdr->p_type)
		{
			case PT_LOAD:
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel, &size, (void*)segment);
		}
	}

	Print(L"Kernel loaded successfully!\r\n");

	GOP_Framebuffer* newBuffer = InitializeGOP();
	if (newBuffer == NULL)
	{
		return EFI_NOT_FOUND;
	}

	PSF_Font* newFont = LoadFont(NULL, L"default.psf", ImageHandle, SystemTable);
	if (newFont == NULL)
	{
		Print(L"Font not found!\r\n");
		return EFI_INVALID_PARAMETER;
	}
	else
	{
		Print(L"Font found! char size = %d\r\n", newFont->fontHeader->charSize);
	}

	void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*)) header.e_entry);

	BootInfo bootInfo;
	bootInfo.fb = newBuffer;
	bootInfo.font = newFont;
	KernelStart(&bootInfo);

	return EFI_SUCCESS;
}
