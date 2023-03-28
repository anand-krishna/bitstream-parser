#include "elf_parser.h"

#include <string.h>
#include <errno.h>

#include <libxml/parser.h>
#include <jansson.h>

#define AOCX_FREQ_SECTION ".acl.quartus_json"
#define AOCX_BOARD_SPEC_SECTION ".acl.board_spec.xml"

#define XML_PROP_NAME "name"
#define JSON_PROP_NAME "quartusFitClockSummary"

// Storage class of the file.
typedef enum
{
    BIT_32,
    BIT_64
} elf_class;

elf_class get_elf_class(int fd);

// Elf64_Ehdr
bitstream_object parse64(int fd);

// Elf32_Ehdr
bitstream_object parse32(int fd);
void read_header32(int fd, Elf32_Ehdr *header);
void read_section_table32(int fd, Elf32_Ehdr *header, Elf32_Shdr *section_table);
char *read_section32(int fd, Elf32_Shdr *section);
// Actual parsing utils.
void parse32_metadata(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, char *board_name);
void parse32_clock(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, double *kern_freq, double *sys_freq);
// Some general utils.
void print_section(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, const char *section_name);

elf_class get_elf_class(int fd)
{
    elf_class ec;
    char *buffer = malloc(sizeof(char));
    off_t offset = lseek(fd, (off_t)4, SEEK_SET);
    if (offset != (off_t)4)
    {
        fprintf(stdout, "[%s:%d:\t%s] Could not realign the read pointer.\n", __FILE__, __LINE__, __func__);
        printf("\tError message: %s.\n", strerror(errno));
        exit(-1);
    }
    int read_ret_val = read(fd, buffer, 1);
    if (read_ret_val == -1)
    {
        fprintf(stdout, "[%s:%d:\t%s] Could not read a byte from the file.\n", __FILE__, __LINE__, __func__);
        fprintf(stdout, "\tError message: %s.\n", strerror(errno));
        exit(-1);
    }

    if (*buffer == '\1')
    {
        ec = BIT_32;
    }
    else if (*buffer == '\2')
    {
        ec = BIT_64;
    }
    else
    {
        // I should not be here as per the spec...
    }

    free(buffer);
    return ec;
}

bitstream_object parse_elf(int fd)
{
    if (get_elf_class(fd) == BIT_32)
    {
        return parse32(fd);
    }
    else
    {
        return parse64(fd);
    }
}

bitstream_object parse64(int fd)
{
    bitstream_object bo;
    return bo;
}

bitstream_object parse32(int fd)
{
    Elf32_Ehdr header;
    Elf32_Shdr *section_table;
    bitstream_object bo;

    read_header32(fd, &header);

    section_table = malloc(header.e_shentsize * header.e_shnum);
    if (section_table == NULL)
    {
        fprintf(stdout, "[%s:%d:\t%s] Could not allocate memory for the section table.\n", __FILE__, __LINE__, __func__);
        fprintf(stdout, "\tError message: %s.\n", strerror(errno));
        exit(-1);
    }

    read_section_table32(fd, &header, section_table);

    char *sh_str = read_section32(fd, &section_table[header.e_shstrndx]);

    for (int i = 0; i < header.e_shnum; i++)
    {
        char *section_name = sh_str + section_table[i].sh_name;

        // print_section(fd, &header, &section_table[i], sh_str + section_table[i].sh_name);

        if (strncmp(section_name, AOCX_BOARD_SPEC_SECTION, strlen(AOCX_BOARD_SPEC_SECTION)) == 0)
        {
            char *buffer = (char *)malloc(128 * sizeof(char));
            parse32_metadata(fd, &header, &section_table[i], buffer);
            memcpy(bo.device_name, buffer, strlen(buffer) * sizeof(char));
            free(buffer);
        }
        else if (strncmp(section_name, AOCX_FREQ_SECTION, strlen(AOCX_FREQ_SECTION)) == 0)
        {
            parse32_clock(fd, &header, &section_table[i], &bo.kernel_frequency, &bo.system_frequency);
        }
        else
        {
            fprintf(stdout, "%s: %s-%d: Parsing for the section: %s is not yet supported.\n", __FILE__, __func__, __LINE__, section_name);
        }
    }
    free(section_table);
    free(sh_str);

    return bo;
}

void read_header32(int fd, Elf32_Ehdr *header)
{
    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset != 0)
    {
        // parser_print_to(stderr, __FILE__, __func__, __LINE__, "Could reposition the cursor.\n");
        printf("\tError message: %s.\n", strerror(errno));
        exit(-1);
    }

    int read_size = read(fd, header, sizeof(Elf32_Ehdr));

    if (read_size != sizeof(Elf32_Ehdr))
    {
        fprintf(stderr, __FILE__, __func__, __LINE__, "Could not allocate memory for the section table.\n");
        printf("\tError message: %s.\n", strerror(errno));
        exit(-1);
    }
}

void read_section_table32(int fd, Elf32_Ehdr *header, Elf32_Shdr *section_table)
{
    off_t offset = lseek(fd, header->e_shoff, SEEK_SET);
    if (offset != header->e_shoff)
    {
        // parser_print_to(stderr, __FILE__, __func__, __LINE__, "Could not reposition the cursor.\n");
        printf("\tError message: %s.\n", strerror(errno));
        exit(-1);
    }

    for (int i = 0; i < header->e_shnum; i++)
    {
        int read_size = read(fd, &section_table[i], header->e_shentsize);
        if (read_size != header->e_shentsize)
        {
            // parser_print_to(stderr, __FILE__, __func__, __LINE__, "Could not read the section table.\n");
            printf("\tError message: %s.\n", strerror(errno));
            exit(-1);
        }
    }
}

char *read_section32(int fd, Elf32_Shdr *section)
{
    // TODO: Check for errors and return values.
    char *buffer = malloc(section->sh_size);
    lseek(fd, section->sh_offset, SEEK_SET);
    read(fd, buffer, section->sh_size);

    return buffer;
}

void print_section(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, const char *section_name)
{
    lseek(fd, section->sh_offset, SEEK_SET);
    char *buffer = malloc(section->sh_size + 1);
    read(fd, buffer, section->sh_size);
    buffer[section->sh_size] = '\0';
    fprintf(stdout, "Contents of the section %s:\n%s\n", section_name, buffer);
    free(buffer);
}

void parse32_metadata(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, char *board_name)
{
    lseek(fd, section->sh_offset, SEEK_SET);
    char *buffer = malloc(section->sh_size + 1);
    read(fd, buffer, section->sh_size);
    buffer[section->sh_size] = '\0';

    xmlDoc *document;
    xmlNode *root;

    document = xmlParseDoc((const xmlChar *)buffer);
    root = xmlDocGetRootElement(document);

    char *board = (char *)xmlGetProp(root, (const xmlChar *)XML_PROP_NAME);

    memcpy(board_name, board, strlen(board) * sizeof(char));

    fprintf(stdout, "Name of the Intel FPGA is: %s.\n", board_name);

    xmlFreeDoc(document);
    free(buffer);
}

void parse32_clock(int fd, Elf32_Ehdr *header, Elf32_Shdr *section, double *kern_freq, double *sys_freq)
{
    lseek(fd, section->sh_offset, SEEK_SET);
    char *buffer = malloc(section->sh_size + 1);
    read(fd, buffer, section->sh_size);
    buffer[section->sh_size] = '\0';

    json_t *root, *quartusFitClk, *nodes, *node, *kernel_clk_freq, *node_type;
    size_t i;
    json_error_t error;

    root = json_loads(buffer, 0, &error);

    if (!root) {
        fprintf(stderr, "Could not load json. Error on line %d: %s\n", error.line, error.text);
        free(buffer);
        return;
    }

    quartusFitClk = json_object_get(root, JSON_PROP_NAME);
    if (!quartusFitClk || !json_is_object(quartusFitClk)) {
        fprintf(stderr, "Error: %s is not an object\n", JSON_PROP_NAME);
        goto cleanup;
    }

    nodes = json_object_get(quartusFitClk, "nodes");
    if (!nodes || !json_is_array(nodes)) {
        fprintf(stderr, "Error: nodes is not an array\n");
        goto cleanup;
    }

    for (i = 0; i < json_array_size(nodes); i++) {
        node = json_array_get(nodes, i);
        if (!node || !json_is_object(node)) {
            fprintf(stderr, "Error: nodes[%ld] is not an object\n", i);
            goto cleanup;
        }

        node_type = json_object_get(node, "type");
        if (!node_type || !json_is_string(node_type)) {
            fprintf(stderr, "Error: nodes[%ld].type is not a string\n", i);
            goto cleanup;
        }

        if (strcmp(json_string_value(node_type), "system") == 0) {
            kernel_clk_freq = json_object_get(node, "kernel clock");
            if (!kernel_clk_freq || !json_is_string(kernel_clk_freq)) {
                fprintf(stderr, "Error: nodes[%ld].(kernel clock) is not a string\n", i);
                goto cleanup;
            }

            *kern_freq = atof(json_string_value(kernel_clk_freq));
        }
    }

cleanup:
    json_decref(root);
    free(buffer);
}

// char * seek_to_offset_and_read()
// {
//     lseek(fd, section->sh_offset, SEEK_SET);
//     char *buffer = malloc(section->sh_size + 1);
//     read(fd, buffer, section->sh_size);
//     return buffer;
// }
