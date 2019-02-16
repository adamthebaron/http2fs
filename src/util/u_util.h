#ifndef _UTIL_H_
#define _UTIL_H_

void u_prefixarr(u8int*, int, int*);
void u_parsereq(HConn*, TData*);
void u_hdrframeresp(u8int*, u64int, uint);
void u_stgsframeresp(u8int*, u64int, uint);

#endif // _UTIL_H_
