/**
 * @file
 * Rotator controller for Hamlib.
 */

#include <hamlib/rotator.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rotator.h"

static ROT *rot;

/**
 * Initialize and open rotator interface. This function exits on error.
 */
void rotator_open(int rot_model, const char *rot_file) {
    rig_set_debug(RIG_DEBUG_WARN);

    rot = rot_init(rot_model);
    if (rot == NULL) {
        puts("Unknown rotator model!");
        exit(EXIT_FAILURE);
    }

    if (rot_file != NULL) {
        strlcpy(rot->state.rotport.pathname, rot_file, FILPATHLEN);
    }

    if (rot_open(rot) != RIG_OK) {
        puts("Could not open rotator!");
        exit(EXIT_FAILURE);
    }
}

/**
 * Get the current position of the rotator interface.
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
