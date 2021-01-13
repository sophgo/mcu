#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <stdint.h>
#include <project_id.h>

struct project {
	uint32_t id;
	char *name;
	void *priv;
	int (*init)(struct project *proj);
};

#define PROJECT(_id, _name, _priv, _init)		\
	static struct project				\
	__attribute__((section(".project")))		\
	__attribute__((used))				\
	project_entry = {				\
		.id = _id,				\
		.name = _name,				\
		.priv = _priv,				\
		.init = _init,				\
	}

void project_list(void);
int project_init(void);
char *project_name(void);

#endif
