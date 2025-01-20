#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

void kbd_set(int port, int key);
void kbd_init(struct i2c_slave_ctx *i2c);

#endif
