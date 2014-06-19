/**
 * @file
 * Main executable.
 */

#include <glib.h>
#include <hamlib/rotator.h>
#include <stdlib.h>
#include <string.h>

struct {
    char *rot_file;
    int rot_model;
} config;

/** Command line options, descriptions, and parameters. */
static GOptionEntry options[] = {
    { "model", 'm', 0, G_OPTION_ARG_INT, &config.rot_model,
        "Rotator model", "ID" },
    { "device", 'd', 0, G_OPTION_ARG_STRING, &config.rot_file,
        "Rotator device", "DEVICE" },
    { NULL },
};

/**
 * Parse command-line arguments.
 */
static void parse_args(int argc, char *argv[]) {
    GOptionContext *context = g_option_context_new("- MARP");
    GError *error = NULL;

    g_option_context_add_main_entries(context, options, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }

    g_option_context_free(context);
}

/**
 * Main program entry point.
 */
int main(int argc, char *argv[]) {
    ROT *rot;

    parse_args(argc, argv);

    if (config.rot_model == 0) {
        g_message("Please select a rotator model.");
        exit(EXIT_FAILURE);
    }

    rot = rot_init(config.rot_model);
    if (rot == NULL) {
        g_critical("Unknown rotator model!");
        exit(EXIT_FAILURE);
    }

    if (config.rot_file != NULL) {
        g_strlcpy(rot->state.rotport.pathname, config.rot_file, FILPATHLEN);
        g_free(config.rot_file);
    }

    if (rot_open(rot) != RIG_OK) {
        g_critical("Could not open rotator!");
        exit(EXIT_FAILURE);
    }

    rot_close(rot);
    rot_cleanup(rot);
} 
