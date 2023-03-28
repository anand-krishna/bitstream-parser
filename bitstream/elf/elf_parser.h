#ifndef BITSTREAM_ELF_PARSER_H
#define BITSTREAM_ELF_PARSER_H

#include <elf.h>
#include <unistd.h>
#include <fcntl.h>

#include "../common.h"

bitstream_object parse_elf(int fd);

#endif // BITSTREAM_ELF_PARSER_H
