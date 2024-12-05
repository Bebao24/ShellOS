#include "shell.h"
#include <input.h>
#include <stdbool.h>
#include <memory.h>
#include <console.h>
#include <ctype.h>
#include <string.h>
#include <pmm.h>

void start_shell(BootInfo* bootInfo)
{
    char cmdString[256] = { 0 };
    char* cmdStringPtr = cmdString;

    int argc = 0;
    char* argv[10] = { 0 };

    char input_char = 0;
    uint8_t inputLength;

    while (true)
    {
        memset(cmdString, 0, sizeof(cmdString));
        
        printf("$ ");
        inputLength = 0;

        while (true)
        {
            input_char = get_key();

            // Enter key
            if (input_char == '\r')
            {
                putc('\n');
                break;
            }

            // Backspace
            if (input_char == '\b')
            {
                if (inputLength > 0)
                {
                    inputLength--;
                    cmdString[inputLength] = '\0';

                    putc('\b');
                }

                continue;
            }

            cmdString[inputLength] = input_char;
            inputLength++;

            putc(input_char);
        }

        // Don't tokenize the input if there is nothing
        if (inputLength == 0)
        {
            putc('\n');
            continue;
        }

        cmdString[inputLength] = '\0';

        cmdStringPtr = cmdString;
        argc = 0;
        memset(argv, 0, sizeof(argv));

        // Tokenize the input
        while (*cmdStringPtr != '\0')
        {
            // Skip all blank space
            while (isspace(*cmdStringPtr)) *cmdStringPtr++ = '\0';

            argv[argc++] = cmdStringPtr;

            if (*cmdStringPtr == '\"')
            {
                cmdStringPtr++;

                while (*cmdStringPtr != '\"')
                {
                    cmdStringPtr++;
                }
            }

            while (!isspace(*cmdStringPtr) && *cmdStringPtr != '\0')
            {
                cmdStringPtr++;
            }
        }

        if (strcmp(argv[0], "echo") == 0)
        {
            if (argc < 2)
            {
                printf("Invalid syntax!\n");
                continue;
            }

            for (int i = 1; i < argc; i++)
            {
                printf("%s ", argv[i]);
            }

            putc('\n');
        }
        else if (strcmp(argv[0], "clear") == 0)
        {
            clearScreen();
        }
        else if (strcmp(argv[0], "memInfo") == 0)
        {
            uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;
            printf("Total memory: %llu KB\n", GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mDescriptorSize) / 1024);
            printf("Free memory: %llu KB\n", pmm_GetFreeRAM() / 1024);
            printf("Used memory: %llu KB\n", pmm_GetUsedRAM() / 1024);
            printf("Reserved memory: %llu KB\n", pmm_GetReservedRAM() / 1024);
        }
        else
        {
            printf("Invalid command!\n");
        }
    }
}

