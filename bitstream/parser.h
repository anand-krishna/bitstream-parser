#ifndef BITSTREAM_PARSER_H
#define BITSTREAM_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "common.h"
#include "utils.h"

#include "elf/elf_parser.h"

typedef enum bitstream_types
{
    AOCX,
    XCLBIN,
    UNDEFINED
} bitstream_t;

typedef struct bitstream_file_handler_
{
    char filepath[256];
    int os_file_descriptor; // The fd returned by open().
    bitstream_t bitstream_type;
} bitstream_file_handler; // TODO: Maybe renaming this would be good.

void parse_bitstream(bitstream_file_handler *bfd);
bitstream_file_handler open_bitstream_file(const char *file_path);
void close_bitstream_file(bitstream_file_handler *to_close);

#endif // BITSTREAM_PARSER_H
