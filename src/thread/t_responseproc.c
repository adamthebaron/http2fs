#include "../http2fs.h"
#include "t_thread.h"

/*
 * t_responseproc
 */
void
t_responseproc(void* arg)
{
	int n;
	TData* data;
	HSettings hsettings;
	HConn conn;
	u32int tid;

	tid = n = 0;
	initconn(&conn, tid, 0);
	data = (TData*) arg;
	data->pid = getpid();
	print("in proc %d\n", data->pid);
	while((n = pread(data->acfd, conn.rreq.buf, MaxBuf, 0)) > 0)
	{
		conn.rreq.len = n;
		print("got %d bytes\n", conn.rreq.len);
		parsereq(&conn, data);
		pwrite(data->acfd, conn.rresp.buf, conn.rresp.len, 0);
	}
}
