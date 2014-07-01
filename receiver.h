/**
 * @file
 * Receiver control interface.
 */

#ifndef _RECEIVER_H
#define _RECEIVER_H

void receiver_close(void);
int receiver_get_strength(void);
void receiver_open(void);

#endif
