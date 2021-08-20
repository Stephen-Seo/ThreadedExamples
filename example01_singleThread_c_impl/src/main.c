#include <stdio.h>
#include <threads.h>
#include <string.h>

typedef struct ThreadData {
    mtx_t mutex;
    int lock_before_printing;
} ThreadData;

int functionCalledByThread(void *user_data) {
    ThreadData *data = (ThreadData*)user_data;

    if(data->lock_before_printing != 0) {
        // lock before printing
        mtx_lock(&data->mutex);
    }
    puts("Hello from spawned thread");
    if(data->lock_before_printing != 0) {
        // unlock after printing
        mtx_unlock(&data->mutex);
    }

    return 0;
}

int main(int argc, char **argv) {
    // create the ThreadData that is shared with the thread
    ThreadData td;
    mtx_init(&td.mutex, mtx_plain);
    td.lock_before_printing = 1;

    // check args
    --argc; ++argv;
    while(argc > 0) {
        if(strcmp("-n", argv[0]) == 0 || strcmp("--no-lock", argv[0]) == 0) {
            td.lock_before_printing = 0;
            puts("Disabling use of mutex before printing");
        }
        --argc; ++argv;
    }

    // create the thread
    thrd_t thread_handle;
    thrd_create(&thread_handle, functionCalledByThread, &td);

    if(td.lock_before_printing != 0) {
        // lock before printing
        mtx_lock(&td.mutex);
    }
    puts("Hello from main thread");
    if(td.lock_before_printing != 0) {
        // unlock after printing
        mtx_unlock(&td.mutex);
    }

    thrd_join(thread_handle, NULL);

    // cleanup
    mtx_destroy(&td.mutex);
    return 0;
}
