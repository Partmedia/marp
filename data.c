/**
 * @file
 * Data implementation using flat, tab-separated plain text files
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
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

static void data_pattern_dump(int origin_az);

/**
 * Translate a compass angle to an angle on a polar plot.
 */
static int compass_translate(int angle) {
    return 90 - angle;
}

/**
 * Clean up data collection module. Used only internally on exit.
 */
static void data_cleanup() {
    fclose(log_file);
}

/**
 * Initialize data collection module. Use this function only when recording
 * live data. This function exits on error.
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
    char buf[64];
    int line = 1, origin_az = 0;

    while (fgets(buf, sizeof(buf), file) != NULL) {
        float azimuth, elevation;
        int strength;

        if (buf[0] == '#') {
            if (sscanf(buf, "# @set %s %d\n", buf, &origin_az) >= 1) {
                // Dump out the previous set, start anew, and mark it dirty.
                data_pattern_dump(0);
                printf("# Data Set: %s\tOrigin: %d\n", buf, origin_az);
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
    data_pattern_dump(origin_az);
}

/**
 * Record future entries in a new data set with the given name. The name must
 * not contain any whitespace.
 */
void data_addset(const char *format, ...) {
    va_list args;

    if (set.has_data) {
        fprintf(stderr, "Data set added with old contents still there!\n");
        abort();
    }

    fprintf(log_file, "# @set ");
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "\n");

    fflush(log_file);
    set.has_data = true;
}

/**
 * Make an annotation in the current data log. This function should only be
 * used when recording live data.
 */
void data_annotate(const char *message) {
    fprintf(log_file, "# %s\n", message);
    fflush(log_file);
}

/**
 * Log and process a data point from a live recording session.
 */
void data_record(float azimuth, float elevation, int strength) {
    // Write a copy of all data to the log.
    fprintf(log_file, format, azimuth, elevation, strength);
    fflush(log_file);
}

/**
 * Dump the contents of the current data set. When finished, clear the current
 * data set from memory. Always call this function before adding a new data
 * set and recording data to it.
 */
static void data_pattern_dump(int origin_az) {
    for (int i = 0; i < 360; i++) {
        if (set.max_strength[i] != -54) {
            if (set.has_data) {
                printf("%d\t%d\n", compass_translate(i - origin_az),
                        set.max_strength[i]);
            }

            set.max_strength[i] = -54;
        }
    }

    fflush(stdout);
    set.has_data = false;
}
