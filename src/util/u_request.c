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
	print("got raw request len(%d): ", conn->rreq.len);
	for(u64int i = 0; i < conn->rreq.len; i++)
		print("%0x ", conn->rreq.buf[i]);
	print("\n");
	while(curpos < conn->rreq.len)
	{
		print("curpos: %d of %d bytes pos (len and type should be: %x %x %x %x)\n",
															   curpos, conn->rreq.len,
															   conn->rreq.buf[curpos],
															   conn->rreq.buf[curpos + 1],
															   conn->rreq.buf[curpos + 2],
															   conn->rreq.buf[curpos + 3]);
		if(memcmp(&(conn->rreq.buf[curpos]), Http2ConnPrefix, sizeof(Http2ConnPrefix)) == 0x0)
		{
			curpos += sizeof(Http2ConnPrefix);
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
				//u_dataframeresp();
				break;
			case 0x1:
				/* headers */
				u_hdrframeresp(framebuf, framelen);
				break;
			case 0x2:
				/* priority */
				//u_priframeresp();
				break;
			case 0x3:
				/* rststream */
				//u_rststrframeresp();
				break;
			case 0x4:
				/* settings */
				u_stgsframeresp(conn, data);
				break;
			case 0x5:
				/* push promise */
				//u_ppframeresp();
				break;
			case 0x6:
				/* ping */
				//u_pingframeresp();
				break;
			case 0x7:
				/* goaway */
				//u_goawayframeresp();
				break;
			case 0x8:
				/* winup */
				//u_winupframeresp();
				break;
			case 0x9:
				/* cont */
				//u_contframeresp();
				break;
		}
		//curpos += 9;
		print ("curpos is now %d\n", curpos);
	}
	return;
}
