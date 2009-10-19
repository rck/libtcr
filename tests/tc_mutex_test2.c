#include "threaded_cr.h"
#include <unistd.h>
#include <sys/epoll.h>

static struct tc_mutex m;
static int in_cr = 0;
static int worker_no = 0;

void worker(void *unused)
{
	int this_worker_no = ++worker_no;
	int i;

	fprintf(stdout, "worker %d started.\n", this_worker_no);
	for (i=0;i<10;i++) {
		tc_mutex_lock(&m);
//		fprintf(stderr, "doing something.\n");
		tc_mutex_unlock(&m);
//		printf("progress on worker %d\n", this_worker_no);
	}
}

void starter(void *unused)
{
	struct tc_threads t;

	while (1) {
		tc_mutex_init(&m);
		fprintf(stdout, "beginning starter.\n");
		tc_threads_new(&t, worker, NULL, "worker %d");
		tc_threads_wait(&t);
		fprintf(stdout, "ending starter.\n");
	}
}


int main()
{
	tc_run(starter, NULL, "test", 10);
	return 0;
}