TQDIR = threadsafe_queue
WSDIR = webpage_scraper
WTDIR = webspider_threads
WPDIR = webspider_threadpools

CC = g++
CPPFLAGS = `pkg-config gumbo libcurl --cflags`
CPPLIBS =  -pthread -lPocoFoundation `pkg-config gumbo libcurl --libs`
ALLFLAGS = -std=c++11 -Wall -Wextra -g $(CPPFLAGS) $(CPPLIBS)
OBJS = threadsafe_exqueue.o threadsafe_queue.o webpage_scraper.o webspider_threadpools.o webspider_threads.o

all: main experiments

experiments: experiments.cc $(OBJS)
	$(CC) $(OBJS) experiments.cc -o experiments $(ALLFLAGS)

main: main.cc $(OBJS)
	$(CC) $(OBJS) main.cc -o spider $(ALLFLAGS)

webspider_threadpools.o: $(WPDIR)/webspider_threadpools.cc $(WPDIR)/webspider_threadpools.h
	$(CC) -c $(WPDIR)/webspider_threadpools.cc $(ALLFLAGS)

webspider_threads.o: $(WTDIR)/webspider_threads.cc $(WTDIR)/webspider_threads.h
	$(CC) -c $(WTDIR)/webspider_threads.cc $(ALLFLAGS)

threadsafe_exqueue.o: $(TQDIR)/threadsafe_exqueue.cc $(TQDIR)/threadsafe_exqueue.h
	$(CC) -c $(TQDIR)/threadsafe_exqueue.cc $(ALLFLAGS)

threadsafe_queue.o: $(TQDIR)/threadsafe_queue.cc $(TQDIR)/threadsafe_queue.h
	$(CC) -c $(TQDIR)/threadsafe_queue.cc $(ALLFLAGS)

webpage_scraper.o: $(WSDIR)/webpage_scraper.cc $(WSDIR)/webpage_scraper.h
	$(CC) -c $(WSDIR)/webpage_scraper.cc $(ALLFLAGS)

clean:
	rm -f *.o
	rm -f spider
	rm -f experiments
