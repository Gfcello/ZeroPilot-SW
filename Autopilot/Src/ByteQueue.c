/**.
 * @file ByteQueue.c
 * @author Serge Babayan
 * @date Feb 19, 2017
 * @copyright Waterloo Aerial Robotics Group 2017 \n
 *   https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md 
 */

#include "ByteQueue.h"
#include <stddef.h>
#include <stdlib.h>

static uint8_t resizeBQueue(ByteQueue* queue, uint32_t new_size);

void initBQueue(ByteQueue* queue, uint32_t initial_size, uint32_t max_size)
{
    queue->_data = malloc(initial_size);
    queue->_size = 0;
    queue->_total_size = initial_size;
    queue->_start_index = 0;
    queue->_initial_size = initial_size;
    queue->_max_size = max_size;
}

uint8_t popBQueue(ByteQueue* queue)
{
    if (queue->_size == 0) {
        return -1;
    } else {
        queue->_size--;
        uint8_t to_return = queue->_data[queue->_start_index];

        //prevent any overflow
        queue->_start_index = (queue->_start_index + 1) % queue->_total_size;

        //if the queue size is a quarter of the total size, half the size of the queue for space efficiency
        if (queue->_size <= queue->_total_size / 4 && queue->_total_size / 2 >= queue->_initial_size) {
            resizeBQueue(queue, queue->_total_size / 2); //resize the queue by a half
        }

        return to_return;
    }
}

uint8_t pushBQueue(ByteQueue* queue, uint8_t byte)
{
    //if the queue is full
    if (queue->_size == queue->_total_size) {
        uint32_t expand_size = queue->_total_size * 2;

        //if we've already reached the max size of the queue
        if (queue->_total_size == queue->_max_size) {
            return 0;
        } else if (expand_size > queue->_max_size) { //otherwise adjust to take up as much space as possible
            expand_size = queue->_max_size;
        }

        uint8_t resize_status = resizeBQueue(queue, expand_size);

        if (resize_status == 0) { //if we couldn't reallocate more data
            return 0;
        }
    }

    queue->_data[(queue->_start_index + queue->_size) % queue->_total_size] = byte;
    queue->_size++;
    return 1;
}

uint32_t getBQueueSize(ByteQueue* queue)
{
    return queue->_size;
}

uint32_t getBQueueSpace(ByteQueue* queue)
{
    return queue->_max_size - queue->_size;
}

void deleteBQueue(ByteQueue* queue)
{
    free((void*) queue->_data);
    queue->_size = 0;
}

/**
 * Pops all the elements in the queue and copies them into a provided array. Make
 * sure this array is at least the size of the queue, otherwise you'll get data
 * corruption. This method is useful if you want to read the entire queue all at once,
 * which will avoid the overhead of a function call
 * @param queue
 * @param array Byte array that is at least the size of the queue
 */
static void popAllBQueue(ByteQueue* queue, uint8_t* array)
{
    if (queue->_size != 0) {
        uint32_t i;
        for (i = 0; i < queue->_size; i++) {
            array[i] = queue->_data[queue->_start_index];
            queue->_start_index = (queue->_start_index + 1) % queue->_total_size;
        }
        queue->_size = 0;
        queue->_start_index = 0;
    }
}

/**
 * Resizes a queue (either shrinks or grows) to the specified new size. Note that
 * realloc cannot be used for the queue as the start index can start at any point
 * in the array. Note that this method will not respect the _max_size property of the
 * queue, so make sure to do that externally
 * @param queue
 * @param new_size
 * @return 1 if successful, 0 if not (if allocation failed)
 */
static uint8_t resizeBQueue(ByteQueue* queue, uint32_t new_size)
{
    uint32_t old_size = queue->_size;
    uint8_t* new_data = malloc(new_size);

    if (new_data == 0) {
        return 0;
    }

    popAllBQueue(queue, new_data);

    free((void*) queue->_data); //free the old data
    queue->_data = new_data;
    queue->_size = old_size;
    queue->_total_size = new_size;
    return 1;
}
