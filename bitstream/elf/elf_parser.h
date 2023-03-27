#ifndef BITSTREAM_ELF_PARSER_H
#define BITSTREAM_ELF_PARSER_H

#include <elf.h>
#include <unistd.h>
#include <fcntl.h>

#include "../common.h"
#include "../utils.h"

#define AOCX_FREQ_SECTION ".acl.quartus_json"
#define AOCX_BOARD_SPEC_SECTION ".acl.board_spec.xml"

bitstream_object parse_elf(int fd);

#endif // BITSTREAM_ELF_PARSER_H
