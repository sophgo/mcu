#ifndef __LOOP_H__
#define __LOOP_H__

#define MAIN_LOOP_ENTRY_MAX	16
typedef void (*loop_entry)(void);

void loop_add(loop_entry entry);
void loop_start(void);

#endif
