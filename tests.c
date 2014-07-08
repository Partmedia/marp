/**
 * @file
 * Test driver implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <time.h>
#include <unistd.h>

#include "data.h"
#include "main.h"
#include "receiver.h"
#include "rotator.h"
#include "source.h"
#include "tests.h"

/**
 * Steer the antenna to the given orientation with the option of collecting
 * data. This function blocks until the antenna is facing the target.
 */
static void steer(float az_target, float el_target, bool collect) {
    const int angle_threshold = 3;
    struct timespec time_start;
    int errors = 0;

    fprintf(stderr, "===> Rotating to %f, %f...\n", az_target, el_target);
    rotator_set_position(az_target, el_target);
    clock_gettime(CLOCK_MONOTONIC, &time_start);

    while (true) {
        struct timespec time_current;
        float azimuth, elevation;

        // If too much time has elapsed, time out and exit.
        clock_gettime(CLOCK_MONOTONIC, &time_current);
        if (time_current.tv_sec - time_start.tv_sec > 60 + 10) {
            fprintf(stderr, "Timeout: rotator is taking too long\n");
            data_annotate("Timeout");
            exit(EXIT_FAILURE);
        }

        // Check rotator return value.
        if (rotator_get_position(&azimuth, &elevation) == false) {
            sleep(3);
            continue;
        }

        // Break out at the correct position or when error limit exceeded.
        if (errors > 3 || (fabs(azimuth - az_target) <= angle_threshold &&
                    fabs(elevation - el_target) <= angle_threshold)) {
            break;
        }

        if (collect) {
            data_record(azimuth, elevation, receiver_get_strength());
        }
    }
}

/**
 * Record pattern for an antenna on its azimuth plane.
 */
static void scan_pan_azimuth() {
    float azimuth, elevation;
    rotator_get_position(&azimuth, &elevation);

    // Figure out which end to start the test at.
    if (fabs(config.az_max - azimuth) < fabs(config.az_min - azimuth)) {
        steer(config.az_max, 0, false);
        steer(config.az_min, 0, true);
    } else {
        steer(config.az_min, 0, false);
        steer(config.az_max, 0, true);
    }
}

/**
 * Record pattern for an antenna on its azimuth and elevation planes.
 */
static void scan_planes(int source_az, int source_el) {
    // Steer to and collect data for azimuth measurement.
    steer(config.az_min, config.el_min, false);
    steer(config.az_max, config.el_min, true);

    // Steer to and collect data for elevation measurement.
    steer(source_az, config.el_max - 90, false);
    steer(source_az, config.el_min, true);
}

/**
 * Scan data for antenna measurement.
 */
static void ant_scan(int start_az, int start_el) {
    // Rotate antenna to starting position.
    steer(start_az, start_el, false);
    data_addset("ant_scan %d,%d", start_az, start_el);

    fprintf(stderr, "===>>> Starting antenna measurement...\n");
    source_on();
    scan_planes(start_az, start_el);
    source_off();
    data_annotate("Data collection finished successfully.");
    data_dump();
    fprintf(stderr, "===>>> Data collection finished successfully.\n");
}

/**
 * Run the whole suite of tests for an antenna.
 */
void tests_run() {
    ant_scan(90, 0);
}
