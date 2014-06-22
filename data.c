/**
 * @file
 * Antenna data processor.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"

/** Data log. */
static FILE *log_file;

static float max_strength[360];

static int data_translate(int angle) {
    return 90 - angle;
}

/**
 * Clean up data collection module. Used only internally on exit.
 */
static void data_cleanup() {
    fclose(log_file);
}

/**
 * Initialize data collection and processing module. Use this function only
 * when recording live data.
 */
void data_init() {
    log_file = fopen("data.log", "a");
    fprintf(log_file, "Starting new data log...\n");
    atexit(data_cleanup);
}

static void data_add(float azimuth, float elevation, float strength) {
    const int azimuth_rnd = (int)roundf(azimuth);
    const int elevation_rnd = (int)roundf(elevation);

    // Record the maximum signal strength at a given orientation.
    if (max_strength[azimuth_rnd] < strength) {
        max_strength[azimuth_rnd] = strength;
    }
}

/**
 * Load recorded data from a file.
 */
void data_load(const FILE * const file) {
}

/**
 * Log and process a data point from a live recording session.
 */
void data_record(float azimuth, float elevation, float strength) {
    // Write a copy of all data to the log.
    fprintf(log_file, "%f\t%f\t%f\n", azimuth, elevation, strength);
    fflush(log_file);

    data_add(azimuth, elevation, strength);
}

void data_dump() {
    for (int i = 0; i < 360; i++) {
        printf("%d\t%f\n", data_translate(i), max_strength[i]);
    }

    fflush(stdout);
}
