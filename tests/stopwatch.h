#ifndef stopwatch_h
#define stopwatch_h

#include <time.h>

typedef struct Stopwatch {
	clock_t start_time;
	clock_t stop_time;
} Stopwatch;

static inline Stopwatch stopwatch_create(void);
static inline void stopwatch_start(Stopwatch *stopwatch);
static inline void stopwatch_stop(Stopwatch *stopwatch);
static inline clock_t stopwatch_get_elapsed_time(Stopwatch stopwatch);

static inline Stopwatch stopwatch_create(void)
{
	return (Stopwatch){ 0 };
}

static inline void stopwatch_start(Stopwatch *stopwatch)
{
	stopwatch->start_time = clock();
}

static inline void stopwatch_stop(Stopwatch *stopwatch)
{
	stopwatch->stop_time = clock();
}

static inline clock_t stopwatch_get_elapsed_time(Stopwatch stopwatch)
{
	return stopwatch.stop_time - stopwatch.start_time;
}

#endif // stopwatch_h
