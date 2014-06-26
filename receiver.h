/**
 * @file
 * Receiver control interface.
 */

#ifndef _RECEIVER_H
#define _RECEIVER_H

void receiver_close(void);
int receiver_get_strength(int unit);
void receiver_open(const char *device);
float receiver_to_decibels(int value);

#endif
