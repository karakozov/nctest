# nctest

Test simple wrapper class for ncurses library.

Example create a simple table with cells, header and footer.

Table dimension in project is set in source code file: nctest.cpp:

unsigned W = 14; // width of cells in symbols 
unsigned H = 3;  // heigth of cells in symbols
unsigned R = 4;  // row count
unsigned C = 7;  // coloumn count

Cells are filling from another thread. Press Ctrl+C to exit from application.
