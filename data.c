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

/** Sentinel value for empty data. */
static const int NODATA = -55;

struct dataset_s {
    int max_strength[360], avg_strength[360], avg_count[360];
    bool elevation;
};

/** Current data set for holding temporary data. */
static struct dataset_s set;

static void data_pattern_dump(int origin_az);

/**
 * Translate a compass angle to an angle on a polar plot.
 */
static int compass_translate(int angle) {
    return 90 - angle;
}

/**
 * Zero the given data set.
 */
static void data_clear(struct dataset_s *set) {
    for (int i = 0; i < 360; i++) {
        set->max_strength[i] = NODATA;
        set->avg_strength[i] = NODATA;
        set->avg_count[i] = 0;
        set->elevation = false;
    }
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
static void data_add(float position, int strength) {
    const int pos_rnd = (int)roundf(position);

    // Ignore data points that are out of bounds.
    if (pos_rnd < 0 || pos_rnd >= 360) {
        return;
    }

    // Record the maximum signal strength at a given orientation.
    if (set.max_strength[pos_rnd] < strength) {
        set.max_strength[pos_rnd] = strength;
    }

    // Update average signal strength.
    set.avg_strength[pos_rnd] =
        (set.avg_strength[pos_rnd] * set.avg_count[pos_rnd] + strength) /
        ++set.avg_count[pos_rnd];
}

/**
 * Load recorded data from a file.
 */
void data_load(FILE *file) {
    char buf[64];
    int line = 1, origin_az = 0;

    while (fgets(buf, sizeof(buf), file) != NULL) {
        if (buf[0] == '#') {
            if (sscanf(buf, "# @%s %d,%*d\n", buf, &origin_az) >= 1) {
                if (strcmp(buf, "origin") == 0) {
                    printf("# Origin: %d\n", origin_az);
                } else if (strcmp(buf, "azimuth") == 0) {
                    printf("#### Azimuth ####\n");
                    data_clear(&set);
                    set.elevation = false;
                } else if (strcmp(buf, "elevation") == 0) {
                    data_pattern_dump(origin_az);
                    printf("#### Elevation ####\n");
                    data_clear(&set);
                    set.elevation = true;
                } else {
                    fprintf(stderr, "%d: warning: unknown command\n", line);
                }
            }
        } else {
            float azimuth, elevation;
            int strength;

            if (sscanf(buf, format, &azimuth, &elevation, &strength) != 3) {
                fprintf(stderr, "%d: error: invalid data format\n", line);
                exit(EXIT_FAILURE);
            }

            if (!set.elevation) {
                data_add(azimuth, strength);
            } else {
                data_add(elevation, strength);
            }
        }

        line++;
    }

    // Dump the data from the elevation pattern.
    data_pattern_dump(origin_az);
}

/**
 * Make an annotation in the current data log. This function should only be
 * used when recording live data.
 */
void data_annotate(const char *format, ...) {
    va_list args;

    fprintf(log_file, "# ");
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "\n");

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
 * Print the current data set in a Gnuplot-friendly format. Do not forget to
 * clear the data set before starting another one.
 */
static void data_pattern_dump(int origin_az) {
    for (int i = 0; i < 360; i++) {
        int current_str = set.max_strength[i];

        if (current_str != NODATA) {
            int display_angle;

            if (!set.elevation) {
                display_angle = compass_translate(i - origin_az);
            } else {
                display_angle = i;
            }

            printf("%d\t%d\n", display_angle, current_str);
        }
    }

    fflush(stdout);
}
