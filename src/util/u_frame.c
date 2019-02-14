#include "../http2fs.h"
#include "u_util.h"
#include "../hpack/h_hpack.h"

/*
 * HEADER frame function
 * u_hdrframeresp
 */
void
u_hdrframeresp(u8int* frame, uint s)
{
	u8int len, index, pos;
	u8int* huffmanbuffer;

	/* set pos to 9 to jump over frame header
	 * we want the meat on this bone */
	pos = 9;
	huffmanbuffer = calloc(1024, sizeof(u8int));
	while(pos < s)
	{
		print("checking 0x%x\n", frame[pos]);
		/* indexed header field rep (6.1) */
		if(frame[pos] & 0x80)
		{
			print("indexed header field, index: ");
			index = (0x7f & frame[pos]) - 1;
			print("0x%x\n", index);
			print("header is: %s:%s\n", hpackstatictable[index].name,
										hpackstatictable[index].val);
			pos++;
		}
		/* literal header field with incremental indexing (6.2.1) */
		else if(frame[pos] & 0x40)
		{
			print("literal header field with inc indexing\n");
			index = 0x3f & frame[pos++];
			if(!index)
				print("new header\n");
			else
				print("indexed header\n");
			if(frame[pos] & 0x80)
				print("huffman encoded\n");
			len = frame[pos] & 0x3f;
			print("size: %d index %d\ndecoding..", len, index);
			h_huffmandec(huffmanbuffer, frame, len);
			print("decoded.\ngot: %s\n", huffmanbuffer);
			pos += len + 1;
		}
		/* literal header field without indexing (6.2.2) */
		else if((frame[pos] & 0x00) == 0x00)
		{
			print("literal header field without indexing\n");
			index = frame[pos ] & (0x00 - 1);
			len = frame[pos + 1] & (-1 >> 1);
			print("size: %d\n", len);
			pos += len + 1;
		}
		/* literal header field never indexed (6.2.3) */
		else if (frame[pos] & 0x10)
		{
			print("literal header field never indexed\n");
			index = frame[pos] & (0x10 - 1);
			len = frame[pos + 1] & (-1 >> 1);
			printf("size: %d\n", len);
			pos += len + 1;
		}
	}
	return;
}

/*
 * SETTINGS frame function
 * u_stgsframeresp
 */
int
u_stgsframeresp(HConn* conn, TData* data)
{
	u8int ack;
	ack = conn->rreq.buf[conn->rreq.curpos + 4];
	print("ack bit is 0x%x\n", ack);
	if(ack)
		return 1;
	print("ack not found\n");
	pwrite(data->acfd, &RawHttp2Settings, 45, 0);
	pwrite(data->acfd, &AckSettingsFrame, 9, 0);
	return 0;
}
