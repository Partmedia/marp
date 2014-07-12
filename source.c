/**
 * @file
 * Source antenna control using CW
 */

#include <stdio.h>

#include "receiver.h"
#include "source.h"

/**
 * Ask human operator to turn source on.
 */
void source_on() {
    receiver_send_morse("TEST SRC ON");
}

/**
 * Ask human operator to turn source off.
 */
void source_off() {
    receiver_send_morse("CL");
}
