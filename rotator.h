/**
 * @file
 * Rotator control interface.
 */

#ifndef _ROTATOR_H
#define _ROTATOR_H

void rotator_close(void);
bool rotator_get_position(float *azimuth, float *elevation);
void rotator_open(int rot_model, const char *rot_file);
bool rotator_set_position(float azimuth, float elevation);

#endif
