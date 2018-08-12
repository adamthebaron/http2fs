#include <u.h>
#include <libc.h>
#include <auth.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>

#include "../http2fs.h"
#include "hpack.h"

static char Ebad[] = "something bad happened but idk what it was";
static char Enomem[] = "no memory";

typedef struct HuffmanEncd HuffmanEncd;
struct HuffmanEncd {
	u8int *rep;
	uint len;
};

typedef struct HuffmanDecd HuffmanDecd;
struct HuffmanDecd {
	u8int *rep;
	uint len;
};

void
fsread(Req *r)
{
	HuffmanEncd* he;
	HuffmanDecd hd;
	he = (HuffmanEncd*) r->fid->file->aux;

	hd.rep = (u8int*) malloc(12 * sizeof(u8int));
	print("read ");
	for(int i = 0; i < he->len; i++)
		print("%x ", he->rep[i]);
	print("of size %d\n", he->len);
	huffmandec(hd.rep, he->rep, he->len);
	r.ofcall->aux = he;
	respond(r, nil);
	free(he);
	return;
}

void
fswrite(Req *r)
{
	HuffmanEncd *he;
	HuffmanDecd hd;

	he = (HuffmanEncd*) r->fid->file->aux;
	huffmanenc(he->rep, hd.rep, he->len, hd.len);
	r->ofcall->aux = he;
	respond(r, nil);
	free(he);
	return;
}

void
fsopen(Req *r)
{
	/*HuffmanFile *rf;

	rf = r->fid->file->aux;

	if(rf && (r->ifcall.mode&OTRUNC)){
		rf->ndata = 0;
		r->fid->file->length = 0;
	}

	respond(r, nil);*/
}

Srv fs = {
	//.open  = fsopen,
	.read  = fsread,
	.write = fswrite,
};

void
usage(void)
{
	fprint(2, "usage: ramfs [-D] [-s srvname] [-m mtpt]\n");
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *addr = nil;
	char *srvname = nil;
	char *mtpt = nil;
	Qid q;

	fs.tree = alloctree(nil, nil, DMDIR|0777, nil);
	q = fs.tree->root->qid;

	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 'a':
		addr = EARGF(usage());
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	case 'm':
		mtpt = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND;

	if(argc)
		usage();

	if(chatty9p)
		fprint(2, "ramsrv.nopipe %d srvname %s mtpt %s\n", fs.nopipe, srvname, mtpt);
	if(addr == nil && srvname == nil && mtpt == nil)
		sysfatal("must specify -a, -s, or -m option");
	if(addr)
		listensrv(&fs, addr);

	if(srvname || mtpt)
		postmountsrv(&fs, srvname, mtpt, MREPL|MCREATE);
	exits(0);
}
