#include "../http2fs.h"
#include "u_util.h"
#include "../hpack/h_hpack.h"

void
u_printframe(u8int* framebuf, u64int framelen)
{
	print("raw frame of type %x: \n", framebuf[4]);
	for(u64int i = 0; i < framelen; i++)
		print("%x ", framebuf[i]);
	print("\n");
	return;
}

/*
 * HEADER frame function
 * u_hdrframeresp
 */
void
u_hdrframeresp(u8int* framebuf, u64int framelen, uint fd)
{
	u8int len, index, pos;
	u8int* huffmanbuffer;
	u8int* decodebuf;

	pos = 9;
	huffmanbuffer = calloc(1024, sizeof(u8int));
	decodebuf = calloc(1024, sizeof(u8int));
	u_printframe(framebuf, framelen);
	while(pos < framelen)
	{
		print("checking 0x%x\n", framebuf[pos]);
		/* indexed header field rep (6.1) */
		if(framebuf[pos] & 0x80)
		{
			print("indexed header field, index: ");
			index = (framebuf[pos] & 0x7f) - 1;
			print("0x%x\n", index);
			print("header is: %s:%s\n", hpackstatictable[index].name,
										hpackstatictable[index].val);
			pos++;
		}
		/* literal header field with incremental indexing (6.2.1) */
		else if(framebuf[pos] & 0x40)
		{
			print("literal header field with inc indexing\n");
			index = 0x3f & framebuf[pos];
			if(index == 0x0)
				print("new header\n");
			else
				print("indexed header\n");
			if(framebuf[pos + 1] & 0x80)
				print("huffman encoded\n");
			len = framebuf[pos + 1] & 0x3f;
			print("size: %d index %d\n", len, index);
			/* bitwise AND this byte with the msb unset
			 * that bit is used to know if the header is
			 * huffman encoded or not */
			framebuf[pos + 1] &= 0x7f;
			/* TODO: need to shift everything in huffmanbuffer
			 * by one so as to not count msb in first byte
			 * during binary tree traversal */
			memcpy(huffmanbuffer, &(framebuf[pos + 1]), len);
			print("huffmanbuffer contains: ");
			for(u64int i = 0; i < len; i++)
				print("%x ", huffmanbuffer[i]);
			print("\ndecoding... ");
			h_huffmandec(decodebuf, huffmanbuffer, len);
			print("decoded.\ngot: %s\n", decodebuf);
			pos += len + 1;
		}
		/* literal header field without indexing (6.2.2) */
		else if((framebuf[pos] & 0x00) == 0x00)
		{
			print("literal header field without indexing\n");
			index = framebuf[pos ] & (0x00 - 1);
			len = framebuf[pos + 1] & (-1 >> 1);
			print("size: %d\n", len);
			pos += len + 1;
		}
		/* literal header field never indexed (6.2.3) */
		else if (framebuf[pos] & 0x10)
		{
			print("literal header field never indexed\n");
			index = framebuf[pos] & (0x10 - 1);
			len = framebuf[pos + 1] & (-1 >> 1);
			printf("size: %d\n", len);
			pos += len + 1;
		}
	}
	free(huffmanbuffer);
	free(decodebuf);
	return;
}

/*
 * SETTINGS frame function
 * u_stgsframeresp
 */
void
u_stgsframeresp(u8int* framebuf, u64int framelen, uint fd)
{
	u8int ack;

	u_printframe(framebuf, framelen);
	ack = framebuf[framelen + 4];
	print("ack bit is 0x%x\n", ack);
	if(ack)
		return;
	print("ack not found\n");
	write(fd, &RawHttp2Settings, sizeof(RawHttp2Settings));
	write(fd, &AckSettingsFrame, sizeof(AckSettingsFrame));
	return;
}
