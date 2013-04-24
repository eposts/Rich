必须使用函数来对两个线程ID进行比较
#include <pthread.h>
int pthread_equal(pthread_t tid1, pthread_t tid2);          //return 0相等，非0不等

获得自身线程ID
#include <pthread.h>
pthread_t pthread_self(void);

