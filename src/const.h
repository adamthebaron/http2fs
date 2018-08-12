#include <u.h>

/* 49 characters */
const uchar* Upgradereq = "HTTP/1.1 426 Upgrade Required\r\nServer: http2fs\r\n\r\n"

/* 71 characters */
const uchar* Upgradeh2c = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: h2c\r\n\r\n"