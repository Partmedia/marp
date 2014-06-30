/**
 * @file
 * Test driver implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <unistd.h>

#include "data.h"
#include "main.h"
#include "receiver.h"
#include "rotator.h"
#include "source.h"
#include "tests.h"

/**
 * Steer the antenna to the given orientation while collecting data. This
 * function blocks until the antenna is facing the right direction.
 */
static void steer_and_collect(int target) {
    float azimuth, elevation;
    fprintf(stderr, "Rotating to %d...\n", target);
    rotator_set_position(target, 0);

    do {
        if (rotator_get_position(&azimuth, &elevation)) {
            data_record(azimuth, elevation,
                    receiver_get_strength(config.rec_unit));
        }

        usleep(1e6 / 10);
    } while (fabs(azimuth - target) > 5);
}

/**
 * Rotate antenna on its azimuth axis and record data.
 */
static void pan_scan() {
    float azimuth, elevation;
    rotator_get_position(&azimuth, &elevation);

    // Figure out which end to start the test at.
    if (abs(config.azimuth_sweep - azimuth) < abs(config.azimuth - azimuth)) {
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
    fprintf(stderr, "Starting antenna measurement...\n");
    source_on();
    data_addset("ant_scan");
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
