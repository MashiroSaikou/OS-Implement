#include "task.h"
#include "type.h"

#define SPINLOCK_LOCKED 0
#define SPINLOCK_UNLOCKED 1

typedef volatile uint32 spinlock_t;

typedef struct _semaphore_t
{
    uint32 counter;
    spinlock_t lock;
    //thread_list_t queue;
}semaphore_t;

void spinlock_lock (spinlock_t *lock);

void spinlock_unlock (spinlock_t *lock);