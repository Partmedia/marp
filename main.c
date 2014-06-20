/**
 * @file
 * Main executable.
 */

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "data.h"
#include "receiver.h"
#include "rotator.h"

/** Runtime configuration. */
static struct {
    char *rot_file, *receiver_file;
    int rot_model;
    int azimuth, azimuth_sweep;
} config;

/**
 * Print a message stating how to get help.
 */
static void print_quickhelp() {
    fprintf(stderr, "Type 'marp -h' for usage.\n");
}

/**
 * Print usage information.
 */
static void print_usage() {
    printf(
        "Usage: marp [options]\n"
        "\n"
        "Options:\n"
        "   -a AZIMUTH      Set starting azimuth (default 0).\n"
        "   -d DEVICE       Set antenna rotator device.\n"
        "   -h              Display this help message.\n"
        "   -m ID           Set antenna rotator model.\n"
        "   -r DEVICE       Set receiver device.\n"
        "   -s SWEEP        Set azimuth sweep (default 360).\n"
    );
}

/**
 * Parse command-line arguments and load configuration defaults.
 */
static void parse_args(int argc, char *argv[]) {
    int flag;

    while ((flag = getopt(argc, argv, "a:d:hm:r:s:")) != -1) {
        switch (flag) {
            case 'a':
                config.azimuth = atoi(optarg);
                break;
            case 'd':
                config.rot_file = optarg;
                break;
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
                break;
            case 'm':
                config.rot_model = atoi(optarg);
                break;
            case 'r':
                config.receiver_file = optarg;
                break;
            case 's':
                config.azimuth_sweep = atoi(optarg);
                break;
            case '?':
                print_quickhelp();
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Load defaults for settings that are not specified.
    if (config.rot_model == 0) {
        puts("Please select a rotator model.");
        print_quickhelp();
        exit(EXIT_FAILURE);
    }

    if (config.rot_file == NULL) {
        config.rot_file = "/dev/ttyU0";
    }

    if (config.receiver_file == NULL) {
        config.receiver_file = "/dev/ttyU1";
    }

    if (config.azimuth_sweep == 0) {
        config.azimuth_sweep = 360;
    }
}

/**
 * Clean up before exiting.
 */
void cleanup() {
    fprintf(stderr, "Cleaning up...\n");
    rotator_close();
    receiver_close();
    data_dump(stdout);
}

/**
 * Main program entry point.
 */
int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    rotator_open(config.rot_model, config.rot_file);
    receiver_open(config.receiver_file);
    atexit(cleanup);
    signal(SIGINT, exit);

    while (true) {
        float azimuth, elevation;
        rotator_get_position(&azimuth, &elevation);
        data_add(azimuth, elevation, receiver_get_strength(0));
        usleep(1e6 / 10);
    }
}
