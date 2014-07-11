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
    char *rot_file, *rig_file;
    int rot_model, rig_model;
    int az_min, az_max, az_origin, el_min, el_max;
    char *load_file, *write_file;
} config_s;

/** Runtime configuration. */
extern config_s config;

#endif
