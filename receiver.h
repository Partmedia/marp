#ifndef _RECEIVER_H
#define _RECEIVER_H

void receiver_close();
float receiver_get_strength();
void receiver_open(const char *device);

#endif
