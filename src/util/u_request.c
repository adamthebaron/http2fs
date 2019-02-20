#include "../http2fs.h"
#include "u_util.h"

/*
 * u_parsereq
 */
void
u_parsereq(TData* data)
{
	u8int frametype;
	u8int* framebuf;
	u64int curpos, curlen, framelen;

	framebuf = calloc(MaxBuf, sizeof(u8int));
	framelen = curlen = curpos = 0;
	print("got raw request len(%d): ", data->conn->rreq.len);
	for(u64int i = 0; i < data->conn->rreq.len; i++)
		print("%0x ", data->conn->rreq.buf[i]);
	print("\n");
	while(curpos < data->conn->rreq.len)
	{
		print("curpos: %d of %d bytes pos (len and type should be: %x %x %x %x)\n",
															   curpos, conn->rreq.len,
															   data->conn->rreq.buf[curpos],
															   data->conn->rreq.buf[curpos + 1],
															   data->conn->rreq.buf[curpos + 2],
															   data->conn->rreq.buf[curpos + 3]);
		if(memcmp(&(conn->rreq.buf[curpos]), Http2ConnPrefix, sizeof(Http2ConnPrefix)) == 0x0)
		{
			curpos += sizeof(Http2ConnPrefix);
			print("found connection prefix\n");
			continue;
		}
		// add 9 to framelen to include frame header
		framelen = (data->conn->rreq.buf[curpos]     << 16) |
				   (data->conn->rreq.buf[curpos + 1] << 8)  |
				    data->conn->rreq.buf[curpos + 2] + 9;
		frametype = data->conn->rreq.buf[curpos + 3];
		print("http2 frame of type %x of size %d\n", frametype, framelen);
		memcpy(framebuf, &(data->conn->rreq.buf[curpos]), framelen);
		switch(frametype)
		{
			case 0x0:
				/* data */
				//u_dataframeresp();
				break;
			case 0x1:
				/* headers */
				print("headers frame found\n");
				u_hdrframeresp(data, framebuf, framelen, data->acfd);
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
				print("settings frame found\n");
				u_stgsframeresp(data, framebuf, framelen, data->acfd);
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
				print("window update frame found\n");
				//u_winupframeresp();
				break;
			case 0x9:
				/* cont */
				//u_contframeresp();
				break;
		}
		curpos += framelen;
		print ("curpos is now %d\n", curpos);
	}
	return;
}
