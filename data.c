/**
 * @file
 * Antenna data processor.
 */

#include <math.h>
#include <stdio.h>

#include "data.h"

/** Data log. */
static FILE *log_file;

static float max_strength[360];

static int data_translate(int angle) {
    return 90 - angle;
}

void data_add(float azimuth, float elevation, float strength) {
    const int azimuth_rnd = (int)roundf(azimuth);
    const int elevation_rnd = (int)roundf(elevation);

    // Create a new data log if it doesn't exist.
    if (log_file == NULL) {
        log_file = fopen("data.log", "a");
        fprintf(log_file, "Starting new data log...\n");
    }

    // Write a copy of all data to the log.
    fprintf(log_file, "%f\t%f\t%f\n", azimuth, elevation, strength);
    fflush(log_file);

    // Record the maximum signal strength at a given orientation.
    if (max_strength[azimuth_rnd] < strength) {
        max_strength[azimuth_rnd] = strength;
    }
}

void data_dump(FILE *output) {
    fclose(log_file);

    for (int i = 0; i < 360; i++) {
        printf("%d\t%f\n", data_translate(i), max_strength[i]);
    }

    fflush(stdout);
}
