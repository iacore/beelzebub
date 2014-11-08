#include "md2html.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include "mdtohtml_test.h"

typedef unsigned DWORD;
#include "discount/markdown.h"
int markdown(Document*, FILE*, int);

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "xml.h"
int open_files(FILE **fin, FILE **fout, const char *pin, const char *pout)
{
	*fin = fopen(pin, "r");
	
	if (!(*fin)) { 
		printf("Error opening file <%s>\n%s\n", pin, strerror(errno)); 
		return 1; 
	}

	*fout = fopen(pout, "aw");

	if (!(*fout)) { 
		printf("Error opening file <%s>\n%s\n", pout, strerror(errno)); 
		return 1; 
	}

	return 0;
}


/* Given a filenme, surround the original contents with <html> and </html>. */
static int hack_root_tags(const char *path_in)
{
	FILE *fin, *fout;

	if (open_files(&fin, &fout, path_in, "tmp"))
		return 1;

	fputs("<html>", fout);

	char buf[201];

	while (fgets(buf, 200, fin))
		fputs(buf, fout);

	fclose(fin);
	fputs("</html>", fout);
	fclose(fout);

	puts("Done...");

	if (rename("tmp", path_in)) {
		printf("Could not move <%s> to <%s>\n%s\n", 
			path_in, "tmp", strerror(errno));
		return 1;
	}

	return 0;
}

/* Convert a given markdown file into a HTML file. */
int convert_file(const char *path_in, const char *path_out)
{
	FILE *fin = fopen(path_in, "r");
	if (!fin) { 
		printf("Error opening file <%s>\n%s\n", path_in, strerror(errno)); 
		return 1; 
	}

	FILE *fout = fopen(path_out, "aw");
	if (!fout) { 
		printf("Error opening file <%s>\n%s\n", path_out, strerror(errno)); 
		return 1; 
	}

	Document *doc = mkd_in(fin, 0);
	if (!doc) {
		printf("Unknown libmarkdown error calling mkd_in().");
		return 1;
	}

	int res = 0;
	if (markdown(doc, fout, 0)) {
		printf("Unknown libmarkdown error calling markdown().");
		res = 1;
	}

	fclose(fin);
	fclose(fout);
	mkd_cleanup(doc);
	return res;
}

/* Given a filename and a path, return a path which consists of the input path, the 
 * file's basename, and the file's extension replaced with .html. 
 *
 * Returned string must be freed by caller.
 *
 * i.e.
 * "in.md", "apple" --> "apple/in.html"
 */
char* create_path(const char *file_in, const char *path_out)
{
	const char ext[] = ".html";
	const unsigned long ext_ln = sizeof ext;

	char *file_out = NULL;

	const char *fn = strrchr(file_in, '/');
	if (!fn)
		fn = file_in;

	const char *dot = strrchr(file_in, '.');

	unsigned long fn_ln;
	if (dot < fn) {
		fn_ln = strlen(fn);
	} else {
		fn_ln = dot - fn;
	}

	unsigned long out_ln = strlen(path_out);

	if (out_ln > 0) {
		file_out = malloc(fn_ln + ext_ln + out_ln);
		strncpy(file_out, path_out, out_ln);
		file_out[out_ln] = '/';
		++out_ln;
	}

	strncpy(file_out+out_ln, fn, fn_ln);
	strncpy(file_out+out_ln+fn_ln, ext, ext_ln);
	assert(fn_ln+ext_ln+out_ln == strlen(file_out) + 1);

	return file_out;
}

/* Make directory, return success if it already exists. */
int xmkdir(const char *path)
{
	int res = mkdir(path, 0777);

	if (!res || res == EEXIST)
		return 0;
	
	//return res;
	/* Whatever. */
	return 0;
}

/* TODO Merge this with create_path... */
char* mkpath(const char *path, const char *fn)
{
	const unsigned long p_len = strlen(path);
	const unsigned long f_len = strlen(fn);

	/* + 2 = '/' and '\0'. */
	const unsigned long t_len = p_len + f_len + 2;
	char *ret = malloc(t_len * sizeof *ret);
	char *cur = ret;

	strncpy(cur, path, p_len);
	cur += p_len;

	*cur = '/';
	++cur;

	strncpy(cur, fn, f_len);
	cur += f_len;

	*cur = '\0';
	assert(strlen(ret) + 1==  t_len);
	return ret;
}


/* Convert a given markdown file into a HTML file of the same name but in the given 
 * directory path_out. */
int convert_move_file(const char *file_in, const char *path_out)
{
	const char *file_tmp = "tmp";

	if (path_out && *path_out != '\0') {
		if (xmkdir(path_out)) {
			printf("Could not create directory <%s>\n", path_out);
			return 1;
		}
	}

	if (convert_file(file_in, file_tmp)) {
		printf("Could not convert file <%s>\n", file_in);
		return 1;
	}

	char *file_out = create_path(file_in, path_out);
	if (rename(file_tmp, file_out)) {
		printf("Could not move <%s> to <%s>\n%s\n", 
			file_tmp, file_out, strerror(errno));
		free(file_out);
		return 1;
	}

	free(file_out);
	return 0;
}

/* Call a given function on each file name in a given directory. */
void map_dir(int (*cb)(const char*, const char*), const char *cb_data, const char *dir)
{
	DIR *dfd = opendir(dir);
	struct dirent *dp;
	struct post *p = NULL;

	 while ((dp = readdir(dfd))) {
	 	if (dp->d_type == DT_REG) {
			char *in = mkpath(dir, dp->d_name);
			char *out = create_path(dp->d_name, cb_data);
			convert_move_file(in, cb_data);
			hack_root_tags(out);
			add_header(out, "header.html");

			
			(void)get_title_summery(&p, out);
			free(in);
			free(out);
			
		}
	 }

	char *list_out = create_path("list", cb_data);
	create_list_page(p, list_out);
	add_header(list_out, "header.html");
	free(list_out);

	//for (struct post *lst = p; lst->next ; lst = lst->next) {
		//printf("title = %s\nsummery = %s\n", lst->title, lst->summery);
	//}
}

int convert_dir(const char *dirin, const char *dirout)
{
	map_dir(&convert_move_file, "out",  "in");
	return 0;

}
