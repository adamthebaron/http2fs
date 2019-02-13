#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>

#include "http2fs.h"
#include "hpack.h"

/* thread stack sizes used by thread(2) */

/* threadmain */
int mainstacksize = 8192;

/* thread */
int stacksize = 8192;

/* thread data */
TData Tdata[1024];

void
initreq(HReq *req)
{
	req->len = 0;
	req->curpos = 0;
	memset(req->buf, 0, MaxBuf);
	return;
}

void
initresp(HResp *resp)
{
	resp->len = 0;
	resp->curpos = 0;
	memset(resp->buf, 0, MaxBuf);
	return;
}

void
initstream(HStream *s)
{
	s->req = (HReq*) malloc(sizeof(HReq));
	s->resp = (HResp*) malloc(sizeof(HResp));
	s->parent = (HStream*) malloc(sizeof(HStream));
	initreq(s->req);
	initresp(s->resp);
	return;
}

void
initconn(HConn *conn, u32int id, u32int pid)
{
	if(pid==0)
	{
		conn->stream[0] = (HStream*) malloc(sizeof(HStream));
		initstream(conn->stream[0]);
		conn->stream[0]->id = id;
		conn->stream[0]->parent = nil;
	}
	else 
	{
		conn->stream[id] = (HStream*) malloc(sizeof(HStream));
		initstream(conn->stream[id]);
		conn->stream[id]->parent = conn->stream[pid];
	}
	return;
}

void
http2initrespond(HConn *conn)
{
	/* there are two possibilitie here:
	 * 1. the client is sending this as a result of an 
	 *    http/1.x proto upgrade
	 * 2. the client is sending this with prior knowledge
	 *    (3.4)
	 */
	if (1) 
	{
		/* we recieved this with prior knowledge */
	}
	else 
	{
		/* we have not recieved this with prior knowledge */
	}
	return;
}

void
dataframeresp(u8int *frame)
{

}

/* christmas day when your mom got you your first bike */
void
hdrframresp(u8int *frame, uint s)
{
	u8int len, index;
	u8int *hufbuf;
	u8int pos;

	pos = 9;
	hufbuf = malloc(1024 * sizeof(u8int));
	/* skip to 9 in buf to jump over frame header */
	while(pos < s)
	{
		print("checking %x\n", frame[pos]);
		if(0x80 & frame[pos])
		{
			print("indexed header field rep, index: ");
			index = (0x7f & frame[pos]) - 1;
			print("%x\n", index);
			print("header is: %s:%s\n", hpackstatictable[index].name, 
										hpackstatictable[index].val);
			pos++;
		}
		else if(0x40 & frame[pos])
		{
			print("literal header field with incremental indexing\n");
			index = 0x3F & frame[pos++]; //00111111 & frame
			if (!index)
				print("new name\n");
			else
				print("indexed name\n");
			if (0x80 & frame[pos])
				print("huffman encoded\n");
			len = 0x3F & frame[pos];
			print("size: %d, index: %d\ndecoding...", len, index);
			huffmandec(hufbuf, frame, len);
			print("decoded, got: %s\n", hufbuf);
			pos += len + 1;
		}
		else if((0x00 & frame[pos]) == 0x00)
		{
			print("literal header field without indexing\n");
			index = (0x00 - 1) & frame[pos];
			len = (-1 >> 1) & frame[pos + 1];
			printf("size: %d, index: %d\n", len);
			pos += len + 1;
		}
		else if(0x10 & frame[pos]){
			print("literal header field never index\n");
			index = (0x10 - 1) & frame[pos];
			len = (-1 >> 1) & frame[pos + 1];
			printf("size: %d, index: %d\n", len);
			pos += len + 1;
		}
	}
	return;
}

void
priframeresp(HConn *conn)
{

}

void
rstframeresp(HConn *conn)
{

}

int
settingsframeresp(HConn *conn, TData *data)
{
	u8int ack;

	ack = conn->rreq.buf[conn->rreq.curpos + 4];
	print("ack is %x\n", ack);
	if(ack)
	{
		print("ack found, returning immediately.\n");
		return 1; //no need to resend settings if client has ack'ed them
	}
	else
	{
		print("ack not found, accepting client settings and sending server settings\n");
		pwrite(data->acfd, &RawHttp2Settings, 45, 0);
		pwrite(data->acfd, &AckSettingsFrame, 9, 0);
	}
	return 0;
}

void
pushpframeresp(HConn *conn)
{

}

void
pingframeresp(HConn *conn)
{

}

void
goawayframeresp(HConn *conn)
{

}

void
winupframeresp(HConn *conn)
{

}

void
contframeresp(HConn *conn)
{

}

/* parse request type (http1 or http2)
 * and handle frames */
int
parsereq(HConn *conn, TData *data)
{
	u8int frametype;
	u32int curpos, curlen;
	uint framelen;
	u8int *fbuf;

	print("parsing\n");
	fbuf = (u8int*) malloc(MaxBuf * sizeof(u8int));
	framelen = curlen = 0;
	for(curpos=0; curpos<conn->rreq.len; curpos+=framelen)
	{
		print("curpos: %d of %d bytes\n", curpos, conn->rreq.len);
		if(!memcmp(&(conn->rreq.buf[curpos]), Http2ConnPrefix, 3))
		{
			/* request has conn prefix */
			framelen = 24;
			print("found conn prefix\n");
			http2initrespond(conn);
			continue;
		}
		framelen = (conn->rreq.buf[curpos] << 16) | 
				   (conn->rreq.buf[curpos + 1] << 8) |
				   conn->rreq.buf[curpos + 2];
		frametype = conn->rreq.buf[curpos + 3];
		print("reg http2 frame of type %x of size %d (hex: %x)\n", 
			frametype, framelen, framelen);
		print("storing frame in buf...");
		memcpy(fbuf, &(conn->rreq.buf[curpos]), framelen + 9);
		print("ok\n");
		switch(frametype)
		{
			case 0x0:
				/* data frame */
				conn->rreq.curpos = curpos;
				dataframeresp(fbuf);
				break;
			case 0x1:
				/* headers frame */
				conn->rreq.curpos = curpos;
				hdrframresp(fbuf, framelen + 9);
				break;
			case 0x2:
				/* priority frame */
				conn->rreq.curpos = curpos;
				priframeresp(conn);
				break;
			case 0x3:
				/* rststream frame */
				conn->rreq.curpos = curpos;
				rstframeresp(conn);
				break;
			case 0x4:
				/* settings frame */
				conn->rreq.curpos = curpos;
				if(settingsframeresp(conn, data))
					return 1;
				break;
			case 0x5:
				/* push promise frame */
				conn->rreq.curpos = curpos;
				pushpframeresp(conn);
				break;
			case 0x6:
				/* ping frame */
				conn->rreq.curpos = curpos;
				pingframeresp(conn);
				break;
			case 0x7:
				/* goaway frame */
				conn->rreq.curpos = curpos;
				goawayframeresp(conn);
				break;
			case 0x8:
				/* winup frame */
				conn->rreq.curpos = curpos;
				winupframeresp(conn);
				break;
			case 0x9:
				/* cont frame */
				conn->rreq.curpos = curpos;
				contframeresp(conn);
				break;
		}
		curpos += 9;
		print("curpos now %d\n", curpos);
	}
	return 0;
}

/* passed to threads as fn arg 
 * resp is raw data sent from client */
void
respproc(void* arg) 
{
	int n;
	TData* data;
	HSettings clientsettings;
	HConn conn;
	u32int rid;

	rid = 0;
	initconn(&conn, rid, 0);
	data = arg;
	data->pid = getpid();
	print("in proc %d, parent is %d\n", data->pid);
	n = 0;
	while((n=pread(data->acfd, conn.rreq.buf, MaxBuf, 0))>0)
	{
		conn.rreq.len = n;
		print("got %d bytes\n", conn.rreq.len);
		parsereq(&conn, data);
		//pwrite(data->acfd, conn.rresp.buf, conn.rresp.len, 0);
	}
		
}

void
threadmain(int argc, char **argv) 
{
	int acfd, lnfd, anfd, i;
	char adir[64], ldir[64];

	gentree();
	i = 0;
	anfd = announce("tcp!*!80", adir);
	if(anfd<0)
	{
		perror("announce()");
		exits(0);
	}
	for(;;)
	{
		lnfd = listen(adir, ldir);
		if(lnfd<0)
		{
			perror("listen()");
			exits(0);
		}
		acfd = accept(lnfd, ldir);
		if(acfd<0)
		{
			perror("accept()");
			exits(0);
		}
		Tdata[i].acfd = acfd;
		Tdata[i].anfd = anfd;
		Tdata[i].lnfd = lnfd;
		strcpy(Tdata[i].adir, adir);
		strcpy(Tdata[i].ldir, ldir);
		proccreate(respproc, &Tdata[i], stacksize);
	}
	exits(0);
}
