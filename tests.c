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
    float azimuth, elevation;

    fprintf(stderr, "Rotating to %f, %f...\n", az_target, el_target);
    rotator_set_position(az_target, el_target);

    do {
        // We need to grab rotator position anyways, so do that first.
        if (rotator_get_position(&azimuth, &elevation)) {
            if (collect) {
                data_record(azimuth, elevation, receiver_get_strength());
            }
        }
    } while (fabs(azimuth - az_target) > 1 || fabs(elevation - el_target) > 1);
}

/**
 * Rotate antenna on its azimuth axis and record data.
 */
static void pan_scan() {
    float azimuth, elevation;
    rotator_get_position(&azimuth, &elevation);

    // Figure out which end to start the test at.
    if (fabs(config.azimuth_sweep - azimuth) < fabs(config.azimuth - azimuth)) {
        steer_and_collect(config.azimuth_sweep);
        steer_and_collect(config.azimuth);
    } else {
        steer_and_collect(config.azimuth);
        steer_and_collect(config.azimuth_sweep);
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
