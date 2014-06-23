/**
 * @file
 * Receiver control interface.
 */

#ifndef _RECEIVER_H
#define _RECEIVER_H

void receiver_close(void);
float receiver_get_strength(int unit);
void receiver_open(const char *device);

#endif
