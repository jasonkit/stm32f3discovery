#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os.h"

#define RINGBUF_SIZE 1024

#define RINGBUF_OK 1
#define RINGBUF_ERR_INVALID -1
#define RINGBUF_ERR_RESOURCES_NOT_AVALIABLE -2

struct RingBuf {
  uint8_t buf[RINGBUF_SIZE];
  uint16_t rd;
  uint16_t wr;
  osMutexId mutex;
};

uint16_t ringbuf_remaining(struct RingBuf* rbuf, bool useMutex);
uint16_t ringbuf_avaliable(struct RingBuf* rbuf, bool useMutex);
int ringbuf_write(struct RingBuf* rbuf, void* data, size_t size);
int ringbuf_read(struct RingBuf* rbuf, void* data, size_t size);
#endif
