#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "http2fs.h"

void
initmethod(HMethod *method, char* smethod) {
	print("2\nsmethod = %s", smethod);

	strcpy(method->method, strtok(smethod, " "));
	print("method->method = %s\n", method->method);
	strcpy(method->url, strtok(0, " "));
	print("method->url = %s\n", method->url);
	strcpy(method->version, strtok(0, " "));
	print("method->version = %s\n", method->version);
}

void
createreq(HReq *req, char *buf) {
	char lines[HTTP2_MAX_HEADERS][HTTP2_MAX_HEADER_SIZE];
	char curline[HTTP2_MAX_HEADER_SIZE];
	int i, j;
	for(int h = 0; h < HTTP2_MAX_HEADERS; h++)
		memset(lines[h], 0, HTTP2_MAX_HEADER_SIZE);
	j = 0;
	while(*buf) {
		i = 0;
		while((curline[i++] = *buf++) != '\n')
			;
		print("%s\n", curline);
		strcpy(lines[j], curline);
		memset(curline, 0, sizeof(curline));
		j++;
	}
	print("1 method = %s\n", lines[0]);
	initmethod(req->method, lines[0]);
	print("3\n");
	for(int h = 1; lines[h][0]; h++) {
		print("h = %d\n", h);
		req->headers[h - 1] = malloc(sizeof(HHeader));
		strcpy(req->headers[h - 1]->name, strtok(lines[h], " "));
		strcpy(req->headers[h - 1]->val, strtok(0, "\n"));
		req->headercount += 1;
		print("header: %s %s\n", req->headers[h - 1]->name, req->headers[h - 1]->val);
	}
}

char*
prepareresp(HResp *resp) {
	char *response;
	int offset, i;

	response = malloc(HTTP2_MAX_BUF);
	i = 0;
	offset = strlen(resp->status);
	strcpy(response, resp->status);
	while( --(resp->headercount) != -1) {
		strcpy(response + offset, resp->headers[resp->headercount]->name);
		offset += strlen(resp->headers[resp->headercount]->name);
		strcpy(response + offset, resp->headers[resp->headercount]->val);
		offset += strlen(resp->headers[resp->headercount]->val);
	}
	strcpy(response + offset, resp->data);
	strcpy(response + offset + 1, "\n");
	return response;
}

void
main(int argc, char *argv[]) {
	int afd, dfd, lcfd, n;
	char adir[128], ldir[128], buf[HTTP2_MAX_BUF], *resp;

	afd = announce("tcp!coltrane.gnot.pluvi.us!http", adir);
	if(afd < 0) {
		close(afd);
		exits(0);
	}

	for(;;) {
		HConn *hconn;

		hconn = malloc(sizeof(HConn));
		hconn->req = malloc(sizeof(HReq));
		hconn->resp = malloc(sizeof(HResp));
		hconn->req->method = malloc(sizeof(HMethod));
		resp = malloc(HTTP2_MAX_BUF * sizeof(char));

		lcfd = listen(adir, ldir);
		if(lcfd < 0) {
			perror("main():");
			close(lcfd);
			continue;
		}
		print("lcfd = %d, adir = %s, ldir = %s, afd = %d\n", lcfd, adir, ldir, afd);

		switch(rfork(RFPROC|RFNOWAIT|RFNAMEG|RFENVG|RFFDG)) {
			case -1:
				perror("fork():");
				close(lcfd);
				continue;
			case 0:
				
				dfd = accept(lcfd, ldir);
				if(dfd < 0)
					exits(0);

				memset(ldir, 0, 128 * sizeof(char));
				memset(adir, 0, 128 * sizeof(char));
				while((n = readn(dfd, buf, HTTP2_MAX_BUF)) > 0) {
					print("RECEIVED\n%s\ncalling createreq\n", buf);
					createreq(hconn->req, buf);
					print("printing created req\n");
					print("method = %s\n%s\n%s\nhost = %s\n", hconn->req->method->method, 
																			hconn->req->method->url,
																			hconn->req->method->version,
																			hconn->req->host);
					for(int i = 0; i < hconn->req->headercount; i++)
						print("header[%d]: %s %s\n", i, hconn->req->headers[i]->name, hconn->req->headers[i]->val);
					print("done printing req\n");
					parsereq(hconn->req, hconn->resp);
					resp = prepareresp(hconn->resp);
					print("SENDING\n%s", resp);
					write(dfd, resp, HTTP2_MAX_BUF);
				}
				free(resp);
				free(hconn->resp);
				free(hconn->req);
				free(hconn);
				//exits(0);
			default:
				close(lcfd);
				break;
		}
	}
}
