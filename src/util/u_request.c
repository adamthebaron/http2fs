#include "../http2fs.h"
#include "u_util.h"

/*
 * u_parsereq
 */
void
u_parsereq(HConn* conn, TData* data)
{
	u8int frametype;
	u8int* framebuf;
	u32int curpos, curlen, framelen;

	framebuf = calloc(MaxBuf, sizeof(u8int));
	framelen = curlen = curpos = 0;
	while(curpos < conn->rreq.len)
	{
		print("curpos: %d of %d bytes\n", curpos, conn->rreq.len);
		if(memcmp(&(conn->rreq.buf[curpos]), Http2ConnPrefix, 3) == 0x0)
		{
			framelen = 24;
			print("found connection prefix\n");
			continue;
		}
		framelen = (conn->rreq.buf[curpos] << 16) |
				   (conn->rreq.buf[curpos + 1] << 8) |
				   conn->rreq.buf[curpos + 2];
		frametype = conn->rreq.buf[curpos + 3];
		print("http2 frame of type %x of size %d\n", frametype, framelen);
		memcpy(framebuf, &(conn->rreq.buf[curpos]), framelen + 9);
		switch(frametype)
		{
			case 0x0:
				/* data */
			case 0x1:
				/* headers */
			case 0x2:
				/* priority */
			case 0x3:
				/* rststream */
			case 0x4:
				/* settings */
			case 0x5:
				/* push promise */
			case 0x6:
				/* ping */
			case 0x7:
				/* goaway */
			case 0x8:
				/* winup */
			case 0x9:
				/* cont */
		}
		curpos += 9;
		print ("curpos is now %d\n", curpos);
	}
	return;
}
