#ifndef __MON_H__
#define __MON_H__

void mon_init(void);
void mon_process(void);
void mon_set_mode(int mode);
int mon_get_mode(void);
void mon_put_text(void);
void set_soc_temp(int idx, int temp);
void set_board_temp(int idx, int temp);

enum MON_MODE {
	MON_MODE_NORMAL = 0,
	MON_MODE_CLEAR,
	MON_MODE_VERBOSE,
};

#endif
