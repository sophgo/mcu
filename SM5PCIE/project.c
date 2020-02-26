#include <info.h>
#include <project_id.h>
#include <assert.h>

static int project = PROJ_INVALID;

void project_init(void)
{
	project = *MCU_INFO_PROJ;

	assert(project < PROJ_INVALID);
}

int project_get(void)
{
	return project;
}

