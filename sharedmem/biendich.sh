rm -f makeshm readshm readshm2 writeshm writeshm2

gcc sharedmem/makeshm.c -o makeshm
gcc sharedmem/readshm.c -o readshm
gcc sharedmem/readshm2.c -o readshm2
gcc sharedmem/writeshm.c -o writeshm
gcc sharedmem/writeshm2.c -o writeshm2