
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>
#include <vector>

#include "nctable.h"

static bool exit_flag = 0;

void resizeHandler(int sig);
void exitHandler(int sig);
void* test_thread(void *params);

//------------------------------------

unsigned W = 14;
unsigned H = 3;
unsigned R = 4;
unsigned C = 7;

//------------------------------------
using namespace std;
//------------------------------------

int main(int argc, char *argv[])
{
    signal(SIGINT, exitHandler);

    table *t = new table(C,W,H);
    if(!t) {
        exit(-1);
    }

    //for(unsigned i=0; i<R; i++) {
    //    t->add_row();
    //}

    t->create_table(R, C);

    if(t->create_header())
        t->set_header_text("%s", "Table header text");

    if(t->create_status())
        t->set_status_text("%s", "Status header text");

    //for(unsigned i=0; i<R; i++) {
    //    for(unsigned j=0; j<C; j++) {
    //        t->set_cell_text(i,j,"%s %d %d","CELL ", i, j);
    //    }
    //}

    //t->refresh_table();

    pthread_t thread;
    int res = pthread_create(&thread, NULL, test_thread, (void*)t);
    if(res < 0) {
        exit(-1);
    }

    while(!exit_flag) {
        sleep(1);
    }

    pthread_join(thread, NULL);

    sleep(1);

    delete t;

    return 0;
}

//------------------------------------

void resizeHandler(int sig)
{
    int nh, nw;
    getmaxyx(stdscr, nh, nw);
    fprintf(stderr, "h = %d  w = %d\n", nh, nw);
}

//------------------------------------

void exitHandler(int sig)
{
    exit_flag = true;
}

//------------------------------------

void* test_thread(void *params)
{
    int step_counter = 0;

    table *t = (table*)params;

    while(!exit_flag) {

        for(unsigned i=0; i<R; i++) {
            for(unsigned j=0; j<C; j++) {
                //t->clear_cell_text(i,j);
                t->set_cell_text(i,j,"%s%d", "WINDOW ", step_counter);
            }
        }

        sleep(1);

        ++step_counter;
    }

    for(unsigned i=0; i<R; i++) {
        for(unsigned j=0; j<C; j++) {
            t->clear_cell_text(i,j);
            t->set_cell_text(i,j,"%s", "EXIT THREAD");
        }
    }

    return NULL;
}
