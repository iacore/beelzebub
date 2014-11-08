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

const char* get_fn(const char *path);
/* Strip the first line from a file. A hacky way to get rid of the <?xml...?> bullshit
 * that libxml adds to the top of files.*/
/* TODO Uneeded? */
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
	/* These fuctions are htmlParse... in order to correctly handle encoding
	 * and HTML entities. */
	xmlDocPtr doc = htmlParseFile(f_doc, NULL);
	if (!doc) {
		printf("libxml failed to parse file <%s>\n", f_doc);
		return 1;
	}

	xmlDocPtr head = htmlParseFile(f_head, NULL);
	if (!head) {
		printf("libxml failed to parse file <%s>\n", f_head);
		return 1;
	}

	xmlNode *doc_head = xmlDocGetRootElement(doc);
	xmlNode *head_head = xmlDocGetRootElement(head);
	(void)xmlAddChild(head_head, doc_head->children);
	htmlSaveFile(f_doc, head); 
//	strip_first(f_doc);

	xmlFreeDoc(head);
	xmlFree(doc_head);
	return 0;
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
	xmlChar *xml_xpath = xmlCharStrdup(xpath);
	xmlXPathObject *xpath_obj = xmlXPathEvalExpression(xml_xpath, xpath_ctx);
	if (!xpath_obj) {
		puts("Could not evaluate xpath expression.\n");
		free(xml_xpath);
		return NULL;
	}

	ret = get_text(xpath_obj->nodesetval->nodeTab[0]);
	xmlXPathFreeObject(xpath_obj);
	free(xml_xpath);
	return ret;
}

struct post {
	struct post *next;
	xmlChar *title;
	xmlChar *summery;
	char *fn;
} post;

/* Given a path to a HTML document parse the title & summery into a given post 
 * linked list. */
int get_title_summery(struct post **head, const char *f_doc)
{
	xmlDocPtr doc = htmlParseFile(f_doc, NULL);
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

	lst->fn = strdup(get_fn(f_doc));
	return 0;
}

/* Recursivly free a post linked list. */
void free_post(struct post *cur)
{
	if (!cur)
		return;

	struct post *next = cur->next;
	xmlFree(cur->title);
	xmlFree(cur->summery);
	free(cur->fn);
	free(cur);

	free_post(next);
}

void create_list_page(const struct post * const p, const char *out)
{
	assert(p);
	if (!p) 
		return;

	xmlDoc *doc = htmlNewDoc(NULL, NULL);

	xmlNode *doc_head = xmlNewNode(NULL, "html");
	xmlDocSetRootElement(doc, doc_head);
	
	for (const struct post *lst = p; lst->next ; lst = lst->next) {
		xmlNewChild(doc_head, NULL, "h1", lst->title);
		xmlNewChild(doc_head, NULL, "p", lst->summery);

		xmlNode *a = xmlNewChild(doc_head, NULL, "a", "more...");
		xmlNewProp(a, BAD_CAST "href", BAD_CAST lst->fn);
	}

	htmlSaveFile(out, doc);

}

/* Return a filename from a path (pointer into input string is returnred. */
const char* get_fn(const char *path)
{
	const char *fn = strrchr(path, '/');
	if (!fn)
		return path;

	++fn;

	/* No forward slahes at the end of the string please. */
	assert(*fn);
	return fn;
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


