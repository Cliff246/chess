#ifndef __BITMANIP__
#define __BITMANIP__

#define SETBIT(n, k) (1 | (n << k))
#define CLRBIT(n, k) (n & ~(1 << k))
#define GETBIT(n, k) (1 & (n >> k)) 

#endif

