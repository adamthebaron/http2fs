#ifndef _THREAD_H_
#define _THREAD_H_

/* stack size of main thread */
extern int mainstacksize;

/* stack size of each thread */
extern int stacksize;

/* max num of threads spawned by libthread */
extern int t_maxcount;

/* thread data */
extern TData Tdata[1024];

/* function that threadmain executes when
 * the server receives a request */
void t_responseproc(void*);

#endif // _THREAD_H_
