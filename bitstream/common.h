#ifndef BITSTREAM_PARSER_COMMON_H
#define BITSTREAM_PARSER_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct bitstream_object_
{
    char device_name[256];
    double kernel_frequency;
    double system_frequency;
} bitstream_object;

#endif // BITSTREAM_PARSER_COMMON_H
