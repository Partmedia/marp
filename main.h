/**
 * @file
 * Main program runtime information.
 */

#ifndef _MAIN_H
#define _MAIN_H

/**
 * @struct config_s
 * Container for runtime configuration.
 */
typedef struct {
    char *rot_file, *receiver_file;
    int rot_model;
    int azimuth, azimuth_sweep;
    char *load_file, *write_file;
} config_s;

/** Runtime configuration. */
extern config_s config;

#endif
