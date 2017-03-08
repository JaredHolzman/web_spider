TQDIR = threadsafe_queue
WSDIR = webpage_scraper
WTDIR = webspider_threads
WPDIR = webspider_threadpools

CC = g++
CFLAGS = -std=c++11 -lboost_system -lboost_thread -lpthread
OBJS = threadsafe_queue.o webpage_scraper.o

all: main

main: main.cc $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) main.cc -o spider

threadsafe_queue.o: $(TQDIR)/threadsafe_queue.cc $(TQDIR)/threadsafe_queue.h
	$(CC) $(CFLAGS) -c $(TQDIR)/threadsafe_queue.cc

webpage_scraper.o: $(WSDIR)/webpage_scraper.cc $(WSDIR)/webpage_scraper.h
	$(CC) $(CFLAGS) -c $(WSDIR)/webpage_scraper.cc

clean:
	rm -f *.o
	rm -f spider
