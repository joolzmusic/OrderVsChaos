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
    char** board = _board;

    //just for testing. No need to catch this
    void verify_board() const {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                if (_board[y][x] != 'X' && _board[y][x] != 'O' && _board[y][x] !=  '.')
                    throw runtime_error("Invalid board!");
    }
 
    //For AI only. No need to catch this
    char get_tile(ushort x, ushort y) const {
        if (x >= size || y >= size) {
            throw runtime_error("Tried to get cell out of bounds!");
            return '\0';
        }

        return _board[y][x];
    }

    // Should this produce a string instead?
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
        return used == size ? CHAOS_WIN : INCOMPLETE;
    }

    void reset_board() {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                _board[y][x] = '.';

        _used = 0;
    }

    // Returns true if the piece was successfully added, false if not. Also throws exceptions.
    bool add_piece(char c, ushort x, ushort y) {
        if (x >= size || y >= size) {
            throw runtime_error("Tried to set to cell out of bounds!");
            return false;
        }

        if (c != 'X' && c != 'O') {
            throw runtime_error("Tried to set invalid piece! (Must be X or O)");
            return false;
        }

        if (_board[y][x] == 'X' || _board[y][x] == 'O') return false;

        _board[y][x] = c;
        _used++;
        return true;
    }
};

bool AI_order(Board& board);
bool AI_chaos(Board& board);
bool AI_dummy(Board& board);

int main() {

    //display instructions for humans at start of game //TODO
    int humanInput;
    cout<<"Welcome to Order and Chaos! ";

    while(true)
    {
        try {
            cout<<"Please choose appropriate size for board game: 6, 7, 8, or 9: ";
            cin>> humanInput;
            Board game(humanInput); //if runtime error happens, it goes to the exception catch asap

            break;

        } catch (const std::runtime_error& e)
        {
            cout << "Incorrect input. ";
        }
    }

    Board game(humanInput);
    Game_State gs = game.check_gamestate();
    game.print_board();

    // Extremely incomplete
    while (gs == INCOMPLETE) {
        string player_move = "";
        while(true)
        {
            try {
                cout << "Enter a move [NumberLetter(X/O)]: ";
                cin >> player_move;
                
                char c = toupper(player_move.at(2));
                ushort x = player_move.at(0) - '0' - 1;
                ushort y = toupper(player_move.at(1)) - 'A';

                game.add_piece(c, x, y);

                break;
            } catch(const runtime_error& e) {
                cout<<"Incorrect input. ";
            }
        }
        game.print_board();
        cout << "Cells used: " << game.used << "\n\n";
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