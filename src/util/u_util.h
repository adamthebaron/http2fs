#ifndef _UTIL_H_
#define _UTIL_H_

void u_prefixarr(u8int*, int, int*);
void u_parsereq(HConn*, TData*);
void u_hdrframeresp(u8int*, uint);
int u_stgsframeresp(HConn*, TData*);

#endif // _UTIL_H_
