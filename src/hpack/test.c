#include <u.h>
#include <libc.h>

#include "../http2fs.h"
#include "hpack.h"

void
main(int argc, char** argv)
{
	char* str = "lemonade was a popular drink and it still is";
	char* decodedstr;
	u8int* encodedstr;
	u8int replen;

	inittree();
	encodedstr = (u8int*) malloc(20 * sizeof(u8int));
	decodedstr = (char*) malloc(strlen(str) * sizeof(char));
	print("encoding %s\n", str);
	huffmanenc((u8int*) str, encodedstr, strlen(str), &replen);
	print("got ");
	for(int i = 0; i < replen; i++)
		print("%x ", encodedstr[i]);
	print("\n");
	print("decoding...");
	huffmandec((u8int*) decodedstr, encodedstr, replen);
	print("got %s\n", (char*) decodedstr);
	//free(encodedstr);
	//free(decodedstr);
	//freenode(rootnode);
	exits(0);
}