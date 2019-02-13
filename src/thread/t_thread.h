#ifndef _THREAD_H_
#define _THREAD_H_


/* stack size of main thread */
const static int mainstacksize = 8192;

/* stack size of each thread */
const static int stacksize = 8192;

/* alias */
const static int t_stacksize = 8192;

/* max num of threads spawned by libthread */
const static int t_maxcount = 1024;

/* thread data */
TData Tdata[1024];

/* function that threadmain executes when
 * the server receives a request */
void t_responseproc(void*);

#endif // _THREAD_H_
