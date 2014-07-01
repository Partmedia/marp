/**
 * @file
 * Data implementation using flat, tab-separated plain text files
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "main.h"
#include "receiver.h"

/** Data log. */
static FILE *log_file;

/** Data format. */
static const char *format = "%f\t%f\t%d\n";

/** Current data set for holding temporary data. */
static struct {
    int max_strength[360];
    bool has_data;
} set;

/**
 * Translate a compass angle to an angle on a polar plot.
 */
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
 * when recording live data. This function exits on error.
 */
void data_init() {
    log_file = fopen(config.write_file, "wx");
    if (log_file == NULL) {
        fprintf(stderr, "Could not create data log '%s': %s\n",
                config.write_file, strerror(errno));
        exit(EXIT_FAILURE);
    }

    atexit(data_cleanup);
    fprintf(stderr, "Logging data to '%s'\n", config.write_file);

    // Initialize empty data set.
    data_dump();
}

/**
 * Add a single data point to the current data set. The caller should have
 * already performed sanity checking on the data.
 */
static void data_add(float azimuth, float elevation, int strength) {
    const int azimuth_rnd = (int)roundf(azimuth);
    const int elevation_rnd = (int)roundf(elevation);

    assert(set.has_data);

    // Ignore data points that are out of bounds.
    if (azimuth_rnd < 0 || azimuth_rnd >= 360) {
        return;
    }

    // Record the maximum signal strength at a given orientation.
    if (set.max_strength[azimuth_rnd] < strength) {
        set.max_strength[azimuth_rnd] = strength;
    }
}

/**
 * Load recorded data from a file.
 */
void data_load(FILE *file) {
    float azimuth, elevation;
    int line = 1, strength;
    char buf[64];

    while (fgets(buf, sizeof(buf), file) != NULL) {
        if (buf[0] == '#') {
            if (sscanf(buf, "# @set %s\n", buf) == 1) {
                // Dump out the previous set, start anew, and mark it dirty.
                data_dump();
                set.has_data = true;
            }
        } else {
            if (sscanf(buf, format, &azimuth, &elevation, &strength) != 3) {
                fprintf(stderr, "Data file format error on line %d\n", line);
                exit(EXIT_FAILURE);
            }

            data_add(azimuth, elevation, strength);
        }

        line++;
    }

    // Dump out whatever data was left in the last set.
    data_dump();
}

/**
 * Record future entries in a new data set with the given name. The name must
 * not contain any whitespace.
 */
void data_addset(const char *name) {
    if (set.has_data) {
        fprintf(stderr, "Data set added with old contents still there!\n");
        abort();
    }

    fprintf(log_file, "# @set %s\n", name);
    fflush(log_file);
    set.has_data = true;
}

/**
 * Log and process a data point from a live recording session.
 */
void data_record(float azimuth, float elevation, int strength) {
    // Write a copy of all data to the log.
    fprintf(log_file, format, azimuth, elevation, strength);
    fflush(log_file);

    data_add(azimuth, elevation, strength);
}

/**
 * Dump the contents of the current data set. When finished, clear the current
 * data set from memory. Always call this function before adding a new data
 * set and recording data to it.
 */
void data_dump() {
    for (int i = 0; i < 360; i++) {
        if (set.max_strength[i] != -54) {
            if (set.has_data) {
                printf("%d\t%d\n", data_translate(i), set.max_strength[i]);
            }

            set.max_strength[i] = -54;
        }
    }

    fflush(stdout);
    set.has_data = false;
}
