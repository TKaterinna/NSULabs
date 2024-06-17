#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define BUFFER_SIZE 64
#define FUNC_ERROR -1
//#define HEAP_SIZE 1024 * 16 //16 Kb 
#define HEAP_SIZE 10*64 
#define NUM_PAGES 2
#define NUM_BUFFERS 10

void* memory = NULL;
size_t endpos = 0;

typedef struct st_block_info {
    struct st_block_info* next_addr;
    size_t size;
    int free = 1;
} block_info;

block_info* first_block = NULL;

#define BLOCK_INFO_SIZE sizeof(block_info)

block_info* find_free_block (block_info** last_block, size_t size) {
    block_info* cur_block = first_block;
    while (cur_block && !(cur_block->free && cur_block->size >= size)) {
        *last_block = cur_block;
        cur_block = cur_block->next_addr;
    }
    return cur_block;
}

block_info *request_block(size_t size)
{
    block_info* last = NULL;
    block_info* block = find_free_block(&last, size);
    if (block) {
        block->free = 0;
        size_t overage = block->size - size;
        if (overage > BLOCK_INFO_SIZE) {
            block->size = size;
            block_info* new_block = (block_info*)((char*)(block + 1) + block->size);
            new_block->free = 1;
            new_block->next_addr = block->next_addr;
            new_block->size = overage - BLOCK_INFO_SIZE;
            block->next_addr = new_block;
        }
        
        return block;
    }

    if (endpos + BLOCK_INFO_SIZE + size > HEAP_SIZE) {
        return NULL;
    }

    block = (block_info*)((char*)memory + endpos);
    endpos += BLOCK_INFO_SIZE + size;
    if (last) {
        last->next_addr = block;
    }
    else {
        first_block = block;
    }
    block->free = 0;
    block->next_addr = NULL;
    block->size = size;

    return block;
}

void* my_malloc(size_t size) {
    block_info* block = request_block(size);
    if (block) {
        return block + 1;
    }
    return NULL;
}
void my_free(void* addr) {
    block_info* block = (block_info*)((char*)addr - BLOCK_INFO_SIZE);
    block->free = 1;
    while (block->next_addr && block->next_addr->free) {
        block->size += block->next_addr->size + BLOCK_INFO_SIZE;
        block->next_addr = block->next_addr->next_addr;
    }
}

int main (int argc, char *argv[]) {
    printf("pid: %d\n", getpid());

    unsigned int ret_sleep;
    ret_sleep = sleep(2);
    if (ret_sleep != PROG_SUCCESS) {
        perror("interrupted sleep");
        return PROG_FAILED;
    }

    printf("start\n");
    
    memory = mmap(NULL, HEAP_SIZE, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap error");
        return PROG_FAILED;
    }
    printf("memory %p\n", memory);
    printf("BLOCK_INFO_SIZE = %ld\n", BLOCK_INFO_SIZE);

    int count;
    char* arr;
    char* arrs[NUM_BUFFERS];
    for (count = 0; count < NUM_BUFFERS; count++) {
        printf("%d\n", count);
        arr = (char*)my_malloc(BUFFER_SIZE);
        if (arr == NULL) {
            perror("my_malloc error");
            return PROG_FAILED;
        }
        arrs[count] = arr;
        memcpy(arr, "hello", strlen("hello"));
        printf("addr %p\n", arr);
    }

    printf("%s\n", arrs[1]);

    // for (count = 0; count < NUM_BUFFERS / 2; count++) {
    //     my_free(arrs[count]);
    // }
    // for (count = 0; count < NUM_BUFFERS / 2; count++) {
    //     arr = (char*)my_malloc(BUFFER_SIZE * 2);
    //     if (arr == NULL) {
    //         perror("my_malloc error");
    //         return PROG_FAILED;
    //     }
    //     arrs[count] = arr;
    //     printf("%d\n", count);
    //     printf("addr %p\n", arr);
    // }

    for (count = NUM_BUFFERS / 2 - 1; count >= 0; count--) {
        my_free(arrs[count]);
    }
    for (count = NUM_BUFFERS / 2 - 1; count >= 0; count--) {
        arr = (char*)my_malloc(BUFFER_SIZE * 2);
        if (arr == NULL) {
            perror("my_malloc error");
            return PROG_FAILED;
        }
        arrs[count] = arr;
        printf("%d\n", count);
        printf("addr %p\n", arr);
    }

    for (count = 0; count < NUM_BUFFERS; count++) {
        my_free(arrs[count]);
    }

    int ret;
    ret = munmap(memory, HEAP_SIZE);
    if (ret == FUNC_ERROR) {
        perror("munmap");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}