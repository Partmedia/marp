/**
 * @file
 * Receiver controller for TS-2000.
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "receiver.h" 

static int fd;

/**
 * Open receiver interface. This function exits on error.
 */
void receiver_open(const char *device) {
    struct termios options;

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        fprintf(stderr, "Could not open receiver on '%s': %s\n",
                device, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &options);

    // Set 8 data bits and no parity.
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set hardware flow control.
    options.c_cflag |= CRTSCTS;

    // Set raw output.
    options.c_lflag &= ~(ICANON | IEXTEN | ECHO);
    options.c_iflag &= ~(INPCK | ICRNL);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
}

float receiver_get_strength(int unit) {
    char buf[8 + 1];
    int value;

    assert(unit >= 0 && unit < 4);

    // Send signal strength request.
    snprintf(buf, sizeof(buf), "sm%d;", unit);
    if (write(fd, buf, 4) != 4) {
        perror("Receiver write error");
        return -1;
    }

    // Read signal strength answer.
    if (read(fd, buf, 8) < 0) {
        perror("Receiver read error");
        return -1;
    }

    buf[sizeof(buf) - 1] = '\0';
    value = atoi(&buf[3]);

    if (unit == 0 || unit == 2) {
        // Main transceiver values range from 0000 to 0030.
        return value / 30.0;
    } else {
        // Sub transceiver values range from 0000 to 0015.
        return value / 15.0;
    }
}

/**
 * Close rotator interface.
 */
void receiver_close() {
    close(fd);
}
