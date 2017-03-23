TQDIR = threadsafe_queue
WSDIR = webpage_scraper
WTDIR = webspider_threads
WPDIR = webspider_threadpools

CC = g++
CFLAGS = -std=c++11 -lpthread -lgumbo -lcurl -lsoup-2.4 -lgio-2.0 -lgobject-2.0 -lglib-2.0
OBJS = page.o threadsafe_queue.o webpage_scraper.o webspider_threadpools.o webspider_threads.o

all: main

main: main.cc $(OBJS)
	$(CC) $(OBJS) main.cc -o spider $(CFLAGS)

webspider_threadpools.o: $(WPDIR)/webspider_threadpools.cc $(WPDIR)/webspider_threadpools.h
	$(CC) -c $(WPDIR)/webspider_threadpools.cc $(CFLAGS)

webspider_threads.o: $(WTDIR)/webspider_threads.cc $(WTDIR)/webspider_threads.h
	$(CC) -c $(WTDIR)/webspider_threads.cc $(CFLAGS)

threadsafe_queue.o: $(TQDIR)/threadsafe_queue.cc $(TQDIR)/threadsafe_queue.h
	$(CC) -c $(TQDIR)/threadsafe_queue.cc $(CFLAGS)

page.o: $(TQDIR)/page.cc $(TQDIR)/page.h
	$(CC) -c $(TQDIR)/page.cc $(CFLAGS)

webpage_scraper.o: $(WSDIR)/webpage_scraper.cc $(WSDIR)/webpage_scraper.h
	$(CC) -c $(WSDIR)/webpage_scraper.cc $(CFLAGS)

clean:
	rm -f *.o
	rm -f spider
