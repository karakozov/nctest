
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <locale.h>
#include <vector>

#include "nctable.h"

//-----------------------------------------------------------------------------

table::table()
{
}

//-----------------------------------------------------------------------------

table::table(int colomn_number, int cell_width, int cell_height)
{
#ifdef __USE_NCURSES__
    init_ncurses();

    m_row = 0;
    m_colomn = colomn_number;
    m_CW = cell_width;
    m_CH = cell_height;
    m_table.clear();

    getmaxyx(stdscr,m_maxH,m_maxW);

    m_W0 = m_maxW/2;
    m_H0 = m_maxH/2-m_maxH/4;

    m_header.w = NULL;
    m_header.X0 = m_header.Y0 = m_header.W = m_header.H = 0;
    m_status.w = NULL;
    m_status.X0 = m_status.Y0 = m_status.W = m_status.H = 0;
#endif
}

//-----------------------------------------------------------------------------

table::table(int row, int col, int cell_width, int cell_height)
{
#ifdef __USE_NCURSES__
    init_ncurses();

    m_row = row;
    m_colomn = col;
    m_CW = cell_width;
    m_CH = cell_height;
    m_table.clear();
#endif
}

//-----------------------------------------------------------------------------

table::~table()
{
#ifdef __USE_NCURSES__
    clear_table();
    clear_header();
    clear_status();
    endwin();
#endif
}

//-----------------------------------------------------------------------------

void table::init_ncurses()
{
#ifdef __USE_NCURSES__
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush( stdscr, FALSE );
    keypad(stdscr, TRUE);
#endif
}

//-----------------------------------------------------------------------------

void table::refresh_table()
{
#ifdef __USE_NCURSES__
    for(unsigned i=0; i<m_table.size(); i++) {
        row_t row = m_table.at(i);
        for(unsigned j=0; j<row.w.size(); j++) {
            WINDOW *w = row.w.at(j);
            wrefresh(w);
        }
    }
    if(m_header.w) wrefresh(m_header.w);
    if(m_status.w) wrefresh(m_status.w);
    refresh();
#endif
}

//-----------------------------------------------------------------------------

void table::clear_table()
{
#ifdef __USE_NCURSES__
    for(unsigned i=0; i<m_table.size(); i++) {
        row_t row = m_table.at(i);
        for(unsigned j=0; j<row.w.size(); j++) {
            WINDOW *w = row.w.at(j);
            delwin(w);
        }
        row.w.clear();
    }
    m_table.clear();
#endif
}

//-----------------------------------------------------------------------------

int table::create_table(int nrow, int ncol)
{
#ifdef __USE_NCURSES__
    clear_table();

    int dX = m_CW * ncol/2;
    int dY = m_CH * nrow/2;

    for(int row = 0; row < nrow; row++) {

        int x0 = m_W0 - dX;
        int y0 = m_H0 + m_CH * row - dY;

        row_t new_row;

        new_row.X0 = x0;
        new_row.Y0 = y0;

        for(int col = 0; col < ncol; col++) {

            int xn = x0 + m_CW * col;
            int yn = y0;

            WINDOW *w = newwin(m_CH, m_CW, yn, xn);
            if(!w)
                break;
            box(w, 0 , 0);
            wrefresh(w);
            new_row.w.push_back(w);
        }

        m_table.push_back(new_row);
    }

    return m_table.size();
#else
    return 0;
#endif
}

//-----------------------------------------------------------------------------

int table::add_row()
{
#ifdef __USE_NCURSES__
    int id = 0;
    if(!m_table.empty())
        id = m_table.size()-1;

    row_t last_row = m_table.at(id);
    int x0 = last_row.X0;
    int y0 = last_row.Y0 + m_CH;

    row_t new_row;
    new_row.X0 = x0;
    new_row.Y0 = y0;

    for(int col = 0; col < m_colomn; col++) {

        int xn = x0 + m_CW * col;
        int yn = y0;

        WINDOW *w = newwin(m_CH, m_CW, yn, xn);
        if(!w)
            break;
            box(w, 0 , 0);
            wrefresh(w);
            new_row.w.push_back(w);
    }

    m_table.push_back(new_row);

    //refresh_table();

    return m_table.size() - 1;
#else
    return 0;
#endif
}

//-----------------------------------------------------------------------------

int table::set_cell_text(unsigned nrow, unsigned ncol, const char *fmt, ...)
{
#ifdef __USE_NCURSES__
    if(nrow >= m_table.size())
        return -1;

    row_t& row = m_table.at(nrow);

    if(ncol >= row.w.size())
        return -2;

    WINDOW *w = row.w.at(ncol);
    if(!w)
        return -3;

    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    wmove(w, m_CH/2, m_CW/2-strlen(msg)/2);
    wprintw(w, "%s", msg);
    wrefresh(w);

    return 0;
#else
    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    fprintf(stderr, "%s\n", msg);

    return 0;
#endif
}

//-----------------------------------------------------------------------------

int table::clear_cell_text(unsigned nrow, unsigned ncol)
{
#ifdef __USE_NCURSES__
    if(nrow >= m_table.size())
        return -1;

    row_t& row = m_table.at(nrow);

    if(ncol >= row.w.size())
        return -2;

    WINDOW *w = row.w.at(ncol);
    if(!w)
        return -3;

    wclear(w);
    box(w, 0, 0);
    wrefresh(w);
#endif
    return 0;
}

//-----------------------------------------------------------------------------

bool table::create_header()
{
#ifdef __USE_NCURSES__
    if(m_header.w)
        return true;

    if(m_table.empty())
        return false;

    row_t row0 = m_table.at(0);

    m_header.X0 = row0.X0;
    m_header.Y0 = row0.Y0-m_CH;

    m_header.H = m_CH;
    m_header.W = m_CW*row0.w.size();

    WINDOW *w = newwin(m_header.H, m_header.W, m_header.Y0, m_header.X0);
    if(!w) {
        return false;
    }

    m_header.w = w;
    box(w, 0 , 0);

    wrefresh(w);
#endif
    return true;
}

//-----------------------------------------------------------------------------

bool table::create_status()
{
#ifdef __USE_NCURSES__
    if(m_status.w)
        return true;

    if(m_table.empty())
        return false;

    row_t rowN = m_table.at(m_table.size()-1);

    m_status.X0 = rowN.X0;
    m_status.Y0 = rowN.Y0+m_CH;

    m_status.H = m_CH;
    m_status.W = m_CW*rowN.w.size();

    WINDOW *w = newwin(m_status.H, m_status.W, m_status.Y0, m_status.X0);
    if(!w) {
        return false;
    }

    m_status.w = w;
    box(w, 0 , 0);

    wrefresh(w);
#endif
    return true;
}

//-----------------------------------------------------------------------------

int table::set_header_text(const char *fmt, ...)
{
#ifdef __USE_NCURSES__
    if(!m_header.w)
        return -1;

    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    wmove(m_header.w, m_header.H/2, m_header.W/2-strlen(msg)/2);
    wprintw(m_header.w, "%s", msg);
    wrefresh(m_header.w);
#else
    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    fprintf(stderr, "%s\n", msg);
#endif
    return 0;
}

//-----------------------------------------------------------------------------

int table::set_status_text(const char *fmt, ...)
{
#ifdef __USE_NCURSES__
    if(!m_status.w)
        return -1;

    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    wmove(m_status.w, m_status.H/2, m_status.W/2-strlen(msg)/2);
    wprintw(m_status.w, "%s", msg);
    wrefresh(m_status.w);
#else

    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    fprintf(stderr, "%s\n", msg);
#endif
    return 0;
}

//-----------------------------------------------------------------------------

void table::clear_status()
{
#ifdef __USE_NCURSES__
    if(m_status.w) {
        delwin(m_status.w);
        m_status.w = NULL;
    }
#endif
}

//-----------------------------------------------------------------------------

void table::clear_header()
{
#ifdef __USE_NCURSES__
    if(m_header.w) {
        delwin(m_header.w);
        m_header.w = NULL;
    }
#endif
}

//-----------------------------------------------------------------------------
