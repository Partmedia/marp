/**
 * @file
 * Manual source antenna controller.
 */

#include <stdio.h>

#include "source.h"

/**
 * Ask human operator to turn source on.
 */
void source_on() {
    fprintf(stderr, "Please turn source ON and hit ENTER...\n");
    fgetc(stdin);
}

/**
 * Ask human operator to turn source off.
 */
void source_off() {
    fprintf(stderr, "Please turn source OFF and hit ENTER...\n");
    fgetc(stdin);
}
