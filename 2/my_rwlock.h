#include <pthread.h>

void my_rdlock(pthread_mutex_t *mutex, pthread_mutex_t *cv, unsigned int *cnt){
	pthread_mutex_lock(mutex);
	(*cnt)++;
	if(*cnt == 1){
//		printf("read cond wait\n");
		pthread_mutex_lock(cv);
	}
//	printf("READ %d\n", *cnt);
	pthread_mutex_unlock(mutex);
}

void my_wrlock(pthread_mutex_t *mutex, pthread_mutex_t *cv){
	pthread_mutex_lock(cv);
}

void my_rdunlock(pthread_mutex_t *mutex, pthread_mutex_t *cv, unsigned int *cnt){
	pthread_mutex_lock(mutex);
	(*cnt)--;
	if(*cnt == 0)
		pthread_mutex_unlock(cv);
	pthread_mutex_unlock(mutex);
}
	
void my_wrunlock(pthread_mutex_t *mutex, pthread_mutex_t *cv){
	pthread_mutex_unlock(cv);
}
