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

/** Macro for old function call. */
#define steer_and_collect(target) steer(target, 0, true)

/**
 * Steer the antenna to the given orientation with the option of collecting
 * data. This function blocks until the antenna is facing the target.
 */
static void steer(float az_target, float el_target, bool collect) {
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
        if (fabs(azimuth - az_target) <= 1 &&
                fabs(elevation - el_target) <= 1) {
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
        steer_and_collect(config.az_max);
        steer_and_collect(config.az_min);
    } else {
        steer_and_collect(config.az_min);
        steer_and_collect(config.az_max);
    }
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
    pan_scan();
    data_dump();
}

/**
 * Run the whole suite of tests for an antenna.
 */
void tests_run() {
    bg_scan();
    ant_scan();
}
