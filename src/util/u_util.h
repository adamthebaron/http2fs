#ifndef _UTIL_H_
#define _UTIL_H_

#define u_shiftarr_left 0x01
#define u_shiftarr_right 0x00

void u_printarr(u8int*, u64int, char*);
void u_shiftarr(u8int*, u64int, u8int, u8int);
void u_prefixarr(u8int*, int, int*);
void u_parsereq(HConn*, TData*);
void u_hdrframeresp(u8int*, u64int, uint);
void u_stgsframeresp(u8int*, u64int, uint);

#endif // _UTIL_H_
