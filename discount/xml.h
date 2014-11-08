#ifndef XML_H
#define XML_H

#include <libxml/xmlstring.h>

struct post {
	struct post *next;
	xmlChar *title;
	xmlChar *summery;
} post;

void free_post(struct post *cur);
int get_title_summery(struct post **head, const char *f_doc);
int add_header(const char *f_doc, const char *f_head);

#endif
