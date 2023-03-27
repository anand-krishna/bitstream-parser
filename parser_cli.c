#include <elf.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "bitstream/parser.h"

int main(int argc, char *argv[])
{
    bitstream_file_handler bfd;

    if (argc < 2)
    {
        fprintf(stdout, "Error: Missing filename. Need to specify the file name.\n\tUsage: bitstream-parser <BITSTREAM_FILE>\n");
        return 0;
    }

    bfd = open_bitstream_file(argv[1]);

    parse_bitstream(&bfd);

    return 0;
}