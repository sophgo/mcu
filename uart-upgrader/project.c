#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <project.h>

extern struct project __ld_project_table_start[0];
extern struct project __ld_project_table_end[0];

volatile uint32_t _project_id;
static struct project *project;

#define for_each_project(proj)			\
	for (proj = __ld_project_table_start;	\
	     proj != __ld_project_table_end;	\
	     ++proj)

uint32_t project_id(void)
{
	return _project_id;
}

char *project_name(void)
{
	return project->name;
}

void project_list(void)
{
	struct project *proj;

	for_each_project(proj) {
		printf("project id %lu, name %s\n", proj->id, proj->name);
	}
}

int project_init(void)
{
	struct project *proj;

	for_each_project(proj) {
		if (proj->id == _project_id) {
			project = proj;
			if (proj->init)
				return proj->init(proj);
			else
				return 0;
		}
	}

	/* project not implemented */
	while (true)
		;

	return -1;
}
