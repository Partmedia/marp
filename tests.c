/**
 * @file
 * Test driver implementation.
 */

#include <stdio.h>
#include <tgmath.h>
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

    fprintf(stderr, "Rotating to %f, %f...\n", az_target, el_target);
    rotator_set_position(az_target, el_target);

    while (true) {
        float azimuth, elevation;

        // Check rotator return value.
        if (rotator_get_position(&azimuth, &elevation) == false) {
            sleep(3);
            continue;
        }

        // Check if we've reached our target.
        if (fabs(azimuth - az_target) <= angle_threshold &&
                fabs(elevation - el_target) <= angle_threshold) {
            break;
        }

        if (collect) {
            data_record(azimuth, elevation, receiver_get_strength());
        }
    }
}

/**
 * Rotate antenna on its azimuth axis and record data.
 */
static void pan_scan() {
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
    steer(source_az, config.el_max, false);
    steer(source_az, config.el_min, true);
}

/**
 * Scan background noise.
 */
static void bg_scan() {
    fprintf(stderr, "Starting background noise scan...\n");
    source_off();
    data_addset("bg_scan");
    pan_scan();
    data_dump();
}

/**
 * Scan data for antenna measurement.
 */
static void ant_scan() {
    float azimuth, elevation;

    fprintf(stderr, "Starting antenna measurement...\n");
    fprintf(stderr, "Please manually point AUT directly at source.\n");
    source_on();
    rotator_get_position(&azimuth, &elevation);
    data_addset("ant_scan %f,%f", azimuth, elevation);
    scan_planes(azimuth, elevation);
    data_dump();
}

/**
 * Run the whole suite of tests for an antenna.
 */
void tests_run() {
    bg_scan();
    ant_scan();
}
