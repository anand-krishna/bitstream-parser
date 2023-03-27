#ifndef BITSTREAM_AXLF_PARSER_H
#define BITSTREAM_AXLF_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <fcntl.h>

#include "../common.h"
#include "../utils.h"

bitstream_object parse_axlf(int fd, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // BITSTREAM_AXLF_PARSER_H
