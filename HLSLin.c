#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

void* cpu_thread(void* p) {
    while(1) {
        double r = 0.0;
        for(int i = 0; i < 1000000; i++) {
            r += sin((double)i) * cos((double)i);
        }
    }
    return NULL;
}

void max_cpu() {
    int tc = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t th[64];
    int ids[64];
    
    
    
    for(int i = 0; i < tc && i < 64; i++) {
        ids[i] = i;
        if(pthread_create(&th[i], NULL, cpu_thread, &ids[i]) != 0) {
            perror("pthread_create cpu");
        }
    }
    
    for(int i = 0; i < tc && i < 64; i++) {
        pthread_detach(th[i]);
    }
}

typedef struct {
    char* blocks[5000];
    int count;
    int size;
} mem_ctx;

void* mem_thread(void* p) {
    mem_ctx* ctx = (mem_ctx*)p;
    
    while(ctx->count < 5000) {
        char* b = (char*)mmap(NULL, ctx->size, 
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        
        if(b != MAP_FAILED) {
            for(int i = 0; i < ctx->size; i += 4096) {
                b[i] = (char)(i % 256);
            }
            ctx->blocks[ctx->count++] = b;
        } else {
            break;
        }
    }
    return NULL;
}

void max_ram() {
    int tc = 2;
    int mb = 1024;
    int sz = mb * 1024 * 1024;
    pthread_t th[8];
    mem_ctx ctxs[8];
    
    
    
    for(int i = 0; i < tc; i++) {
        memset(&ctxs[i], 0, sizeof(mem_ctx));
        ctxs[i].size = sz;
        pthread_create(&th[i], NULL, mem_thread, &ctxs[i]);
    }
}

void* disk_write(void* p) {
    int id = *(int*)p;
    char fn[32];
    sprintf(fn, "d%d.tmp", id);
    
    int fd = open(fn, O_WRONLY | O_CREAT | O_DIRECT | O_SYNC, 0644);
    if(fd < 0) {
        perror("open write");
        return NULL;
    }
    
    char* buf;
    if(posix_memalign((void**)&buf, 4096, 4096) != 0) {
        close(fd);
        return NULL;
    }
    
    srand((unsigned int)(time(NULL) + id));
    
    while(1) {
        off_t off = (rand() % (1024LL * 1024 * 512)) & ~4095LL;
        lseek(fd, off, SEEK_SET);
        
        for(int i = 0; i < 4096; i++) {
            buf[i] = (char)(rand() % 256);
        }
        
        write(fd, buf, 4096);
        fsync(fd);
    }
    
    free(buf);
    close(fd);
    return NULL;
}

void* disk_read(void* p) {
    int id = *(int*)p;
    char fn[32];
    sprintf(fn, "r%d.tmp", id);
    
    int fd = open(fn, O_RDWR | O_CREAT | O_DIRECT, 0644);
    if(fd < 0) {
        perror("open read");
        return NULL;
    }
    
    char* buf;
    if(posix_memalign((void**)&buf, 4096, 4096) != 0) {
        close(fd);
        return NULL;
    }
    
   
    for(long long b = 0; b < 1024LL * 128; b++) {
        for(int i = 0; i < 4096; i++) {
            buf[i] = (char)(i % 256);
        }
        write(fd, buf, 4096);
    }
    
    srand((unsigned int)(time(NULL) + id));
    
    while(1) {
        off_t off = (rand() % (1024LL * 1024 * 512)) & ~4095LL;
        lseek(fd, off, SEEK_SET);
        
        read(fd, buf, 4096);
        
        volatile char cs = 0;
        for(int i = 0; i < 4096; i += 64) {
            cs += buf[i];
        }
    }
    
    free(buf);
    close(fd);
    return NULL;
}

void max_disk() {
    int wc = 4;
    int rc = 4;
    pthread_t th[16];
    int ids[16];
    int tot = 0;
    
    
    for(int i = 0; i < wc; i++) {
        ids[tot] = tot;
        pthread_create(&th[tot], NULL, disk_write, &ids[tot]);
        tot++;
    }
    
    for(int i = 0; i < rc; i++) {
        ids[tot] = tot;
        pthread_create(&th[tot], NULL, disk_read, &ids[tot]);
        tot++;
    }
}

int main(int argc, char* argv[]) {

    
    max_cpu();
    max_ram();
    max_disk();
    
   
    while(1) {
        sleep(10);
    }
    
    return 0;

}
