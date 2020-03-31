#include <log.h>

unsigned log_output_level = LEVEL_INFO;

void set_log_level(int level)
{
	log_output_level = level;
}
