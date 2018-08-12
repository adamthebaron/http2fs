#include <u.h>
#include <libc.h>

#include "http2fs.h"

HResp*
sendupgradereq() {
	HResp *resp;

	resp = malloc(sizeof(HResp));
	strcpy(resp->status, "HTTP/1.1 426 Upgrade Required\n");
	resp->headers[0] = malloc(sizeof(HHeader));
	resp->headers[1] = malloc(sizeof(HHeader));
	resp->headercount = 0;
	//resp->headers[2] = malloc(sizeof(HHeader));
	//resp->headers[3] = malloc(sizeof(HHeader));
	strcpy(resp->headers[0]->name, "Upgrade: ");
	strcpy(resp->headers[0]->val, "HTTP/2.0\n");
	strcpy(resp->headers[1]->name, "Connection: ");
	strcpy(resp->headers[1]->val, "Upgrade\n");
	resp->headercount += 2;
	return resp;
}

void
parsereq(HReq *req, HResp *resp) {
	/* check to see if request is upgrade before doing anything else */
	for(int i = 0; i < req->headercount; i++) {
		print("headercount: %d\n", req->headercount);
		print("inside for loop of parsereq\n");
		print("testing header:\n%s %s\n", req->headers[i]->name, req->headers[i]->val);
		if((strcmp(req->headers[i]->name, "Connection:") == 0) && 
		   (strcmp(req->headers[i]->val, "Upgrade") == 0)) {
			resp = handleh2c();
			print("returning handleh2c()\n");
			return;
		}
	}
	resp = sendupgradereq();
	print("returning sendupgradereq()\n");
	return;
}