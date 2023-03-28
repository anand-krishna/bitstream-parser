#include "parser.h"

#include <string.h>

#include "elf/elf_parser.h"
#include "axlf/axlf_parser.h"

#define AOCX_MAGIC ELFMAG // AOCX files are ELF files.
#define XCLBIN_MAGIC "xclbin2\0"

bitstream_t get_bitstream_type(int fd);

bitstream_object parse_bitstream(bitstream_file_handler *bfd)
{
    if (bfd->bitstream_type == AOCX)
    {
        return parse_elf(bfd->os_file_descriptor);
    }
    else if (bfd->bitstream_type == XCLBIN)
    {
        return parse_axlf(bfd->os_file_descriptor, bfd->filepath);
    }
}

bitstream_file_handler open_bitstream_file(const char *file_path)
{
    bitstream_file_handler bfd;

    strcpy(bfd.filepath, file_path);

    if (file_path == NULL)
    {
        fprintf(stdout, "[%s:%d:\t%s] Filepath cannot be null.\n", __FILE__, __LINE__, __func__);
        exit(-1);
    }

    fprintf(stdout, "[%s:%d:\t%s] Opening the bitstream file.\n", __FILE__, __LINE__, __func__);
    int fd = open(file_path, O_RDONLY | O_SYNC);

    if (fd == -1)
    {
        fprintf(stdout, "[%s:%d:\t%s] Could not open the file.\n", __FILE__, __LINE__, __func__);
        exit(-1);
    }

    bfd.os_file_descriptor = fd;

    // Determine the type of the bitstream file.
    bitstream_t bitstream_type = get_bitstream_type(fd);

    if (bitstream_type == AOCX)
    {
        bfd.bitstream_type = bitstream_type;
    }
    else if (bitstream_type == XCLBIN)
    {
        bfd.bitstream_type = bitstream_type;
    }
    else
    {
        fprintf(stdout, "[%s:%d:%s] Cannot parse the file. Only Intel and Xilinx bitstreams are supported.\n", __FILE__, __LINE__, __func__);
        exit(-1);
    }

    return bfd;
}

void close_bitstream_file(bitstream_file_handler *to_close)
{
    if (close(to_close->os_file_descriptor) == -1)
    {
        // TODO: Handle this.
    }
}

bitstream_t get_bitstream_type(int fd)
{
    bitstream_t btype;
    char buffer[8];

    if (read(fd, buffer, sizeof(buffer)) == -1)
    {
        // TODO: Handle this.
    }

    if (strncmp(buffer, AOCX_MAGIC, 4) == 0)
    {
        btype = AOCX;
    }
    else if (strncmp(buffer, XCLBIN_MAGIC, 8) == 0)
    {
        btype = XCLBIN;
    }
    else
    {
        btype = UNDEFINED;
    }

    return btype;
}
