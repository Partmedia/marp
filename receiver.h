/**
 * @file
 * Receiver control interface.
 */

#ifndef _RECEIVER_H
#define _RECEIVER_H

#include <stdbool.h>

void receiver_close(void);
int receiver_get_strength(void);
void receiver_open(void);
bool receiver_send_morse(const char *message);

#endif
