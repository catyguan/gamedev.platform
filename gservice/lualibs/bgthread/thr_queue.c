#include <stdlib.h>             // malloc
#include "thr_queue.h"

struct thr_queue_s {
        void ** _queue;

        size_t _max_size;
        size_t _cur_size;
        size_t _head;
        size_t _tail;

        pthread_mutex_t _mutex;
        pthread_cond_t _cond_avail;     // signal when queue is not full, can push more
        pthread_cond_t _cond_have;      // signal when queue is not empty, can pop
};

// ---------------- Create/Destroy -------------------
int thr_queue_create(thr_queue_t *q, size_t max_size)
{
        struct thr_queue_s *queue = (struct thr_queue_s *) malloc(sizeof(struct thr_queue_s));

        if (queue == NULL) {
                return -1;
        }

        queue->_queue = (void **) malloc(max_size * sizeof(void *));
        if (queue->_queue == NULL) {
                free(queue);
                queue = NULL;
                return -1;
        }

        queue->_max_size = max_size;
        queue->_cur_size = queue->_head = queue->_tail = 0;

        pthread_mutex_init(&queue->_mutex, NULL);
        pthread_cond_init(&queue->_cond_avail, NULL);
        pthread_cond_init(&queue->_cond_have, NULL);

        *q = queue;
        return 0;
}

void thr_queue_destroy(thr_queue_t *q)
{
        struct thr_queue_s *queue;

        if ( q == NULL || *q == NULL )
                return;

        queue = *q;

        pthread_cond_destroy(&queue->_cond_have);
        pthread_cond_destroy(&queue->_cond_avail);
        pthread_mutex_destroy(&queue->_mutex);

        free(queue->_queue);
        free(queue);
}


// ---------------- push/pop -------------------
size_t thr_queue_size(const thr_queue_t *self)
{
        return (*self)->_cur_size;
}

size_t thr_queue_capacity(const thr_queue_t *self)
{
        return (*self)->_max_size;
}

void thr_queue_push(thr_queue_t *q, void *in)
{
        struct thr_queue_s *self = *q;
        pthread_mutex_lock(&self->_mutex);

        while (self->_cur_size == self->_max_size)
        {
                // full? wait it
                pthread_cond_wait(&self->_cond_avail, &self->_mutex);
        }

        self->_queue[self->_tail++] = in;
        if (self->_tail == self->_max_size)
        {
                self->_tail = 0;
        }

        if (self->_cur_size++ == 0)
        {
                // not empty? wakeup pop()
                pthread_cond_signal(&self->_cond_have);
        }

        pthread_mutex_unlock(&self->_mutex);
}

void thr_queue_pop(thr_queue_t *q, void **out)
{
        struct thr_queue_s *self = *q;
        pthread_mutex_lock(&self->_mutex);

        while (self->_cur_size == 0)
        {
                // empty? wait
                pthread_cond_wait(&self->_cond_have, &self->_mutex);
        }

        if (out)
        {
                *out = self->_queue[self->_head];
        }

        self->_head++;
        if (self->_head == self->_max_size)
        {
                self->_head = 0;
        }

        if (self->_cur_size-- == self->_max_size)
        {
                // not full? wakeup push()
                pthread_cond_signal(&self->_cond_avail);
        }

        pthread_mutex_unlock(&self->_mutex);
}

int thr_queue_peek(thr_queue_t *q, void **out)
{
        struct thr_queue_s *self = *q;
        pthread_mutex_lock(&self->_mutex);

        if(self->_cur_size == 0)
        {
                pthread_mutex_unlock(&self->_mutex);
				return 0;
        }

        if (out)
        {
                *out = self->_queue[self->_head];
        }

        self->_head++;
        if (self->_head == self->_max_size)
        {
                self->_head = 0;
        }

        if (self->_cur_size-- == self->_max_size)
        {
                // not full? wakeup push()
                pthread_cond_signal(&self->_cond_avail);
        }

        pthread_mutex_unlock(&self->_mutex);
		return 1;
}