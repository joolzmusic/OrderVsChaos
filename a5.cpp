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
    char** _board;
    ushort _rows;
    ushort _cols;

public:
    Board(ushort x, ushort y): _board(nullptr), _rows(y), _cols(x) {
        if (x > 9 || x < 6 || y > 9 || y < 6) {
            throw runtime_error("Tried to make a board with dimensions outside 1x1 to 9x9!");
            _rows = 6;
            _cols = 6;
        }

        // Frankly unnecessary but I wanted to practice dynamic allocation
        _board = new char*[_rows];
        for (ushort i = 0; i < _rows; i++) {
            _board[i] = new char[_cols];
            for (ushort j = 0; j < _cols; j++) _board[i][j] = '.'; // Don't want garbage data!
        }
    }

    ~Board() {
        for (ushort i = 0; i < _rows; i++) delete[] _board[i];
        delete[] _board;
    }

    const ushort& width = _cols;
    const ushort& height = _rows;
    // char** board = _board; // Be careful! Look - don't touch!

    void verify_board() {
        for (ushort y = 0; y < height; y++)
            for (ushort x = 0; x < width; x++)
                if (_board[y][x] != 'X' && _board[y][x] != 'O' && _board[y][x] !=  '.')
                    throw runtime_error("Invalid board!");
    }

    char get_tile(ushort x, ushort y) {
        if (x >= width || y >= height) {
            throw runtime_error("Tried to get cell out of bounds!");
            return '\0';
        }

        return _board[y][x];
    }

    // Returns 1 if the piece was successfully added, 0 if not.
    // This method won't add a piece if there's already a piece at the specified spot or if an
    // exception is thrown, which happens if X exceeds _width - 1, Y exceeds _height - 1, or if
    // C is not 'X' or 'O'.
    bool add_piece(char c, ushort x, ushort y) {
        if (x >= width || y >= height) {
            throw runtime_error("Tried to set to cell out of bounds!");
            return false;
        }

        if (c != 'X' && c != 'O') {
            throw runtime_error("Tried to set invalid piece! (Must be X or O)");
            return false;
        }

        if (_board[y][x] == 'X' || _board[y][x] == 'O') return false;

        _board[y][x] = c;
        return true;
    }

    Game_State check_gamestate() {
        verify_board(); // Can't be too safe!
        // Check for a row, column, or cross of 5 'X's or 'O's
        // Row check
        for (ushort y = 0; y < height; y++) {
            ushort consecutive = 0;

            for (ushort x = 1; x < width; x++) {
                if (_board[y][x] == _board[y][x - 1] && _board[y][x] != '.') consecutive++;
            }

            if (consecutive >= 4) return ORDER_WIN;
        }

        // Column check
        for (ushort x = 0; x < width; x++) {
            ushort consecutive = 0;

            for (ushort y = 1; y < height; y++) {
                if (_board[y][x] == _board[y - 1][x] && _board[y][x] != '.') consecutive++;
            }

            if (consecutive >= 4) return ORDER_WIN;
        }

        // Cross (\) check
        

        // Cross (/) check

        // Check if all board spaces are filled
        ushort spaces_used = 0;
        for (ushort y = 0; y < height; y++) {
            for (ushort x = 0; x < width; x++) {
                if (_board[y][x] == 'X' || _board[y][x] == 'Y') spaces_used++;
            }

            if (spaces_used == width * height) return CHAOS_WIN;
        }

        return INCOMPLETE;
    }

    void print_board() {
        cout << "  ";
        for (ushort x = 0; x < width; x++) cout << x + 1 << ' ';
        cout << '\n';

        for (ushort y = 0; y < height; y++) {
            cout << static_cast<char>('A' + y) << ' ';
            for (ushort x = 0; x < width; x++) cout << _board[y][x] << ' ';
            cout << '\n';
        }

        cout << '\n';
    }
};

bool AI_order(Board& board);
bool AI_chaos(Board& board);
bool AI_dummy(Board& board);

int main() {
    Board board(8, 6);
    board.print_board();

    // Extremely incomplete
    while (board.check_gamestate() == INCOMPLETE) {
        string player_move = "";
        cout << "Enter a move [NumberLetter(X/O)]: ";
        cin >> player_move;

        char c = toupper(player_move.at(2));
        ushort x = player_move.at(0) - '0' - 1;
        ushort y = toupper(player_move.at(1)) - 'A';

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
    TBA. If playing first, always takes a centre cell with a random piece.

AI_chaos:
    A contrarian. Searches for the longest row/column/cross of the same pieces, and places the
    opposite piece at one end. If playing first, always takes a centre cell with a random piece.

AI_dummy:
    Always places a random piece on a random cell. Rather boring.

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