#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

DWORD WINAPI cpu_thread(LPVOID p) {
    while(1) {
        double r = 0.0;
        for(int i = 0; i < 1000000; i++) r += sin((double)i) * cos((double)i);
    }
    return 0;
}

void max_cpu() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    int tc = si.dwNumberOfProcessors; 
    HANDLE th[64];
    int ids[64];
    for(int i = 0; i < tc; i++) {
        ids[i] = i;
        th[i] = CreateThread(NULL, 0, cpu_thread, &ids[i], 0, NULL);
    }
}

typedef struct { char* blocks[5000]; int count; int size; } mem_ctx;

DWORD WINAPI mem_thread(LPVOID p) {
    mem_ctx* ctx = (mem_ctx*)p;
    while(ctx->count < 5000) {
        char* b = (char*)VirtualAlloc(NULL, ctx->size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if(b) {
            for(int i = 0; i < ctx->size; i += 4096) b[i] = (char)(i % 256);
            ctx->blocks[ctx->count++] = b;
        } else break;
    }
    return 0;
}

void max_ram() {
    int tc = 2; 
    int mb = 1024; 
    int sz = mb * 1024 * 1024;
    HANDLE th[8];
    mem_ctx ctxs[8];
    for(int i = 0; i < tc; i++) {
        ctxs[i].count = 0;
        ctxs[i].size = sz;
        th[i] = CreateThread(NULL, 0, mem_thread, &ctxs[i], 0, NULL);
    }
}

DWORD WINAPI disk_write(LPVOID p) {
    int id = *(int*)p;
    char fn[32];
    sprintf(fn, "d%d.tmp", id);
    HANDLE fh = CreateFileA(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
                           FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
    if(fh == INVALID_HANDLE_VALUE) return 0;
    char* buf = (char*)_aligned_malloc(4096, 4096); 
    if(!buf) { CloseHandle(fh); return 0; }
    srand((unsigned int)(time(NULL) + id));
    while(1) {
        LARGE_INTEGER off;
        off.QuadPart = (rand() % (1024LL * 1024 * 512)) & ~4095LL; 
        SetFilePointerEx(fh, off, NULL, FILE_BEGIN);
        for(int i = 0; i < 4096; i++) buf[i] = (char)(rand() % 256);
        DWORD bw;
        WriteFile(fh, buf, 4096, &bw, NULL);
        FlushFileBuffers(fh);
    }
    _aligned_free(buf);
    CloseHandle(fh);
    return 0;
}

DWORD WINAPI disk_read(LPVOID p) {
    int id = *(int*)p;
    char fn[32];
    sprintf(fn, "r%d.tmp", id);
    HANDLE fh = CreateFileA(fn, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, 
                           FILE_FLAG_NO_BUFFERING, NULL);
    if(fh == INVALID_HANDLE_VALUE) return 0;
    char* buf = (char*)_aligned_malloc(4096, 4096);
    if(!buf) { CloseHandle(fh); return 0; }
    for(long long b = 0; b < 1024LL * 128; b++) {
        for(int i = 0; i < 4096; i++) buf[i] = (char)(i % 256);
        DWORD bw;
        WriteFile(fh, buf, 4096, &bw, NULL);
    }
    srand((unsigned int)(time(NULL) + id));
    while(1) {
        LARGE_INTEGER off;
        off.QuadPart = (rand() % (1024LL * 1024 * 512)) & ~4095LL;
        SetFilePointerEx(fh, off, NULL, FILE_BEGIN);
        DWORD br;
        ReadFile(fh, buf, 4096, &br, NULL);
        volatile char cs = 0;
        for(int i = 0; i < 4096; i += 64) cs += buf[i];
    }
    _aligned_free(buf);
    CloseHandle(fh);
    return 0;
}

void max_disk() {
    int wc = 4; 
    int rc = 4; 
    HANDLE th[16];
    int ids[16];
    int tot = 0;
    for(int i = 0; i < wc; i++) {
        ids[tot] = tot;
        th[tot] = CreateThread(NULL, 0, disk_write, &ids[tot], 0, NULL);
        tot++;
    }
    for(int i = 0; i < rc; i++) {
        ids[tot] = tot;
        th[tot] = CreateThread(NULL, 0, disk_read, &ids[tot], 0, NULL);
        tot++;
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    
    max_cpu();
    max_ram();
    max_disk();
    

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}