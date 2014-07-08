/**
 * @file
 * Receiver controller using Hamlib.
 */

#include <assert.h>
#include <hamlib/rig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "receiver.h"

static RIG *rig;

/**
 * Open receiver interface. This function exits on error.
 */
void receiver_open() {
    assert(config.rig_model != 0);

    rig = rig_init(config.rig_model);
    if (rig == NULL) {
        fprintf(stderr, "Unknown rig model!\n");
        exit(EXIT_FAILURE);
    }

    if (config.rig_file != NULL) {
        strlcpy(rig->state.rigport.pathname, config.rig_file, FILPATHLEN);
    }

    if (rig_open(rig) != RIG_OK) {
        fprintf(stderr, "Could not open rig!\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Send a message from the receiver using Morse code.
 */
bool receiver_send_morse(const char *message) {
    const value_t speed = {20}, pitch = {700};
    bool retvalue;

    // Switch to VFO B and change mode to CW.
    rig_set_vfo(rig, RIG_VFO_B);
    rig_set_mode(rig, RIG_VFO_B, RIG_MODE_CW,
            rig_passband_normal(rig, RIG_MODE_CW));

    // Set CW options.
    rig_set_func(rig, RIG_VFO_B, RIG_FUNC_FBKIN, true);
    rig_set_level(rig, RIG_VFO_B, RIG_LEVEL_KEYSPD, speed);
    rig_set_level(rig, RIG_VFO_B, RIG_LEVEL_CWPITCH, pitch);

    // Send message and wait for 10 seconds.
    retvalue = rig_send_morse(rig, RIG_VFO_B, message) == RIG_OK;
    sleep(15);

    // Switch back to VFO A.
    rig_set_vfo(rig, RIG_VFO_A);
    return retvalue;
}

/**
 * Get the calibrated signal strength reported by the receiver.
 */
int receiver_get_strength() {
    static int strength;

    if (rig_get_strength(rig, RIG_VFO_A, &strength) == RIG_OK) {
        return strength;
    } else {
        return -100;
    }
}

/**
 * Close and destroy receiver interface.
 */
void receiver_close() {
    rig_close(rig);
    rig_cleanup(rig);
}
