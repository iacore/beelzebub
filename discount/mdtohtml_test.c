#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <assert.h>
#include "md2html.h"

#include <libxml/xmlreader.h>
#include "xml.h"

const char* get_fn(const char *path);

void create_list_page(const struct post * const p, const char *out)
{
	assert(p);
	if (!p) 
		return;

	xmlDoc *doc = htmlNewDoc(NULL, NULL);

	xmlNode *doc_head = xmlNewNode(NULL, BAD_CAST "html");
	xmlDocSetRootElement(doc, doc_head);
	
	for (const struct post *lst = p; lst->next ; lst = lst->next) {
		xmlNewChild(doc_head, NULL, BAD_CAST "h1", lst->title);
		xmlNewChild(doc_head, NULL, BAD_CAST "p", lst->summery);

		xmlNode *a = xmlNewChild(doc_head, NULL, BAD_CAST "a", BAD_CAST "more...");
		xmlNewProp(a, BAD_CAST BAD_CAST "href", BAD_CAST lst->fn);
	}

	htmlSaveFile(out, doc);

}

int main()
{
	printf("hello world\n");
	//create_list_page(NULL, NULL);

//	convert_move_file("in.md", "a");
	convert_dir("", "");
#if 0
	struct post *head = NULL;
	get_title_summery(&head, "out.html");
	assert(head);
	printf("%s\n", head->title);
	printf("%s\n", head->summery);

	free_post(head);
	//add_header("out.html", "out2.html");

	//hack_root_tags("out.html");
	//convert_file("in.md", "out.html");
#endif

}


