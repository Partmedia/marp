/**
 * @file
 * Data collection, processing, and storage interface
 */

#ifndef _DATA_H
#define _DATA_H

#include <stdio.h>

void data_addset(const char *format, ...);
void data_annotate(const char *message);
void data_init(void);
void data_load(FILE *file);
void data_record(float azimuth, float elevation, int strength);

#endif
