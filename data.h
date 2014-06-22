/**
 * @file
 * Data collection and processing module.
 */

#ifndef _DATA_H
#define _DATA_H

void data_dump();
void data_init();
void data_load(const FILE * const file);
void data_record(float azimuth, float elevation, float strength);

#endif
