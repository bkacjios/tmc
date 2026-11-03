// Maps the original ROM at its GBA address (0x08000000), read-only.
//
// Some data is still included as raw binary from the ROM (songs, some tables)
// and holds GBA ROM addresses. With the ROM mapped there, those pointers lead
// to the same bytes as on the GBA.
//
// The ROM is found at $TMC_ROM, ./baserom.gba, or baserom.gba in the
// repository root (two levels above the executable), unless the executable
// has it built in (embed.c).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

#define ROM_ADDR ((void*)0x08000000)

#ifdef _WIN32
#include <windows.h>

static int MapFile(const char* path) {
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE mapping;
    void* view;
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    if (GetFileSize(file, NULL) < PC_ROM_SIZE) {
        CloseHandle(file);
        return -1;
    }
    mapping = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    PcReleaseReservation(ROM_ADDR);
    view = mapping ? MapViewOfFileEx(mapping, FILE_MAP_READ, 0, 0, PC_ROM_SIZE, ROM_ADDR) : NULL;
    CloseHandle(file);
    return view == ROM_ADDR ? 1 : -1;
}

static void ExeDir(char* buf, size_t size) {
    char* slash;
    GetModuleFileNameA(NULL, buf, (DWORD)size);
    slash = strrchr(buf, '\\');
    if (slash)
        slash[1] = '\0';
}
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

static int MapFile(const char* path) {
    struct stat st;
    void* p;
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return 0;
    if (fstat(fd, &st) != 0 || st.st_size < PC_ROM_SIZE) {
        close(fd);
        return -1;
    }
    p = mmap(ROM_ADDR, PC_ROM_SIZE, PROT_READ, MAP_PRIVATE | MAP_FIXED_NOREPLACE, fd, 0);
    close(fd);
    return p == ROM_ADDR ? 1 : -1;
}

static void ExeDir(char* buf, size_t size) {
    ssize_t n = readlink("/proc/self/exe", buf, size - 1);
    char* slash;
    buf[n > 0 ? n : 0] = '\0';
    slash = strrchr(buf, '/');
    if (slash)
        slash[1] = '\0';
}
#endif

// Finds a data file the user provides: $env if set, else name in the working
// directory, else name two levels above the executable (the repository root
// for build/<dir>/tmc). Returns 1 with its path, or 0.
int PcFindFile(const char* env, const char* name, char* path, size_t size) {
    const char* value = getenv(env);
    FILE* f;
    if (value != NULL) {
        snprintf(path, size, "%s", value);
    } else {
        snprintf(path, size, "%s", name);
        if ((f = fopen(path, "rb")) != NULL) {
            fclose(f);
            return 1;
        }
        ExeDir(path, size);
        strncat(path, "../../", size - strlen(path) - 1);
        strncat(path, name, size - strlen(path) - 1);
    }
    if ((f = fopen(path, "rb")) == NULL)
        return 0;
    fclose(f);
    return 1;
}

void PcMapRom(void) {
    char path[4096];
    int result;

    if (gPcEmbeddedRom != NULL) {
        if (!PcMapReadOnlyCopy(ROM_ADDR, gPcEmbeddedRom, PC_ROM_SIZE))
            PcFatal("cannot map the ROM at 0x08000000");
        return;
    }
    if (!PcFindFile("TMC_ROM", "baserom.gba", path, sizeof(path)))
        PcFatal("baserom.gba not found (put it in the working directory, or set TMC_ROM to its path)");
    result = MapFile(path);
    if (result <= 0)
        PcFatal("cannot map %s at 0x08000000", path);
}
