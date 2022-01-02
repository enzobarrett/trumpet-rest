#include "main.h"
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>


int main() {	
    init();

    int state = 0; // 0: waiting for user to start session
		   // 1: in session
		   // 2: in wait period after session;

    session_t*  currSession = NULL; // current mini-session
    session_t** currSessionPtr = &currSession; // to help in setting currSession with initSession

    timeval_t now; // current time
    timeval_t lastPress; // last press of space bar, to be passed to stateone()

    // init stats for the overall session
    stat_t sessionStats; 
    init_stats(&sessionStats);

    printw ("Press space to begin.\n");
    refresh();

    char c;
    while ((c = getch()) != 'q') {
	if (state == 0 && c == ' ') {
	    // notify that new session has started
	    system("beep -l 50 &");

	    // once space is pressed a new session begins
	    // alloc a new session_t struct
	    initSession(currSessionPtr);
	    currSession = *currSessionPtr;

	    state = 1; // we are now in a session
	}

	// in session
	if (state == 1) {
	    gettimeofday(&lastPress, NULL); // set time of last space 
	    // handle timeing out spaces
	    if (stateone(&lastPress) == 1)
		c = 'q';

	    // set end time of session
	    currSession->endTime = &lastPress;

	    // calculate duration
	    currSession->duration = cmpTime(currSession->startTime, currSession->endTime);

	    // calculate end of wait time
	    currSession->endOfWait = timevalToDouble(currSession->endTime) + currSession->duration;

	    // print wait time
	    printw("Please wait for %f seconds!\n", currSession->duration);
	    refresh();

	    state = 2; // in wait period
	}

	// in wait period
	if (state == 2) {
	    // if session still in progress check if its over
	    gettimeofday(&now, NULL); 

	    // if its end time has passed
	    if (timevalToDouble(&now) > currSession->endOfWait) {
		system("beep -f 3000 -l 50 -r 3 &");
		printw ("Press space to begin.\n");
		sessionStats.numSessions++;

		// cleanup currSession
		if(currSession) {
		    free(currSession->startTime);
		    free(currSession);
		}

		state = 0; // reset to beginning
	    }
	}
    }

    cleanup(&sessionStats);
    return 0;
}

void init() {
    initscr();			/* Start curses mode */
    noecho();			/* Don't print user entered chars */
    cbreak();			/* Disable buffering */
    nodelay(stdscr, TRUE);	/* Makes getch() non-blocking, returns ERR if no char */
    scrollok(stdscr, TRUE);	/* Allow output scroll */
    keypad(stdscr, TRUE);       /* We get F1, F2 etc.. */

    signal(SIGINT, intHandler); /* Capture ctrl-c */
}

void intHandler(int dummy) {
    printw("Press q to quit.\n");
}

void init_stats(stat_t* stat) {
    stat->practiceStartTime = calloc(1, sizeof(timeval_t));
    stat->numSessions = 0;

    // set start time of session to current time
    gettimeofday(stat->practiceStartTime, NULL); 
}

void initSession(session_t** sessionPtr) {
    // alloc memory for session
    *sessionPtr = calloc(1, sizeof(session_t));

    // alloc timeval_t ptrs
    (*sessionPtr)->startTime = calloc(1, sizeof(timeval_t));
    (*sessionPtr)->endTime = calloc(1, sizeof(timeval_t));

    // set startTime
    gettimeofday((*sessionPtr)->startTime, NULL); 
}

int stateone(timeval_t* lastSpace) {
    timeval_t now;
    char c;

    // method is called once one space has been pressed
    int numSpaces = 1;

    while ((c = getch()) != 'q') {
	// update current time;
	gettimeofday(&now, NULL); 

	if (c == ' ') {
	    numSpaces++;
	    gettimeofday(lastSpace, NULL); 
	} else {
	    // second space has a longer timeout
	    if (numSpaces == 1) {
		if (cmpTime(lastSpace, &now) > .7)
		    return 0;
	    }
	    else {
		if (cmpTime(lastSpace, &now) > .2)
		    return 0;
	    }
	}
    }

    return 1; // calling method will set c to 'q'
}

double timevalToDouble(timeval_t* timeval) {
    return timeval->tv_sec + 1e-6 * timeval->tv_usec;
}

double cmpTime(timeval_t* t1, timeval_t* t2) {
    return timevalToDouble(t2)-timevalToDouble(t1);
}

void cleanup(stat_t* stats) {
    timeval_t now;
    gettimeofday(&now, NULL);

    // calulate session time elapsed
    double elapsed_sec = timevalToDouble(&now)-timevalToDouble(stats->practiceStartTime);

    // print stats and wait for user to look
    printw("You've practiced %f minutes this session!\n", elapsed_sec/60.0);
    printw("You've had a total of %d mini sessions!\n", stats->numSessions);
    refresh();

    // play mario victory sound
    system("./mariovictory.sh &");

    sleep(5);

    endwin();			/* End curses mode */
}
