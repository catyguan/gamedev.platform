#ifndef KCODE_THREAD_QUEUE_H
#define KCODE_THREAD_QUEUE_H

#include <sys/types.h>          // size_t
#include <pthread.h>

struct thr_queue_s;
typedef struct thr_queue_s      *thr_queue_t;

int thr_queue_create(thr_queue_t *q, size_t max_size);          // 0, ok; -1, fail
void thr_queue_destroy(thr_queue_t *q);

void thr_queue_push(thr_queue_t *self, void *in);
int thr_queue_peek(thr_queue_t *self, void **out);
void thr_queue_pop(thr_queue_t *self, void **out);
size_t thr_queue_size(const thr_queue_t *self);
size_t thr_queue_capacity(const thr_queue_t *self);

#endif