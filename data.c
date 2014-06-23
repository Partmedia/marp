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

/** Data format. */
static const char *format = "%f\t%f\t%f\n";

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
    log_file = fopen("data.log", "wx");
    if (log_file == NULL) {
        perror("Could not open log file");
        exit(EXIT_FAILURE);
    }

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
void data_load(FILE *file) {
    float azimuth, elevation, strength;
    int retcode, line = 1;

    while ((retcode = fscanf(file, format, &azimuth, &elevation, &strength))
            != EOF) {
        if (retcode != 3) {
            fprintf(stderr, "Data file format error on line %d\n", line);
            exit(EXIT_FAILURE);
        }

        data_add(azimuth, elevation, strength);
        line++;
    }
}

/**
 * Log and process a data point from a live recording session.
 */
void data_record(float azimuth, float elevation, float strength) {
    // Write a copy of all data to the log.
    fprintf(log_file, format, azimuth, elevation, strength);
    fflush(log_file);

    data_add(azimuth, elevation, strength);
}

void data_dump() {
    for (int i = 0; i < 360; i++) {
        if (max_strength[i] != 0) {
            printf("%d\t%f\n", data_translate(i), max_strength[i]);
        }
    }

    fflush(stdout);
}
