#include "ringbuf.h"
#include <string.h>

uint16_t ringbuf_remaining(struct RingBuf* rbuf, bool useMutex) {
  uint16_t s = 0;
  if (rbuf) {
    if (useMutex && rbuf->mutex) {
      osMutexWait(rbuf->mutex, osWaitForever);
    }

    if (rbuf->wr >= rbuf->rd) {
      s = RINGBUF_SIZE - (rbuf->wr - rbuf->rd) - 1;
    } else {
      s = rbuf->rd - rbuf->wr - 1;
    }

    if (useMutex && rbuf->mutex) {
      osMutexRelease(rbuf->mutex);
    }
  }
  return s;
}

uint16_t ringbuf_avaliable(struct RingBuf* rbuf, bool useMutex) {
  return (RINGBUF_SIZE - ringbuf_remaining(rbuf, useMutex) - 1);
}

int ringbuf_write(struct RingBuf* rbuf, void* data, size_t size) {
  if (rbuf) {
    if (rbuf->mutex) {
      osMutexWait(rbuf->mutex, osWaitForever);
    }
    if (size > ringbuf_remaining(rbuf, false)) {
      if (rbuf->mutex) {
        osMutexRelease(rbuf->mutex);
      }
      return RINGBUF_ERR_RESOURCES_NOT_AVALIABLE;
    }
    uint16_t offset = 0;
    if (rbuf->wr + size > RINGBUF_SIZE) {
      uint16_t chunk_size = RINGBUF_SIZE - rbuf->wr;
      memcpy(&rbuf->buf[rbuf->wr], data, chunk_size);
      size -= chunk_size;
      rbuf->wr = 0;
      offset = chunk_size;
    }

    if (size > 0) {
      memcpy(&rbuf->buf[rbuf->wr], ((uint8_t*)data) + offset, size);
      rbuf->wr += size;
    }

    if (rbuf->mutex) {
      osMutexRelease(rbuf->mutex);
    }
    return RINGBUF_OK;
  } else {
    return RINGBUF_ERR_INVALID;
  }
}

int ringbuf_read(struct RingBuf* rbuf, void* data, size_t size) {
  if (rbuf) {
    if (rbuf->mutex) {
      osMutexWait(rbuf->mutex, osWaitForever);
    }

    if (size > ringbuf_avaliable(rbuf, false)) {
      if (rbuf->mutex) {
        osMutexRelease(rbuf->mutex);
      }
      return RINGBUF_ERR_RESOURCES_NOT_AVALIABLE;
    }

    uint16_t offset = 0;
    if (rbuf->wr + size > RINGBUF_SIZE) {
      uint16_t chunk_size = RINGBUF_SIZE - rbuf->rd;
      memcpy(data, &rbuf->buf[rbuf->rd], chunk_size);
      size -= chunk_size;
      rbuf->rd = 0;
      offset = chunk_size;
    }

    if (size > 0) {
      memcpy(((uint8_t*)data) + offset, &rbuf->buf[rbuf->rd], size);
      rbuf->rd += size;
    }

    if (rbuf->mutex) {
      osMutexRelease(rbuf->mutex);
    }
    return RINGBUF_OK;
  } else {
    return RINGBUF_ERR_INVALID;
  }
}
