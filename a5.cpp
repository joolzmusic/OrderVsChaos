// a5.cpp

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : <put your full name here!>
// St.# : <put your full SFU student number here>
// Email: <put your SFU email address here>
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anyone else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

//
// You can use any other C++17 standard #includes that you need.
//
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

enum Game_State { INCOMPLETE, ORDER_WIN, CHAOS_WIN };

class Board {
private:
    char** _board/*[9][9]*/;
    ushort _width;
    ushort _height;

public:
    Board(ushort x, ushort y): _board(nullptr), _width(x), _height(y) {
        if (x > 9 || x < 1 || y > 9 || y < 1) {
            throw runtime_error("Tried to make a board with dimensions outside 1x1 to 9x9!");
            _width = 9;
            _height = 9;
        }

        // Frankly unnecessary but I wanted to practice dynamic allocation
        _board = new char*[_width];
        for (ushort i = 0; i < _width; i++) {
            _board[i] = new char[_height];
            for (ushort j = 0; j < _height; j++) _board[i][j] = '_'; // Don't want garbage data
        }

        /*
        for (ushort i = 0; i < _width; i++) {
            for (ushort j = 0; j < _height; j++) _board[i][j] = '_';
        }
        */
    }

    ~Board() {
        for (ushort i = 0; i < _width; i++) delete[] _board[i];
        delete[] _board;
    }

    const ushort& width = _width;
    const ushort& height = _height;
    char** board = _board; // Be careful! Look - don't touch!

    void verify_board() {
        for (ushort i = 0; i < _height; i++)
            for (ushort j = 0; j < _width; j++)
                if (_board[j][i] != 'X' && _board[j][i] != 'O' && _board[j][i] !=  '_') 
                    throw runtime_error("Invalid board!");
    }

    char get_tile(ushort x, ushort y) {
        if (x >= _width || y >= _height) {
            throw runtime_error("Tried to get tile out of bounds!");
            return '\0';
        }

        return _board[x][y];
    }

    // Returns 1 if the piece was successfully added, 0 if not.
    // This method won't add a piece if there's already a piece at the specified spot or if an
    // exception is thrown, which happens if X exceeds _width - 1, Y exceeds _height - 1, or if
    // C is not 'X' or 'O'.
    bool add_piece(char c, ushort x, ushort y) {
        if (x >= _width || y >= _height) {
            throw runtime_error("Tried to set to tile out of bounds!");
            return 0;
        }

        if (c != 'X' && c != 'O') {
            throw runtime_error("Tried to set invalid piece! (Must be X or O)");
            return 0;
        }

        if (_board[x][y] == 'X' || _board[x][y] == 'O') return 0;

        _board[x][y] = c;
        return 1;
    }

    Game_State check_gamestate() {
        verify_board(); // Can't be too safe!
        // Check for a row column, or cross of 5 'X's or 'O's

        // Check if all board spaces are filled
        for (ushort i = 0, spaces_used = 0; i < _width; i++) {
            for (ushort j = 0; j < _height; j++) {
                if (_board[j][i] == 'X' || _board[j][i] == 'Y') spaces_used++;
            }

            if (spaces_used == _width * _height) return CHAOS_WIN;
        }

        return INCOMPLETE;
    }

    void print_board() {
        cout << ' ';
        for (ushort i = 0; i < _height; i++) cout << ' ' << i;
        cout << '\n';
        for (ushort i = 0; i < _height; i++) {
            cout << static_cast<char>(i + 'A');
            for (ushort j = 0; j < _width; j++) cout << ' ' << _board[j][i];
            cout << '\n';
        }
    }
};

bool AI_order(Board& board);
bool AI_chaos(Board& board);
bool AI_dummy(Board& board);

int main() {
    Board board(9, 9);
    board.print_board();

    // Extremely incomplete
    while (board.check_gamestate() == INCOMPLETE) {
        string player_move = "";
        cout << "Enter a move [numberLETTER(X/O)]: ";
        cin >> player_move;
        ushort x = player_move.at(0) - '0';
        ushort y = player_move.at(1) - 'A';
        char c = player_move.at(2);

        board.add_piece(c, x, y);
        board.print_board();
    }

    return 0;
}


/*
Assignment 5 Report
===================
Description of Computers Playing Strategy
-----------------------------------------
AI_order:
    TBA. If playing first, always takes a centre tile with a random piece.

AI_chaos:
    A contrarian. Searches for the longest row/column/cross of the same pieces, and places the
    opposite piece at one end. If playing first, always takes a centre tile with a random piece.

AI_dummy:
    Always places a random piece on a random tile. Rather boring.

Extra Features
--------------
(Describe all extra features your program has that were not required by the
assignment; if there are no extra features, then say that.)
Known Bugs and Limitations
--------------------------
(If your program is perfect and unlimited, then say so!)
Acknowledgements of All Help
----------------------------
(Cite all websites, books, people, etc. that you got help from, including what
sort of help you got. Each citation should be about 1 to 2 lines long.)
*/