#include "xml.h"
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlreader.h>

/* Strip the first line from a file. A hacky way to get rid of the <?xml...?> bullshit
 * that libxml adds to the top of files.*/
int strip_first(const char *path_in)
{
	FILE *fin, *fout;

	if (open_files(&fin, &fout, path_in, "tmp"))
		return 1;

	char buf[1024];

	/* Discard first line. */
	(void)fgets(buf, 1024, fin);

	while (fgets(buf, 1024, fin))
		fputs(buf, fout);

	fclose(fin);
	fclose(fout);

	if (rename("tmp", path_in)) {
		printf("Could not move <%s> to <%s>\n%s\n", 
			path_in, "tmp", strerror(errno));
		return 1;
	}

	return 0;
}

/* Given the file path to a document and a header, append the document contents 
 * (minus the root node) to the end of the header (before the close root node)
 * and overwrite the oringial document file with this new document. */
int add_header(const char *f_doc, const char *f_head)
{
	xmlDocPtr doc = xmlParseFile(f_doc);
	if (!doc) {
		printf("libxml failed to parse file <%s>\n", f_doc);
		return 1;
	}

	xmlDocPtr head = xmlParseFile(f_head);
	if (!head) {
		printf("libxml failed to parse file <%s>\n", f_head);
		return 1;
	}

	xmlNode *doc_head = xmlDocGetRootElement(doc);
	xmlNode *head_head = xmlDocGetRootElement(head);
	(void)xmlAddChild(head_head, doc_head->children);
	xmlSaveFile(f_doc, head); 
	strip_first(f_doc);

	xmlFreeDoc(doc);
	xmlFreeDoc(head);
}

void print_element_names(xmlNode *a_node)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node type: Element, name: %s\n", cur_node->name);
		}

		print_element_names(cur_node->children);
	}
}

/* Recurse through a node's children until text is found.
 * Returns NULL or an xmlChar* containing text (must be freed with xmlFree(). */
xmlChar* get_text(xmlNode *cur)
{
	for (xmlNode *cur_node = cur; ; cur_node = cur_node->children) {

		if (!cur_node)
			return NULL;
		else if (cur_node->type == XML_TEXT_NODE)
			return xmlNodeGetContent(cur_node);
	}
}

xmlChar* get_node_text(const char *xpath, xmlXPathContext *xpath_ctx)
{
	xmlChar *ret;
	xmlXPathObject *xpath_obj = xmlXPathEvalExpression(xpath, xpath_ctx);
	if (!xpath_obj) {
		puts("Could not evaluate xpath expression.\n");
		return NULL;
	}

	ret = get_text(xpath_obj->nodesetval->nodeTab[0]);
	xmlXPathFreeObject(xpath_obj);
	return ret;
}

/* Given a path to a HTML document parse the title & summery into a given post 
 * linked list. */
int get_title_summery(struct post **head, const char *f_doc)
{
	xmlDocPtr doc = xmlParseFile(f_doc);
	if (!doc) {
		printf("Could not parse document <%s>\n", f_doc);
		return 1;
	}

	xmlXPathContext *xpath_ctx = xmlXPathNewContext(doc);
	if (!xpath_ctx) {
		printf("Could not create new xpath context.\n");
		xmlFreeDoc(doc);
		return 1;
	}

	const char *xpath_title = "(//h1)[1]";
	const char *xpath_summery = "(//p)[1]";

	assert(head);
	struct post *lst = *head;

	if (!lst) {
		lst = malloc(sizeof *lst);
		*head = lst;
	} else {
		for (; lst->next ; lst = lst->next);
		struct post *newlst = malloc(sizeof *newlst);
		lst->next = newlst;
		lst = newlst;
	}

	lst->next = NULL;
	lst->title = get_node_text(xpath_title, xpath_ctx);
	lst->summery = get_node_text(xpath_summery, xpath_ctx);

	xmlXPathFreeContext(xpath_ctx); 
	xmlFreeDoc(doc);
}

/* Recursivly free a post linked list. */
void free_post(struct post *cur)
{
	if (!cur)
		return;

	struct post *next = cur->next;
	xmlFree(cur->title);
	xmlFree(cur->summery);
	free(cur);

	free_post(next);
}
