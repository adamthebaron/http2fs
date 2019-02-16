#ifndef _HTTPFS_H_
#define _HTTPFS_H_

#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>
#include <bio.h>

/* it looks cool */
#define forever for(;;)

/* max size of http buffer in bytes (64kb) */
#define MaxBuf			64 * 1024
/* total number of headers allowed per req */
#define MaxHeaders		64
/* total size in bytes of a single header */
#define MaxHeaderSize	2048

/* hardcoded http response to go from http/1 to http/2 */
extern const char Upgradereq[];
extern const char Upgradeh2c[];

/* crlf needed for http1 */
static u8int CRLF[2] = {
	0x0d, 0x0a,
};

/* "GET" */
static u8int GET[3] = {
	0x47, 0x45, 0x54,
};

/* magic connection prefix
 * 0x505249202a20485454502f322e300d0a0d0a534d0d0a0d0a */
static u8int Http2ConnPrefix[24] = {
	0x50, 0x52, 0x49, 0x20, 0x2a, 0x20, 0x48,
	0x54, 0x54, 0x50, 0x2f, 0x32, 0x2e, 0x30,
	0x0d, 0x0a, 0x0d, 0x0a, 0x53, 0x4d, 0x0d,
	0x0a, 0x0d, 0x0a,
};

/* test SETTINGS frame */
static u8int RawHttp2SettingsFrame[9] = {
	0x00, 0x00, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00,
};

/* test ACK frame */
static u8int AckSettingsFrame[9] = {
	0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00,
};

static u8int RawHttp2Settings[45] = {
	0x00, 0x00, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x00, 0x00, 0x00, 0x20,
	0x00, 0x04, 0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x05, 0x00, 0x00, 0x40, 0x00,
	0x00, 0x06, 0x00, 0x00, 0x00, 0x40,
};

typedef struct HMethod HMethod;
typedef struct HHeader HHeader;
typedef struct HStream HStream;
typedef struct HMsg HMsg;
typedef struct HConn HConn;
typedef struct HSettings HSettings;
typedef void (*framefunc) (u8int* framebuf, u64int framelen, uint fd);

/* thread data
 * acfd, anfd, lnfd: file descriptors for tcp connection
 * pid: proc id
 * adir[], ldir[]: tcp connection on filesystem */
typedef struct TData TData;
struct TData {
	int acfd, anfd, lnfd, pid;
	char adir[64], ldir[64];
};

/* HStatusCodes indexes */
enum statuscodes {
	HContinue,
	HSwitchProto,
	HOk,
	HCreated,
	HAccepted,
	HNonAuthInfo,
	HNoContent,
	HResetContent,
	HPartialContent,
	HMultiChoice,
	HMovedPerm,
	HFound,
	HSeeOther,
	HNotModified,
	HUseProxy,
	HTribe,
	HTempRedir,
	HBadReq,
	HUnauth,
	HPayRequired,
	HForbidden,
	HNotFound,
	HMethNotAllowed,
	HNotAcceptable,
	HProxyAuthRequired,
	HMsgTimeout,
	HConflict,
	HGone,
	HLengthRequired,
	HPrecondFailed,
	HMsgEntityTooLarge,
	HMsgURITooLong,
	HUnsupportedMediaType,
	HMsgRangeNotSatisfiable,
	HExpectationFailed,
	HInternalServerError,
	HNotImplemented,
	HBadGateway,
	HServUnavailable,
	HGatewayTimeout,
	HVerNotSupported
};

/* should we delete these? */
static char* HStatuscodes[] = {
	[HContinue]					"100 Continue",
	[HSwitchProto]				"101 Switching Protocols",
	[HOk]						"200 OK",
	[HCreated]					"201 Created",
	[HAccepted]					"202 Accepted",
	[HNonAuthInfo]				"203 Non-Authoritative Information",
	[HNoContent]				"204 No Content",
	[HResetContent]				"205 Reset Content",
	[HPartialContent]			"206 Partial Content",
	[HMultiChoice]				"300 Multiple Choices",
	[HMovedPerm]				"301 Moved Permanently",
	[HFound]					"302 Found",
	[HSeeOther]					"303 See Other",
	[HNotModified]				"304 Not Modified",
	[HUseProxy]					"305 Use Proxy",
	[HTribe]					"306 Tell Your Mother Tell Your Father Send A Telegram",
	[HTempRedir]				"307 Temporary Redirect",
	[HBadReq]					"400 Bad Request",
	[HUnauth]					"401 Unauthorized",
	[HPayRequired]				"402 Payment Required",
	[HForbidden]				"403 Forbidden",
	[HNotFound]					"404 Not Found",
	[HMethNotAllowed]			"405 Method Not Allowed",
	[HNotAcceptable]			"406 Not Acceptable",
	[HProxyAuthRequired]		"407 Proxy Authentication Required",
	[HMsgTimeout]				"408 Request Timeout",
	[HConflict]					"409 Conflict",
	[HGone]						"410 Gone",
	[HLengthRequired]			"411 Length Required",
	[HPrecondFailed]			"412 Precondition Failed",
	[HMsgEntityTooLarge]		"413 Request Entity Too Large",
	[HMsgURITooLong]			"414 Request-URI Too Long",
	[HUnsupportedMediaType]		"415 Unsupported Media Type",
	[HMsgRangeNotSatisfiable]	"416 Requested Range Not Satisfiable",
	[HExpectationFailed]		"417 Expectation Failed",
	[HInternalServerError]		"500 Internal Server Error",
	[HNotImplemented]			"501 Not Implemented",
	[HBadGateway]				"502 Bad Gateway",
	[HServUnavailable]			"503 Service Unavailable",
	[HGatewayTimeout]			"504 Gateway Timeout",
	[HVerNotSupported]			"505 HTTP Version Not Supported",
};

/* frame definitions */
enum frames {
	/* 0000 */ Data =			0x0,
	/* 0001 */ Headers =		0x1,
	/* 0010 */ Priority =		0x2,
	/* 0011 */ RstStream =		0x3,
	/* 0100 */ Settings =		0x4,
	/* 0101 */ PushPromise =	0x5,
	/* 1000 */ Ping =			0x6,
	/* 1001 */ Goaway =			0x7,
	/* 1010 */ WindowUpdate =	0x8
};

/* SETTINGS frame parameters */
enum http2settings {
	/* 0001 */ HeaderTableSize =		0x1,
	/* 0010 */ EnablePush =				0x2,
	/* 0011 */ MaxConcurrentStreams =	0x3,
	/* 0100 */ InitialWindowSize =		0x4,
	/* 0101 */ MaxFrameSize =			0x5,
	/* 1000 */ MaxHeaderListSize =		0x6,
};

/* method data structure */
struct HMethod {
	char method[8];	/* "verb" of method */
	char url[256];		/* location of document requested */
	char version[8];	/* http version */
};

/* header data structure */
struct HHeader {
	char name[64]; /* name of header (Content-Type, Upgrade, etc) */
	char val[256]; /* value of respective header */
};

/* http message data structure */
struct HMsg {
	u64int len;
	u64int curpos;
	u8int buf[MaxBuf];
};

/* http/2 stream structure */
struct HStream {
	u64int id;
	HMsg *resp;
	HMsg *req;
	HStream *parent;
	HStream *child[32];
};

/* connection state */
struct HConn {
	HMsg rreq;
	HMsg rresp;
	HStream *stream[256];
	//NetConnInfo *conninfo;	/* network info about connection */
};

/* SETTINGS frame */
struct HSettings {
	u16int id[6];
	u32int val[6];
};

/* default HTTP/2 settings */
static HSettings defaultsettings[6] = {
	{HeaderTableSize,		128},
	{EnablePush,			0},
	{MaxConcurrentStreams,	32},
	{InitialWindowSize,		65535},
	{MaxFrameSize,			16384},
	{MaxHeaderListSize,		64},
};

char* getword(char*);
void main(int, char*[]);
void createreq(HMsg*, uchar*);
int u_submem(u8int*, int, u8int*, int);
int parsereq(HConn*, TData*);
void initreq(HMsg*);
void initresp(HMsg*);
void initstream(HStream *s);

#endif /* _HTTPFS_H_ */
