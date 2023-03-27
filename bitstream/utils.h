#ifndef BITSTREAM__UTILS_H
#define BITSTREAM__UTILS_H

#include <stdio.h>
#include <stdlib.h>


static void parser_print_to(FILE *file, const char* file_name, const char *function_name, int line_number, const char *message)
{
    fprintf(file, "%s: %s: %d: \t%s", file_name, function_name, line_number, message);
}

#endif // BITSTREAM__UTILS_H
