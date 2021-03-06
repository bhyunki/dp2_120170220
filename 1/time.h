/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _TIME_H_RPCGEN
#define _TIME_H_RPCGEN

#include <rpc/rpc.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#define Time_PROG 0x31234567
#define Time_Vers 1

#if defined(__STDC__) || defined(__cplusplus)
#define Get_time 1
extern  int * get_time_1(void *, CLIENT *);
extern  int * get_time_1_svc(void *, struct svc_req *);
#define Delay_time 2
extern  void * delay_time_1(int *, CLIENT *);
extern  void * delay_time_1_svc(int *, struct svc_req *);
extern int time_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define Get_time 1
extern  int * get_time_1();
extern  int * get_time_1_svc();
#define Delay_time 2
extern  void * delay_time_1();
extern  void * delay_time_1_svc();
extern int time_prog_1_freeresult ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

struct timeval global;
int tod;
//pthread_mutex_t mutex;

#endif /* !_TIME_H_RPCGEN */
