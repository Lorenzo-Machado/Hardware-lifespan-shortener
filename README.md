# HARDWARE LIFESPAN SHORTENER
An idiotic piece software made by me in C, it's one and only function in this world is to shorten your hardware lifespan.
It will stress your:
-CPU
the max_cpu() function stresses the CPU by creating one thread per logical processor available on the system.
Each thread runs at the same time and executes CPU-intensive work, keeping all CPU cores busy.
As a result, the processor usage can reach close to 100% on all cores, which puts maximum load on the CPU.

-RAM
the max_ram() function stresses system memory by creating multiple threads that continuously allocate large memory blocks.
Each thread allocates 1 GB of memory at a time using VirtualAlloc and writes to each memory page, forcing the operating system to commit physical RAM. The allocations continue until memory can no longer be allocated.
running multiple threads in parallel increases memory pressure faster, leading to very high RAM usage and possible paging.

-SSD/HDD
for the storage, the code stresses the disk by running multiple read and write threads in parallel.
writer threads continuously write 4 KB blocks of random data at random file positions.
each write is flushed immediately, forcing real disk writes.
reader threads repeatedly read 4 KB blocks from random file offsets.
unbuffered I/O is used to bypass the operating system cache.
data is accessed after reading to ensure the disk read actually occurs.
reads and writes happen at the same time.
this creates heavy random disk I/O.
it maximizes disk usage and I/O latency.
useful for disk stress testing. 

For now, it only have a CLI version, i will work on a GUI version on a near future... maybe when Half-Life 3 releases.
