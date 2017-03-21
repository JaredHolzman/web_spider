#include "webspider_threadpools.h"

ThreadsafeQueue *WebSpiderThreadPools::tsqueue = new ThreadsafeQueue();

WebSpiderThreadPools::WebSpiderThreadPools(int max_threads)
  : max_threads(max_threads) {}

void WebSpiderThreadPools::crawl_web(std::string root_webpage_address) {
  // Do stuff
  pthread_t workers[max_threads];
  void *status;

  // Create producer/consumer threads
  for (long i = 0; i < max_threads; i++) {
    pthread_create(&workers[i], NULL, crawl_page, (void *)i);
  }
  // Wait for threads to complete
  for (int i = 0; i < max_threads; i++) {
    int rc = pthread_join(workers[i], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Producer: completed join with thread %d having a status of %ld\n",
           i, (long)status);
  }
}

void *WebSpiderThreadPools::crawl_page(void *threadID) {
  tsqueue->append(new std::string("d"));
}
