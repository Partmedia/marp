/**
 * @file
 * Main executable.
 */

#include <hamlib/rotator.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** Runtime configuration. */
struct {
    char *rot_file;
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
        "   -s SWEEP        Set azimuth sweep (default 360).\n"
    );
}

/**
 * Parse command-line arguments and load configuration defaults.
 */
static void parse_args(int argc, char *argv[]) {
    int flag;

    while ((flag = getopt(argc, argv, "a:d:hm:s:")) != -1) {
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

    if (config.azimuth_sweep == 0) {
        config.azimuth_sweep = 360;
    }
}

/**
 * Main program entry point.
 */
int main(int argc, char *argv[]) {
    ROT *rot;

    parse_args(argc, argv);

    rot = rot_init(config.rot_model);
    if (rot == NULL) {
        puts("Unknown rotator model!");
        exit(EXIT_FAILURE);
    }

    if (config.rot_file != NULL) {
        strlcpy(rot->state.rotport.pathname, config.rot_file, FILPATHLEN);
    }

    if (rot_open(rot) != RIG_OK) {
        puts("Could not open rotator!");
        exit(EXIT_FAILURE);
    }

    rot_close(rot);
    rot_cleanup(rot);
}
