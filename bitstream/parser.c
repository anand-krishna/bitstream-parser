#include "parser.h"

#include <string.h>

#define AOCX_MAGIC ELFMAG // AOCX files are ELF files.
#define XCLBIN_MAGIC "xclbin2\0"

bitstream_t get_bitstream_type(int fd);

void parse_bitstream(bitstream_file_handler *bfd)
{

    bitstream_object bo;

    if (bfd->bitstream_type == AOCX)
    {
        bo = parse_elf(bfd->os_file_descriptor);
    }
    else if (bfd->bitstream_type == XCLBIN)
    {
        // TODO: Use the XRT API.
    }

    fprintf(stdout, "[%s: %s:%d] Contents of the parsed bitstream are:\n\tDevice ID:%s\n\tKernel Frequency:%f\n\tSystem Frequency:%f\n", __FILE__, __func__, __LINE__, bo.device_name, bo.kernel_frequency, bo.system_frequency);
}

bitstream_file_handler open_bitstream_file(const char *file_path)
{
    bitstream_file_handler bfd;

    strcpy(bfd.filepath, file_path);

    if (file_path == NULL)
    {
        parser_print_to(stderr, __FILE__, __func__, __LINE__, "File path cannot be NULL.\n");
        exit(-1);
    }

    parser_print_to(stdout, __FILE__, __func__, __LINE__, "Opening the bitstream file.\n");
    int fd = open(file_path, O_RDONLY | O_SYNC);

    if (fd == -1)
    {
        parser_print_to(stderr, __FILE__, __func__, __LINE__, "Could not open the file.\n");
        exit(-1);
    }

    bfd.os_file_descriptor = fd;

    // Determine the type of the bitstream file.
    parser_print_to(stdout, __FILE__, __func__, __LINE__, "Reading the magic bytes.\n");
    bitstream_t bitstream_type = get_bitstream_type(fd);

    if (bitstream_type == AOCX)
    {
        parser_print_to(stdout, __FILE__, __func__, __LINE__, "Opened an Intel AOCX bitstream.\n");
        bfd.bitstream_type = bitstream_type;
    }
    else if (bitstream_type == XCLBIN)
    {
        parser_print_to(stdout, __FILE__, __func__, __LINE__, "Opened an AMD XCLBIN bitstream.\n");
        bfd.bitstream_type = bitstream_type;
    }
    else
    {
        parser_print_to(stderr, __FILE__, __func__, __LINE__, "Cannot parse the file. Only Intel and AMD target bitstreams are supported\n.");
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
