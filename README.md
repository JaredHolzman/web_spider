# web_spider

Multithreaded Web Spider meant for ~~evil~~ good. Two implementations, one with just threads, the other using threadpools.

## General Project Structure
1. WebScraper has a single public method that takes in a web adress and returns all hrefs on that page

2. Threadsafe Queue that can safely be manipulated concurrently

3. WebSpiderThreads takes in a WebScraper and Threadsafe Queue and creates up to n threads to concurrently scrape pages and add all their hrefs to the queue

4. WebSpiderThreadPools takes in a WebScraper and Threadsafe Queue and uses threads from a thread pool to concurrently scrape pages and add all their hrefs to the queue

5. spider executable takes in command line arguments for which address to start at and which WebSpider implementation to use

## Usage
To run the spider, run:
```
make
./spider <webpage-address> <max-threads> <max-depth> <t|p> <v>
```

`http://www.umass.edu` is a good example.

For now, this will just print out (if 'v' is passed as an argument) hrefs  found as it explores up to `max-depth`.

## Dependencies
* libcurl
* gumbo-parser
* libpoco-dev

