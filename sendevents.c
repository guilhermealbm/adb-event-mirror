#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

typedef uint32_t        __u32;
typedef uint16_t        __u16;
typedef __signed__ int  __s32;

struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};


#define EVIOCGVERSION		_IOR('E', 0x01, int)			/* get driver version */

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

static int getLine (char *buff, size_t sz) {
    int ch, extra;

    fflush (stdout);
    if (fgets (buff, sz, stdin) == NULL)
        return NO_INPUT;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return OK;
}


int main(int argc, char *argv[]) {
    int rc;
    char buff[64];

    while(strcmp(buff, "exit") != 0) {
        rc = getLine (buff, sizeof(buff));
        if (rc == NO_INPUT) {
            fprintf (stderr, "\nNo input\n");
            return 1;
        }

        if (rc == TOO_LONG) {
            fprintf (stderr, "Input too long [%s]\n", buff);
            return 1;
        }

        int fd;
        ssize_t ret;
        int version;
        struct input_event event;

        char *args[4];
        int i = 0;
        args[i] = strtok(buff, " ");

        while(args[i] != NULL) {

            char *string = strtok(NULL, " ");
            if (string != NULL) {
                args[++i] = string;
            } else {
                if (i != 3) {
                    fprintf(stderr, "You should pass exactly 4 args. Index %d\n", ++i);
                    return 1;
                }
                break;
            }
        }
        
        fd = open(args[0], O_RDWR);
        if(fd < 0) {
            fprintf(stderr, "could not open %s, %s\n", args[0], strerror(errno));
            return 1;
        }
        if (ioctl(fd, EVIOCGVERSION, &version)) {
            fprintf(stderr, "could not get driver version for %s, %s\n", args[0], strerror(errno));
            return 1;
        }
        memset(&event, 0, sizeof(event));
        event.type = atoi(args[1]);
        event.code = atoi(args[2]);
        event.value = atoi(args[3]);
        ret = write(fd, &event, sizeof(event));
        if(ret < (ssize_t) sizeof(event)) {
            fprintf(stderr, "write event failed, %s\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}