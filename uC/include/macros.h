#ifndef __MACROS_H__
#define __MACROS_H__

#define BIT(x) (1 << (x)) 
#define SETBITS(x,y) ((x) |= (y)) 
#define CLEARBITS(x,y) ((x) &= (~(y))) 
#define SETBIT(x,y) SETBITS((x), (BIT((y)))) 
#define CLEARBIT(x,y) CLEARBITS((x), (BIT((y)))) 
#define BITSET(x,y) ((x) & (BIT(y))) 
#define BITCLEAR(x,y) !BITSET((x), (y)) 
#define BITSSET(x,y) (((x) & (y)) == (y)) 
#define BITSCLEAR(x,y) (((x) & (y)) == 0) 
#define BITVAL(x,y) (((x)>>(y)) & 1) 
#define WRITEMASKEDBYTE(byte, data, mask) ((byte) ^= ((byte) ^ (data)) & (mask))

#define CONCAT(a, b)            a ## b
#define CONCAT_EXPANDED(a, b)   CONCAT(a, b)

#define GETPORT(name)           CONCAT(PORT, name)
#define GETPIN(name)            CONCAT(PIN, name)
#define GETDDR(name)            CONCAT(DDR, name)

#endif
