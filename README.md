# HARDWARE LIFESPAN SHORTENER
![Linguagem](https://img.shields.io/badge/Language-C-blue)
![Plataforma](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-red)
![Licença](https://img.shields.io/badge/License-Do%20What%20The%20Fuck%20You%20Want-green)
![Saúde](https://img.shields.io/badge/Your%20PC's%20Health-At%20Risk-orange)
</br>
An idiotic piece software made by me in C, it's one and only function in this world is to shorten your hardware lifespan.
It will stress your CPU, SSD and RAM, i will work on a GPU stresser soon. 
Both Linux and Windows verion do the same thing essencially, with some differences which i will explain.
theres how it stresses your shit<br/><br/>
WINDOWS VERSION<br/>
<br/>
-- CPU --<br/>
it stresses your CPU mainly using this math function
```
while(1) {
    double r = 0.0;
    for(int i = 0; i < 1000000; i++) 
        r += sin((double)i) * cos((double)i);
}
```
tf this does?</br>
creates an infinite loop performing heavy mathematical calculations using sin() and cos() functions (which are among the most computationally expensive operations)</br>
Creates one thread per CPU core (detected via GetSystemInfo())
</br></br>
-- RAM --
the code snippet that tries to destroy your ram is
```
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
```
now you may ask what this does, it's pretty simple actually</br>
it allocates 1GB memory blocks repeatedly using VirtualAlloc()</br>
fills each allocated block with data (writing to every 4096th byte) and it runs 2 parallel threads during this allocation 
</br></br>

-- STORAGE --</br></br>
and finally for the storage
```
while(1) {
    SetFilePointerEx(fh, random_position, NULL, FILE_BEGIN);
    WriteFile(fh, buf, 4096, &bw, NULL);
    FlushFileBuffers(fh);  // Forces immediate physical write
}

// Read thread:
while(1) {
    SetFilePointerEx(fh, random_position, NULL, FILE_BEGIN);
    ReadFile(fh, buf, 4096, &br, NULL);
}
```
it writes and reads 4KB blocks at random positions</br>
uses FILE_FLAG_NO_BUFFERING and FILE_FLAG_WRITE_THROUGH to bypass OS cache
</br></br></br>
**BEWARE**
</br>
ONLY USE THIS SOFTWARE IF YOU KNOW WHAT YOU DOING I DO NOT TAKE ANY RESPOSABILITY OF ANY DAMAGE THIS MAY CAUSE
For now, it only have a CLI version, i will work on a GUI version on a near future... maybe when Half-Life 3 releases.
