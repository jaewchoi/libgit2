#include "clar_libgit2.h"
#include "diff_helpers.h"

static git_repository *g_repo = NULL;

void test_diff_patch__initialize(void)
{
	g_repo = cl_git_sandbox_init("status");
}

void test_diff_patch__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

#define EXPECTED_OUTPUT "diff --git a/subdir.txt b/subdir.txt\n" \
	"deleted file mode 100644\n" \
	"index e8ee89e..0000000\n" \
	"--- a/subdir.txt\n" \
	"+++ /dev/null\n"

static int check_removal_cb(
	void *cb_data,
	char line_origin,
	const char *formatted_output)
{
	GIT_UNUSED(cb_data);

	if (line_origin != 'F')
		return 0;

	if (strcmp(EXPECTED_OUTPUT, formatted_output) == 0)
		return 0;

	return -1;
}

void test_diff_patch__can_properly_display_the_removal_of_a_file(void)
{
	/*
	* $ git diff 26a125e..735b6a2
	* diff --git a/subdir.txt b/subdir.txt
	* deleted file mode 100644
	* index e8ee89e..0000000
	* --- a/subdir.txt
	* +++ /dev/null
	* @@ -1,2 +0,0 @@
	* -Is it a bird?
	* -Is it a plane?
	*/

	const char *one_sha = "26a125e";
	const char *another_sha = "735b6a2";
	git_tree *one, *another;
	git_diff_list *diff;

	one = resolve_commit_oid_to_tree(g_repo, one_sha);
	another = resolve_commit_oid_to_tree(g_repo, another_sha);

	cl_git_pass(git_diff_tree_to_tree(g_repo, NULL, one, another, &diff));

	cl_git_pass(git_diff_print_patch(diff, NULL, check_removal_cb));

	git_diff_list_free(diff);

	git_tree_free(another);
	git_tree_free(one);
}
