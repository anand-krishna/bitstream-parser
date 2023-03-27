CC = gcc
CXX = g++

CFLAGS = -Wall # -Werror
CXXFLAGS = -g -std=c++17

all: bitstream-parser-cli

bitstream-parser-cli: parser-lib
	$(CC) $(CFLAGS) -L. -o bitstream-parser parser_cli.c -lparser -lxml2 -ljansson

parser-lib: parser
	ar -rcs libparser.a parser.o elf_parser.o axlf_parser.o

parser: elf-parser axlf-parser
	$(CC) $(CFLAGS) -c -o parser.o bitstream/parser.c

elf-parser:
	$(CC) $(CFLAGS) -I/usr/include/libxml2 -c -o elf_parser.o bitstream/elf/elf_parser.c
	
# `xml2-config --cflags` `xml2-config --libs`

axlf-parser:
	$(CXX) $(CXXFLAGS) -c -o axlf_parser.o bitstream/axlf/axlf_parser.cpp
# $(CXX) $(CXXFLAGS) -I$XILINX_XRT/include -L$XILINX_XRT/lib -o axlf_parser.o bitstream/elf/axlf_parser.cpp -lxrt_coreutil -pthread

clean:
	rm -rf *.o *.a bitstream-parser