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
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace std;

enum Game_State { INCOMPLETE, ORDER_WIN, CHAOS_WIN };
struct Point { ushort x, y; };
struct Move { char piece; Point cell; };

class Board {
private:
    char** _board;
    ushort _size;
    ushort _used;

public:
    Board(ushort s): _board(nullptr), _size(s), _used(0) {
        if (s > 9 || s < 6) {
            throw runtime_error("Tried to make a board with dimensions outside 6x6 to 9x9!");
            _size = 6;
        }

        // Frankly unnecessary but I wanted to practice dynamic allocation
        _board = new char*[_size];
        for (ushort y = 0; y < _size; y++) {
            _board[y] = new char[_size];
            for (ushort x = 0; x < _size; x++) _board[y][x] = '.'; // Don't want garbage data!
        }
    }

    ~Board() {
        for (ushort i = 0; i < _size; i++) delete[] _board[i];
        delete[] _board;
    }

    const ushort& size = _size;
    const ushort& used = _used;
    const uint area = _size * _size;

    void verify_board() const {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                if (_board[y][x] != 'X' && _board[y][x] != 'O' && _board[y][x] !=  '.')
                    throw runtime_error("Invalid board!");
    }

    char get_cell(ushort x, ushort y) const {
        if (x >= size || y >= size) {
            throw runtime_error("Tried to get cell out of bounds!");
            return '\0';
        }

        return _board[y][x];
    }

    void print_board() const {
        cout << "  ";
        for (ushort x = 0; x < size; x++) cout << x + 1 << ' ';
        cout << '\n';

        for (ushort y = 0; y < size; y++) {
            cout << static_cast<char>('A' + y) << ' ';
            for (ushort x = 0; x < size; x++) cout << _board[y][x] << ' ';
            cout << '\n';
        }

        cout << '\n';
    }

    // This one's a doozy
    Game_State check_gamestate() const {
        verify_board(); // Can't be too safe!

        // Check for five in a row in any direction by casting a "ray" in each direction
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++) {
                if (_board[y][x] == '.') continue;

                // Check south
                if (y + 4 < size) { // Check to see if there's actually enough room for a line
                    for (short i = y, consecutive = 1; i < size; i++) {
                        if (_board[i][x] == _board[i + 1][x]) consecutive++;
                        else break;
                        if (consecutive >= 5) return ORDER_WIN;
                    }
                }

                // Check east
                if (x + 4 < size) {
                    for (short i = x, consecutive = 1; i < size; i++) {
                        if (_board[y][i] == _board[y][i + 1]) consecutive++;
                        else break;
                        if (consecutive >= 5) return ORDER_WIN;
                    }
                }

                // Check southeast
                if (x + 4 < size && y + 4 < size) {
                    for (short i = x, j = y, consecutive = 1; i < size && j < size; i++, j++) {
                        if (_board[j][i] == _board[j + 1][i + 1]) consecutive++;
                        else break;
                        if (consecutive >= 5) return ORDER_WIN;
                    }
                }

                // Check southwest
                if (x >= 4 && y + 4 < size) {
                    for (short i = x, j = y, consecutive = 1; i >= 0 && j < size; i--, j++) {
                        if (_board[j][i] == _board[j + 1][i - 1]) consecutive++;
                        else break;
                        if (consecutive >= 5) return ORDER_WIN;
                    }
                }
        }
        
        // If all cells are used, Chaos wins. If not, the game is incomplete.
        return used == size * size ? CHAOS_WIN : INCOMPLETE;
    }

    // Returns a heap-allocated array of all free cells on the board. You must free this!
    Point* get_free_cells() const {
        Point* free_cells = new Point[size * size - used];
        ushort index = 0;

        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                if (_board[y][x] == '.') {
                    free_cells[index].x = x;
                    free_cells[index].y = y;
                    index++;
                }

        return free_cells;
    }

    void reset_board() {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                _board[y][x] = '.';

        _used = 0;
    }

    // Returns true if the piece was successfully added, false if not. Also throws exceptions.
    bool add_piece(Move m) {
        if (m.cell.x >= size || m.cell.y >= size) {
            throw runtime_error("Tried to set to cell out of bounds!");
            return false;
        }

        if (m.piece != 'X' && m.piece != 'O') {
            throw runtime_error("Tried to set invalid piece! (Must be X or O)");
            return false;
        }

        if (_board[m.cell.y][m.cell.x] == 'X' || _board[m.cell.y][m.cell.x] == 'O') return false;

        _board[m.cell.y][m.cell.x] = m.piece;
        _used++;
        return true;
    }
};


Move ai_random(Board& board);
Move ai_clever(Board& board);

int main() {
    srand(time(NULL));

    int board_size;

    cout<<"Welcome to Order and Chaos!\n";

    while(true) {
        try {
            cout << "Please choose appropriate size for board game: 6, 7, 8, or 9: ";
            cin >> board_size;
            Board game(board_size); // if runtime error happens, it goes to the exception catch asap

            break;
        } catch (const std::runtime_error& e) {
            cout << "Invalid input.\n";
        }
    }

    Board game(board_size);
    Game_State gs = game.check_gamestate();
    game.print_board();

    // Extremely incomplete
    while (gs == INCOMPLETE) {
        string player_move = "";
        while(true) {
            try {
                cout << "Enter a move [NumberLetter(X/O)]: ";
                cin >> player_move;

                char c = toupper(player_move.at(2));
                ushort x = player_move.at(0) - '0' - 1;
                ushort y = toupper(player_move.at(1)) - 'A';

                game.add_piece({c, {x, y}});

                break;
            } catch(const runtime_error& e) {
                cout << "Invalid input.\n";
            }
        }

        Move ai_move = ai_random(game);
        game.add_piece(ai_move);
        game.print_board();
        cout << "CPU plays an \"" << ai_move.piece << "\" at " << ai_move.cell.x + 1
             << static_cast<char>(ai_move.cell.y + 'A') << '\n';

        gs = game.check_gamestate();

        switch (gs) {
            case ORDER_WIN:
                cout << "Order wins!\n";
                break;
            
            case CHAOS_WIN:
                cout << "Chaos wins!\n";
                break;
            
            default:
                break;
        }
    }

    return 0;
}

// Loads the coordinates of each empty cell on the board and places a random piece on one of them.
// Returns the coordinates of the piece placed.
Move ai_random(Board& board) {
    if (board.used == board.size * board.size) {
        throw runtime_error("Tried to play on a full board!");
        return {'X', {0, 0}};
    }

    Point* free_cells = board.get_free_cells();

    Move m = {
        rand() % 2 ? 'X' : 'O',
        free_cells[rand() % (board.size * board.size - board.used)]
    };

    delete[] free_cells;
    return m;
}

Move ai_clever(Board& board) {
    if (board.used == board.size * board.size) {
        throw runtime_error("Tried to play on a full board!");
        return {'O', {0, 0}};
    }

    Point* free_cells = board.get_free_cells();

    Move m = {'.', {0, 0}};

    delete[] free_cells;
    return m;
}

/*
Assignment 5 Report
===================
Description of Computers Playing Strategy
-----------------------------------------
ai_random:
    Always places a random piece on a random open cell. Rather boring.

ai_clever:
    Looks for the longest sequence of pieces in a row, column, or diagonal. If it finds one, it
    places a piece on the next cell in that sequence. If playing Order, it places a piece of the
    same type. Vice versa if playing Chaos. Opening play is a random piece on a centre tile.

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