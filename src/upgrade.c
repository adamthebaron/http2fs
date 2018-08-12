#include <u.h>
#include <libc.h>

#include "http2fs.h"

/* pushtls(2) if you get to it */

void
handleh2() {
}

HResp*
handleh2c() {
	HResp *resp;

	resp = malloc(sizeof(HResp));
	resp->headers[0] = malloc(sizeof(HHeader));
	resp->headers[1] = malloc(sizeof(HHeader));
	resp->headercount = 0;
	resp->status = strcat(HStatuscodes[HSwitchProto], "\n");
	strcpy(resp->headers[0]->name, "Connection: ");
	strcpy(resp->headers[0]->val, "Upgrade\n");
	strcpy(resp->headers[1]->name, "Upgrade: ");
	strcpy(resp->headers[1]->val, "h2c\n");
	resp->headercount += 2;
	resp->data = " ";
	return resp;
}