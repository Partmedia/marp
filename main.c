/**
 * @file
 * Main executable.
 */

#include "config.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_CAPSICUM
#include <sys/capability.h>
#endif

#include "data.h"
#include "main.h"
#include "receiver.h"
#include "rotator.h"
#include "tests.h"

config_s config;

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
        "Usage: marp -m ID [options]\n"
        "       marp -l FILE [options]\n"
        "\n"
        "Options:\n"
        "   -a MIN-MAX      Set azimuth sweep range (default 0-359).\n"
        "   -d DEVICE       Set antenna rotator device.\n"
        "   -h              Display this help message.\n"
        "   -l FILE         Load recorded data from a file.\n"
        "   -m ID           Set antenna rotator model.\n"
        "   -r DEVICE       Set receiver device.\n"
        "   -w FILE         Write data log to a file.\n"
    );
}

/**
 * Parse command-line arguments and load configuration defaults.
 */
static void parse_args(int argc, char *argv[]) {
    int flag;

    // Load default settings.
    config.rot_file = "/dev/ttyU0";
    config.rig_file = "/dev/ttyU1";
    config.rig_model = 214;
    config.az_min = 0, config.az_max = 359;
    config.el_min = 0, config.el_max = 180;
    config.write_file = "data.log";

    while ((flag = getopt(argc, argv, "a:d:hl:m:r:w:")) != -1) {
        switch (flag) {
            case 'a':
                if (sscanf(optarg, "%d,%d", &config.az_min,
                            &config.az_max) != 2) {
                    print_quickhelp();
                    exit(EXIT_FAILURE);
                }

                if (config.az_min < 0 || config.az_max > 360) {
                    fprintf(stderr, "Invalid azimuth sweep range\n");
                    print_quickhelp();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'd':
                config.rot_file = optarg;
                break;
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
                break;
            case 'l':
                config.load_file = optarg;
                break;
            case 'm':
                config.rot_model = atoi(optarg);
                break;
            case 'r':
                config.rig_file = optarg;
                break;
            case 'w':
                config.write_file = optarg;
                break;
            case '?':
                print_quickhelp();
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Complain about required options that are not specified.
    if (config.rot_model == 0 && config.load_file == NULL) {
        fprintf(stderr, "You must specify one of '-m' or '-l'.\n");
        print_quickhelp();
        exit(EXIT_FAILURE);
    }
}

/**
 * Clean up before exiting.
 */
static void cleanup() {
    fprintf(stderr, "Cleaning up...\n");
    rotator_close();
    receiver_close();
}

/**
 * Initialize sandboxing using Capsicum.
 */
static void init_sandbox() {
#ifdef HAVE_CAPSICUM
    if (cap_enter() != 0) {
        perror("Could not start Capsicum");
        exit(EXIT_FAILURE);
    }
#endif
}

/**
 * Main program entry point.
 */
int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    // Load data from a file if one is set, otherwise collect new data.
    if (config.load_file != NULL) {
        FILE *data_file = fopen(config.load_file, "r");
        if (data_file == NULL) {
            perror("Could not load data file");
            exit(EXIT_FAILURE);
        }

        init_sandbox();
        data_load(data_file);
        fclose(data_file);
    } else {
        rotator_open();
        receiver_open();
        data_init();
        init_sandbox();
        atexit(cleanup);
        signal(SIGINT, exit);
        tests_run();
    }
}
