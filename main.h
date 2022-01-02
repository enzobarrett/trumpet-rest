#include <sys/time.h>
#include <stdbool.h>

/*
 * timeval_t
 *
 * Redef of time.h struct timeval for convenience
 *
 * struct timeval {
 *    // Number of whole seconds of elapsed time
 *    time_t tv_sec;    
 *    // Number of microseconds of rest of elapsed time minus tv_sec. 
 *    // Always less than one million
 *    long int tv_usec;  
 * };
 *
 */
typedef struct timeval timeval_t;

/*
 * struct Session or session_t
 *
 * Holds information relating to current mini-session
 *
 * startTime: start time of mini-session
 * endTime: end time of mini-session
 * duration: difference of startTime and endTime in seconds
 * endOfWait: endTime + duration in seconds
 */
typedef struct Session {
    struct timeval* startTime;
    struct timeval* endTime;
    double duration;
    double endOfWait;
} session_t;

/*
 * struct Stats or stat_t
 *
 * Stores statistics of overall practice session
 *
 * numSessions: the number of mini-sessions
 * practiceStartTime: start of overall session, used to
 * calculate overall practice session duration
 */
typedef struct Stats {
    int numSessions;
    struct timeval* practiceStartTime;
} stat_t;


/* 
 * void init();
 *
 * Initializes curses and sets up intHandler (see below)
 *
 */
void init();

/* 
 * void intHandler(int dummy);
 *
 * Catches Ctrl-C singnal and prints "please use 'q' to quit"
 * to the screen
 *
 * dummy: unused var
 */
void intHandler(int dummy);

/*
 * void init_stats(stat_t* stat);
 *
 * Initializes stat_t struct:
 * Allocs space for start time
 * Sets numSessions to 0
 * Sets start time to current time
 *
 * stat: ptr to stat_t struct
 */
void init_stats(stat_t* stat);

/*
 * void initSession(session_t**);
 *
 * Initializes session_t struct and ptr:
 * - Allocs space for session_t
 * - Allocs timeval_t for start and end time
 * - Sets start time
 */
void initSession(session_t**);

/*
 * int stateone(timeval_t* lastspace);
 *
 * Helper function to handle user input while in
 * mini-session. Times out in .7 seconds after the
 * first space is pressed, .2 for each space after.
 * This accounts for keyboard delay when holding down
 * the space button.
 *
 * lastspace: ptr to timeval_t that stores the time
 * that the last space was pressed, this helps to make
 * a more accurate calculation of the session time and allows
 * us time gauge the time since the last space was pressed
 *
 * returns: 0 for normal return, 1 if q was pressed during execution
 *
 */
int stateone(timeval_t* lastspace);

/*
 * double timevalToDouble(timeval_t* timeval);
 *
 * Converts timeval_t struct to seconds stored in a 
 * double.
 *
 * timeval: ptr to timeval_t that stores time to be
 * converted. 
 *
 * returns: double with num seconds
 */
double timevalToDouble(timeval_t* timeval);

/*
 * double cmpTime(timeval_t* t1, timeval_t* t2);
 *
 * Compares two timeval_t structs using timevalToDouble
 *
 * t1: the smaller timeval_t struct
 * t2: the larger timeval_t struct
 *
 * returns: the difference in seconds as a double. Will be 
 * negative if t1 is larger
 */
double cmpTime(timeval_t* t1, timeval_t* t2);

/*
 * void cleanup(stat_t* stats)
 *
 * free stat_t struct and print stats to the screen
 *
 * stats: ptr to stat_t struct 
 */
void cleanup(stat_t* stats);
