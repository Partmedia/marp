/**
 * @file
 * Rotator controller for Hamlib.
 */

#include <assert.h>
#include <hamlib/rotator.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "rotator.h"

static ROT *rot;

/**
 * Initialize and open rotator interface. This function exits on error.
 */
void rotator_open() {
    rig_set_debug(RIG_DEBUG_WARN);
    assert(config.rot_model != 0);

    rot = rot_init(config.rot_model);
    if (rot == NULL) {
        fprintf(stderr, "Unknown rotator model!\n");
        exit(EXIT_FAILURE);
    }

    if (config.rot_file != NULL) {
        strlcpy(rot->state.rotport.pathname, config.rot_file, FILPATHLEN);
    }

    if (rot_open(rot) != RIG_OK) {
        fprintf(stderr, "Could not open rotator!\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Get the current position of the rotator interface. On failure, the output
 * variables are set to zero and thus become invalid.
 * @return True if successful, false otherwise.
 */
bool rotator_get_position(float *azimuth, float *elevation) {
    return rot_get_position(rot, azimuth, elevation) == RIG_OK;
}

/**
 * Set the current position of the rotator interface.
 * @return True if successful, false otherwise.
 */
bool rotator_set_position(float azimuth, float elevation) {
    return rot_set_position(rot, azimuth, elevation) == RIG_OK;
}

/**
 * Close and destroy rotator interface.
 */
void rotator_close() {
    rot_close(rot);
    rot_cleanup(rot);
}
