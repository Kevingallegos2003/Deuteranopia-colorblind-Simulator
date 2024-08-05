#include "io.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct buffer {
    int fd;
    int offset;
    //int a;

    int num_remaining;
    uint8_t a[BUFFER_SIZE];
};

Buffer *read_open(const char *filename) {
    if (open(filename, O_RDONLY) < 0) {
        return NULL;
    }
    Buffer *b = (Buffer *) malloc(sizeof(Buffer));
    b->fd = open(filename, O_RDONLY);
    b->offset = 0;
    b->num_remaining = 0;
    return b;
    //more here
}

void read_close(Buffer **pbuf) {
    close((*pbuf)->fd);
    *pbuf = NULL;
}

Buffer *write_open(const char *filename) {
    int temp = creat(filename, 0664);
    if (temp < 0) {
        return NULL;
    }
    Buffer *b = (Buffer *) malloc(sizeof(Buffer));
    b->fd = temp;
    b->offset = 0;
    b->num_remaining = 0;
    return b;
    //more here
}

void write_close(Buffer **pbuf) {
    write((*pbuf)->fd, (*pbuf)->a, (*pbuf)->offset);
    close((*pbuf)->fd);
    *pbuf = NULL;
}

bool read_uint8(Buffer *buf, uint8_t *x) {
    if (buf->num_remaining == 0) { //below is loop or does smt else?
        ssize_t rc = read(buf->fd, buf->a, sizeof(buf->a));
        if (rc < 0) {
            fprintf(stderr, "error reading uint8, rc<0\n");
        }
        if (rc == 0) {
            return false;
        }
        buf->num_remaining = rc;
        buf->offset = 0;
    }

    *x = buf->a[buf->offset]; //is this the way?
    //coulkd also be worng
    buf->offset++;
    buf->num_remaining--;
    return true;
}

bool read_uint16(Buffer *buf, uint16_t *x) {
    uint8_t temp = 0;
    if (!(read_uint8(buf, &temp))) {
        return false;
    }
    uint8_t var1 = temp;
    if (!(read_uint8(buf, &temp))) {
        return false;
    }
    uint16_t var2 = temp;
    var2 = (var2 << 8);
    var2 = (var1 | var2);
    *x = var2;
    return true;
}

bool read_uint32(Buffer *buf, uint32_t *x) {
    uint16_t temp = 0;
    if (!(read_uint16(buf, &temp))) {
        return false;
    }
    uint16_t var1 = temp;
    if (!(read_uint16(buf, &temp))) {
        return false;
    }
    uint32_t var2 = temp;
    var2 = (var2 << 16);
    var2 = (var1 | var2);
    *x = var2;
    return true;
}

void write_uint8(Buffer *buf, uint8_t x) {
    if (buf->offset == BUFFER_SIZE) {
        uint8_t *start = buf->a;
        int num_bytes = buf->offset;
        do {
            ssize_t rc = write(buf->fd, start, num_bytes);
            if (rc < 0) {
                fprintf(stderr, "Error write uint8, rc<0\n");
            }
            start += rc;
            num_bytes -= rc;
        } while (num_bytes > 0);
        buf->offset = 0;
    }
    buf->a[buf->offset] = x;
    buf->offset++;
}

void write_uint16(Buffer *buf, uint16_t x) {
    write_uint8(buf, x);
    write_uint8(buf, (x >> 8));
}

void write_uint32(Buffer *buf, uint32_t x) {
    write_uint16(buf, x);
    write_uint16(buf, (x >> 16));
}
