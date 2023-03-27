#include "axlf_parser.h"

#include <iostream>
#include <experimental/xrt_xclbin.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <libxml/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

bitstream_object parse_axlf(int fd, const char *filepath)
{
	bitstream_object bo;

       	xrt::xclbin xclbinfile{filepath};
	const axlf *axlf_ptr = xclbinfile.get_axlf();
	for (int i=0; i<axlf_ptr->m_header.m_numSections; i++)
	{
		if (axlf_ptr->m_sections[i].m_sectionKind == CLOCK_FREQ_TOPOLOGY)
		{	
			const clock_freq_topology *clk_freq_topo = xclbinfile.get_axlf_section<const clock_freq_topology *>(CLOCK_FREQ_TOPOLOGY);
			for (int i=0; i<clk_freq_topo->m_count; i++)
			{
				if (clk_freq_topo->m_clock_freq[i].m_type == CLOCK_TYPE::CT_KERNEL)
				{
					bo.kernel_frequency = clk_freq_topo->m_clock_freq[i].m_freq_Mhz;
				}
				else if (clk_freq_topo->m_clock_freq[i].m_type == CLOCK_TYPE::CT_SYSTEM)
				{
					bo.system_frequency = clk_freq_topo->m_clock_freq[i].m_freq_Mhz;
				}
			}
		}
		else if (axlf_ptr->m_sections[i].m_sectionKind == EMBEDDED_METADATA)
		{
			std::unique_ptr<char []> xml_buffer(new char[axlf_ptr->m_sections[i].m_sectionSize + 1]);
			lseek(fd, axlf_ptr->m_sections[i].m_sectionOffset, SEEK_SET);
			read(fd, xml_buffer.get(), axlf_ptr->m_sections[i].m_sectionSize);
			xml_buffer.get()[axlf_ptr->m_sections[i].m_sectionSize + 1] = '\0';

			xmlDoc *document;
			xmlNode *root;

			document = xmlParseDoc((const xmlChar *)xml_buffer.get());
			root = xmlDocGetRootElement(document);

			std::function<xmlNode *(xmlNode *)> traverse_and_print = [&traverse_and_print](xmlNode *current_node){
				xmlNode *ret_val = NULL;
				for (xmlNode *node=current_node; node != NULL; node=node->next)
				{					
					if (node->type == XML_ELEMENT_NODE && (xmlStrcmp(node->name, (const xmlChar *)"platform") == 0))
					{
						return node;
					}
					ret_val = traverse_and_print(node->children);
				}
				return ret_val;
			};

			xmlNode *current_node = traverse_and_print(root);

			char *board_name = (char *)xmlGetProp(current_node, (const xmlChar *)"boardid");

			std::cout << "Board id: " << board_name;

			memcpy(bo.device_name, board_name, strlen(board_name) * sizeof(char));

			xmlFreeDoc(document);
		}
	}
	return bo;
}

#ifdef __cplusplus
}
#endif

