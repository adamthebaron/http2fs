#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>

#include "http2fs.h"
#include "hpack/h_hpack.h"
#include "util/u_util.h"
#include "thread/t_thread.h"

TData Tdata[1024];
int stacksize = 8192;
int mainstacksize = 8192;
int t_threadcount = 1024;
#define t_stacksize stacksize
extern void t_responseproc(void*);
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
	for(curpos = 0; curpos < conn->rreq.len; curpos += framelen)
	{
		print("curpos: %d of %d bytes\n", curpos, conn->rreq.len);
		if(!memcmp(&(conn->rreq.buf[curpos]), Http2ConnPrefix, 3))
		{
			/* request has conn prefix */
			framelen = 24;
			print("found conn prefix\n");
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
				//u_dataframeresp(fbuf);
				break;
			case 0x1:
				/* headers frame */
				conn->rreq.curpos = curpos;
				u_hdrframeresp(fbuf, framelen + 9);
				break;
			case 0x2:
				/* priority frame */
				conn->rreq.curpos = curpos;
				//u_priframeresp(conn);
				break;
			case 0x3:
				/* rststream frame */
				conn->rreq.curpos = curpos;
				//u_rstframeresp(conn);
				break;
			case 0x4:
				/* settings frame */
				conn->rreq.curpos = curpos;
				if(u_stgsframeresp(conn, data))
					return 1;
				break;
			case 0x5:
				/* push promise frame */
				conn->rreq.curpos = curpos;
				//u_pushpframeresp(conn);
				break;
			case 0x6:
				/* ping frame */
				conn->rreq.curpos = curpos;
				//u_pingframeresp(conn);
				break;
			case 0x7:
				/* goaway frame */
				conn->rreq.curpos = curpos;
				//u_goawayframeresp(conn);
				break;
			case 0x8:
				/* winup frame */
				conn->rreq.curpos = curpos;
				//u_winupframeresp(conn);
				break;
			case 0x9:
				/* cont frame */
				conn->rreq.curpos = curpos;
				//u_contframeresp(conn);
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

	h_inittree();
	i = 0;
	anfd = announce("tcp!*!80", adir);
	if(anfd < 0)
	{
		perror("announce()");
		threadexitsall("threadmain");
	}
	forever
	{
		lnfd = listen(adir, ldir);
		if(lnfd < 0)
		{
			perror("listen()");
			threadexitsall("threadmain");
		}
		acfd = accept(lnfd, ldir);
		if(acfd < 0)
		{
			perror("accept()");
			threadexitsall("threadmain");
		}
		Tdata[i].acfd = acfd;
		Tdata[i].anfd = anfd;
		Tdata[i].lnfd = lnfd;
		strcpy(Tdata[i].adir, adir);
		strcpy(Tdata[i].ldir, ldir);
		proccreate(t_responseproc, &Tdata[i], t_stacksize);
	}
	threadexitsall("threadmain");
	//threadexits(0);
}
