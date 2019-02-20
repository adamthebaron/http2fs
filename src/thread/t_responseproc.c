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
	u32int tid;

	tid = n = 0;
	data = (TData*) arg;
	initconn(data->conn, tid, 0);
	data->pid = getpid();
	print("got Tdata: %s %s %d %d %d\n", data->adir, data->ldir, data->acfd,
										 data->anfd, data->lnfd);
	print("in proc %d\n", data->pid);
	print("reading from %s and fd %d\n", data->adir, data->acfd);
	//while((n = pread(data->acfd, conn.rreq.buf, MaxBuf, 0)) > 0)
	while((n = read(data->acfd, data->conn->rreq.buf, MaxBuf)) > 0)
	{
		print("read data... ");
		data->conn->rreq.len = n;
		print("got %d bytes\n", data->conn->rreq.len);
		print("raw data: %x\n", data->conn->rreq.buf);
		u_parsereq(data);
		pwrite(data->acfd, data->conn->rresp.buf, data->conn->rresp.len, 0);
	}
}
