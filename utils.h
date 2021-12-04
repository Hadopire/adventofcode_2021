#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

const char* mapFile(const char* fname, size_t& length)
{
    int fd = open(fname, O_RDONLY);
    if (fd == -1)
        return nullptr;

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        return nullptr;
    }

    length = sb.st_size;

    const char* addr = static_cast<const char*>(mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED)
        return nullptr;

    return addr;
}