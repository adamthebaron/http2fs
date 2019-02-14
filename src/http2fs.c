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
	print("announce\n");
	forever
	{
		lnfd = listen(adir, ldir);
		if(lnfd < 0)
		{
			perror("listen()");
			threadexitsall("threadmain");
		}
		print("listened\n");
		acfd = accept(lnfd, ldir);
		if(acfd < 0)
		{
			perror("accept()");
			threadexitsall("threadmain");
		}
		print("got request\n");
		Tdata[i].acfd = acfd;
		Tdata[i].anfd = anfd;
		Tdata[i].lnfd = lnfd;
		strcpy(Tdata[i].adir, adir);
		strcpy(Tdata[i].ldir, ldir);
		print("Tdata[%d]: %s %s %d %d %d\n", i, Tdata[i].adir, Tdata[i].ldir,
											 Tdata[i].acfd, Tdata[i].anfd,
											 Tdata[i].lnfd);
		proccreate(t_responseproc, &Tdata[i], t_stacksize);
		i = i + 1 % 1024;
	}
	threadexitsall("threadmain");
}
