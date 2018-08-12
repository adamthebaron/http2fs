#include <u.h>
#include <libc.h>

#include "../http2fs.h"

HFunc *headerfuncs[12];

HFunc hacceptfunc = {
	nil,
	"Accept",
	nil,
	&haccept
};

HFunc hacceptcharsetfunc = {
	&hacceptfunc,
	"Accept-Charset",
	nil,
	&hacceptcharset
};

HFunc hacceptencodingfunc = {
	&hacceptcharsetfunc,
	"Accept-Encoding",
	nil,
	&hacceptencoding
};

HFunc hacceptlanguagefunc = {
	&hacceptencodingfunc,
	"Accept-Language",
	nil,
	&hacceptlanguage
};

HFunc hacceptdatetimefunc = {
	&hacceptlanguagefunc,
	"Accept-Datetime",
	nil,
	&hacceptdatetime
};

HFunc hcachecontrolfunc = {
	&hacceptdatetimefunc,
	"Cache-Control",
	nil,
	&hcachecontrol
};

HFunc hconnectionfunc = {
	&hcachecontrolfunc,
	"Connection",
	nil,
	&hconnection
};

HFunc hcontentlengthfunc = {
	&hconnectionfunc,
	"Content-Length",
	nil,
	&hcontentlength
};

HFunc hcontenttypefunc = {
	&hcontentlengthfunc,
	"Content-Type",
	nil,
	&hcontenttype
};

HFunc hdatefunc = {
	&hcontenttypefunc,
	"Date",
	nil,
	&hdate
};

HFunc hhostfunc = {
	&hdatefunc,
	"Host",
	nil,
	&hhost
};

HFunc hifmodifiedsincefunc = {
	&hhostfunc,
	"If-Modified-Since",
	nil,
	&hifmodifiedsince
};

void
initheaderfuncs(void) {
	for(int i = 0; i < 12; i++)
		headerfuncs[i] = malloc(sizeof(HFunc));
	headerfuncs[0] = &hacceptfunc;
	headerfuncs[1] = &hacceptcharsetfunc;
	headerfuncs[2] = &hacceptencodingfunc;
	headerfuncs[3] = &hacceptlanguagefunc;
	headerfuncs[4] = &hacceptdatetimefunc;
	headerfuncs[5] = &hcachecontrolfunc;
	headerfuncs[6] = &hconnectionfunc;
	headerfuncs[7] = &hcontentlengthfunc;
	headerfuncs[8] = &hcontenttypefunc;
	headerfuncs[9] = &hdatefunc;
	headerfuncs[10] = &hhostfunc;
	headerfuncs[11] = &hifmodifiedsincefunc;
	return;
}

uint
hash(char *s) {
	uint i;

	for(i = 0; *s != '\0'; s++)
		i = *s + 31 * i;
	return i % 19;
}

HFunc*
search(char *s) {
	HFunc *node;

	for(node = headerfuncs[hash(s)]; node != nil; node = node->next)
		if(strcmp(s, node->header) == 0)
			return node;
	return nil;
}

HFunc*
install(char *name, char *defn) {
	HFunc *node;
	uint i;

	if((node = search(name)) == nil) {
		node = malloc(sizeof(HFunc));
		if(node == nil || (node->header = strdup(name)) == nil)
			return nil;
		i = hash(name);
		node->next = headerfuncs[i];
		headerfuncs[i] = node;
	}
	else
		free(node->defn);
	if((node->defn = strdup(defn)) == nil)
		return nil;
	return node;
}