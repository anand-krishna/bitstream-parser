#ifndef BITSTREAM_AXLF_PARSER_H
#define BITSTREAM_AXLF_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <fcntl.h>

#define XCLBIN_FREQ_SECTION ".acl.quartus_json"
#define XCLBIN_METADATA_SECTION ".acl.quartus_json"

void parse_axlf(int fd, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // BITSTREAM_AXLF_PARSER_H
