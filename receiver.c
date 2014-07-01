/**
 * @file
 * Receiver controller using Hamlib.
 */

#include <assert.h>
#include <hamlib/rig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "receiver.h"

static RIG *rig;

/**
 * Open receiver interface. This function exits on error.
 */
void receiver_open() {
    assert(config.rig_model != 0 && config.rig_file != NULL);

    rig = rig_init(config.rig_model);
    if (rig == NULL) {
        fprintf(stderr, "Unknown rig model!\n");
        exit(EXIT_FAILURE);
    }

    strlcpy(rig->state.rigport.pathname, config.rig_file, FILPATHLEN);

    if (rig_open(rig) != RIG_OK) {
        fprintf(stderr, "Could not open rig!\n");
        exit(EXIT_FAILURE);
    }
}

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
