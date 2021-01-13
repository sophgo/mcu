#ifndef __MON_H__
#define __MON_H__

void mon_init(void);
void mon_process(void);
void mon_set_verbose(int verbose);
int mon_get_verbose(void);
void mon_put_text(void);

#endif
