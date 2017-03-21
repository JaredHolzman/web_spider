TQDIR = threadsafe_queue
WSDIR = webpage_scraper
WTDIR = webspider_threads
WPDIR = webspider_threadpools

CC = g++
CFLAGS = -std=c++11 -lboost_system -lboost_thread -lpthread -lgumbo
OBJS = page.o threadsafe_queue.o webpage_scraper.o webspider_threadpools.o webspider_threads.o

all: main

main: main.cc $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) main.cc -o spider

webspider_threadpools.o: $(WPDIR)/webspider_threadpools.cc $(WPDIR)/webspider_threadpools.h
	$(CC) $(CFLAGS) -c $(WPDIR)/webspider_threadpools.cc

webspider_threads.o: $(WTDIR)/webspider_threads.cc $(WTDIR)/webspider_threads.h
	$(CC) $(CFLAGS) -c $(WTDIR)/webspider_threads.cc

threadsafe_queue.o: $(TQDIR)/threadsafe_queue.cc $(TQDIR)/threadsafe_queue.h
	$(CC) $(CFLAGS) -c $(TQDIR)/threadsafe_queue.cc

page.o: $(TQDIR)/page.cc $(TQDIR)/page.h
	$(CC) $(CFLAGS) -c $(TQDIR)/page.cc

webpage_scraper.o: $(WSDIR)/webpage_scraper.cc $(WSDIR)/webpage_scraper.h
	$(CC) $(CFLAGS) -c $(WSDIR)/webpage_scraper.cc

clean:
	rm -f *.o
	rm -f spider
