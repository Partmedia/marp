/**
 * @file
 * Rotator control interface.
 */

#ifndef _ROTATOR_H
#define _ROTATOR_H

#include <stdbool.h>

void rotator_close(void);
bool rotator_get_position(float *azimuth, float *elevation);
void rotator_open(void);
bool rotator_set_position(float azimuth, float elevation);

#endif
