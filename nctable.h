
#ifndef __NCTABLE_H__
#define __NCTABLE_H__

#include <ncurses.h>
#include <vector>

struct row_t {
    int X0;
    int Y0;
    std::vector<WINDOW*> w;
};

struct header_t {
    int X0;
    int Y0;
    int W;
    int H;
    WINDOW *w;
};

class table {

public:
    table();
    table(int colomn_number, int cell_width, int cell_height);
    table(int row, int col, int cell_width, int cell_height);
    virtual ~table();

    int create_table(int nrow, int ncol);
    int add_row();
    bool create_header();
    int set_header_text(const char *fmt, ...);
    bool create_status();
    int set_status_text(const char *fmt, ...);
    int set_cell_text(unsigned nrow, unsigned ncol, const char *fmt, ...);
    int clear_cell_text(unsigned nrow, unsigned ncol);
    int set_colomn_number(int col);
    void refresh_table();

private:

    void init_ncurses();
    void clear_table();
    void clear_status();
    void clear_header();

    int m_colomn;
    int m_row;
    int m_maxW;
    int m_maxH;
    int m_W0;
    int m_H0;
    int m_CW;
    int m_CH;

    std::vector<struct row_t> m_table;

    header_t m_header;
    header_t m_status;
};

#endif //__NCTABLE_H__
