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
#include <ctime> //provides time() function
#include <cstdlib> //provides rand() and srand (seed) functions
#include <stdexcept> //provides runtime_error
#include <string>

using namespace std;

// Could just use int-based return codes but this improves readability
// Also using enum classes to satisfy project requirements
enum class Player_Role { ORDER, CHAOS };
enum class Game_State { INCOMPLETE, ORDER_WIN, CHAOS_WIN };

struct Point { ushort x, y; };
struct Move { char piece; Point cell; };

// The big cheese. Holds the board and all the game logic.
// Board is stored as a heap-alloced 2D char array.
class Board {
private:
    char** _board; //pointer to a pointer of char
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
            for (ushort x = 0; x < _size; x++) _board[y][x] = '*'; // Don't want garbage data!
        }
    }

    // Copy constructor, may well never be used.
    Board(const Board& b): _board(nullptr), _size(b.size), _used(b.used) {
        _board = new char*[_size];
        for (ushort y = 0; y < _size; y++) {
            _board[y] = new char[_size];
            for (ushort x = 0; x < _size; x++) _board[y][x] = b.get_cell(x, y); 
            //b.get_cell(x, y) same as b._board[y][x]
        }
    }

    ~Board() {
        for (ushort i = 0; i < _size; i++) delete[] _board[i];
        delete[] _board;
    }

    const ushort& size = _size; //pass by reference and can't be changed
    const ushort& used = _used;
    const uint area = _size * _size;

    // Used for debugging. This error should not be caught, but must prevented from ever happening!
    void verify_board() const {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                if (_board[y][x] != 'X' && _board[y][x] != 'O' && _board[y][x] !=  '*')
                    throw runtime_error("Invalid board!");
    }

    // Returns the character at a given board spot. Throws error if OOB - should never happen!
    char get_cell(ushort x, ushort y) const {
        if (x >= size || y >= size) {
            throw runtime_error("Tried to get cell out of bounds!");
            return '\0';
        }

        return _board[y][x]; 
    }

    // Does what it says on the tin.
    void print_board() const {
        cout << "  ";
        for (ushort x = 0; x < size; x++) cout << x + 1 << ' ';
        cout << '\n';

        for (ushort y = 0; y < size; y++) {
            cout << (char)('A' + y) << ' '; //char means turn it into char
            for (ushort x = 0; x < size; x++) cout << _board[y][x] << ' ';
            cout << '\n';
        }
    }

    // Checks the game state, returning "INCOMPLETE," "ORDER_WIN," or "CHAOS_WIN."
    Game_State check_gamestate() const {
        verify_board(); // Can't be too safe!

        // Check for five in a row in any direction by casting a "ray" in each direction
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++) {
                if (_board[y][x] == '*') continue; //continue means skip the rest of the loop

                // Check south
                if (y + 4 < size) // Check to see if there's actually enough room for a line
                    for (ushort i = y, len = 1; i < size - 1; i++) {
                        if (_board[i][x] == _board[i + 1][x]) len++;
                        else break;
                        if (len >= 5) return Game_State::ORDER_WIN; 
                    }

                // Check east
                if (x + 4 < size)
                    for (ushort i = x, len = 1; i < size - 1; i++) {
                        if (_board[y][i] == _board[y][i + 1]) len++;
                        else break;
                        if (len >= 5) return Game_State::ORDER_WIN;
                    }

                // Check southeast
                if (x + 4 < size && y + 4 < size)
                    for (ushort i = x, j = y, len = 1; i < size - 1 && j < size - 1; i++, j++) {
                        if (_board[j][i] == _board[j + 1][i + 1]) len++;
                        else break;
                        if (len >= 5) return Game_State::ORDER_WIN;
                    }

                // Check southwest
                if (x >= 4 && y + 4 < size)
                    for (ushort i = x, j = y, len = 1; i > 0 && j < size - 1; i--, j++) {
                        if (_board[j][i] == _board[j + 1][i - 1]) len++;
                        else break;
                        if (len >= 5) return Game_State::ORDER_WIN;
                    }
            }
        
        // If all cells are used, Chaos wins. If not, the game is incomplete.
        return used == area ? Game_State::CHAOS_WIN : Game_State::INCOMPLETE;
    }

    // Returns a heap-allocated array of all free cells on the board. You must free this!
    Point* get_free_cells() const {
        Point* free_cells = new Point[area - used];
        ushort index = 0;

        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                if (_board[y][x] == '*') {
                    free_cells[index].x = x;
                    free_cells[index].y = y;
                    index++;
                }

        return free_cells;
    }

    void reset_board() {
        for (ushort y = 0; y < size; y++)
            for (ushort x = 0; x < size; x++)
                _board[y][x] = '*';

        _used = 0;
    }

    // Returns true if the piece was successfully added, false if not.
    // Throws exceptions if piece is invalid or cell is out of bounds.
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
}; //end of Board class

Move prompt_player_move(const char* prompt, const Board& board);
Move ai_random(const Board& board);
Move ai_clever(const Board& board);

int main(int argc, char** argv) { //char** is a pointer to an array of pointers to char
    // This should NEVER be done in the real world
    argv = nullptr; //argv points to nothing
    srand(time((long int*)argv)); // this generates a random number stored in the variable rand

    ushort board_size = 0;
    uint turn_count = 0; 
    const Player_Role human_player_role = rand() % 2 ? Player_Role::ORDER : Player_Role::CHAOS;
    const Player_Role first_player = rand() % 2 ? Player_Role::ORDER : Player_Role::CHAOS;

    // Skip introduction if the game has been restarted
    if (argc != -1) {
        // Assign the entire text to a string variable
        string gameDescription = "Welcome to Order and Chaos!\n\n"
        "Game Description:\n"
        "Order and Chaos is a 2-player game played on a grid of squares. One player is Order,\n"
        "and the other is Chaos. They take turns placing either an X or an O in an empty square.\n"
        "Order wins if they make 5 Xs or Os in a line (horizontally, vertically, or diagonally),\n"
        "and Chaos wins if they can prevent this.\n\n"
        "Gameplay:\n"
        "- The game is played on a 6x6 grid by default, but you can also choose grids of size\n"
        "  7x7, 8x8, or 9x9.\n"
        "- You will be randomly assigned to be either Order or Chaos.\n"
        "- The human player moves first or second at random.\n"
        "- To make a move, enter the appropriate coordinates of the square you want to place your piece in.\n"
        "- The game can end in the following ways:\n"
        "  1. The human resigns by typing \"quit\" or \"concede\". In this case, the human loses, and the computer wins.\n"
        "  2. Order makes 5 Xs or Os in a line. In this case, Order wins, and Chaos loses.\n"
        "  3. There are no empty squares left on the board, and there is no line of 5 Xs or Os.\n"
        "     In this case, Chaos wins, and Order loses.\n"
        "- After the game ends, a message will be displayed indicating who won.\n\n"
        "Good luck and have fun playing Order and Chaos!\n";
        

    cout << gameDescription << endl;
    }

    // Determine board size
    while (board_size < 6 || board_size > 9) {
        cout << "Please choose appropriate size for board game: 6, 7, 8, or 9: ";
        cin >> board_size;
        if(cin.fail()) { //prevent user from entering a string
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            board_size = 0;
        }
        else {
            if (board_size >= 6 && board_size <= 9) 
            break;
            cout << "Invalid input.\n";
        }
    }

    Board game(board_size);
    Game_State gs = game.check_gamestate(); //initalized to incomplete

    cout << "You are playing as " << (human_player_role == Player_Role::ORDER ? "Order" : "Chaos")
         << (first_player == human_player_role ? " - Human" : " - Computer") << " plays first.\n";

    // Show a blank board if human goes first
    if (first_player == human_player_role) game.print_board();
    cout << '\n';

    // Main game loop
    while (gs == Game_State::INCOMPLETE) {
        // Player 1 turn
        // If the first player is human, prompt them for a move. Otherwise, use the AI.
        if (first_player == human_player_role) 
            cout << (human_player_role == Player_Role::ORDER ? "[ORDER]: " : "[CHAOS]: ");

        Move player_1_move = human_player_role == first_player ?
            prompt_player_move("Enter a move [NumberLetter(X/O)]: ", game) : ai_random(game);

        // If the player entered a null move, they conceded.
        // conceded means they gave up
        //does not work
        if (player_1_move.piece == '\0') { //
            cout << "You conceded after " << turn_count << " turn(s). Good game.\n";
            break;
        }

        // If the first player is NOT human, print the computer's move.
        if (human_player_role != first_player) {
            cout << "Computer plays " << player_1_move.piece << " at "
                 << player_1_move.cell.x + 1 << (char)(player_1_move.cell.y + 'A') << '\n';
        }

        game.add_piece(player_1_move); 
        game.print_board();
        cout << '\n';

        gs = game.check_gamestate();
        if (gs != Game_State::INCOMPLETE) break; //breaks out of look if game is not incomplete

        // Player 2 turn
        // If the first player ISN'T human, prompt them for a move. Otherwise, use the AI.
        if (first_player != human_player_role)
            cout << (human_player_role == Player_Role::ORDER ? "[ORDER]: " : "[CHAOS]: ");

        Move player_2_move = human_player_role != first_player ?
            prompt_player_move("Enter a move [NumberLetter(X/O)]: ", game) : ai_random(game);

        if (player_1_move.piece == '\0' || player_2_move.piece == '\0') { 
            cout << "You conceded after " << turn_count << " turn(s). Good game.\n";
            break;
        }

        if (human_player_role == first_player) {
            cout << "Computer plays " << player_1_move.piece << " at "
                 << player_1_move.cell.x + 1 << (char)(player_1_move.cell.y + 1 + 'A') << '\n';
        }

        game.add_piece(player_2_move);
        game.print_board();
        cout << '\n';

        turn_count++;
        gs = game.check_gamestate();
    }

    switch (gs) {
        case Game_State::ORDER_WIN:
            cout << "Order wins!\n"
                 << (human_player_role == Player_Role::ORDER ? "You won" : "You lost") << " in "
                 << turn_count << " turns. Good game" 
                 << (human_player_role == Player_Role::ORDER ? ", well played!" : ".") << '\n';
            break;
        
        case Game_State::CHAOS_WIN:
            cout << "Chaos wins!\n"
                 << (human_player_role == Player_Role::CHAOS ? "You won in " : "You lost in ")
                 << turn_count << " turns. Good game" 
                 << (human_player_role == Player_Role::CHAOS ? ", well played!" : ".") << '\n';
            break;
        
        default: break;
    }

    char play_again = 'N';
    cout << "Play again? (y/N): ";
    cin >> play_again;
    if (play_again == 'y' || play_again == 'Y')
        return main(-1, nullptr); // Bad, but kinda funny.
        return 0;
}

// Prompts the player for a move and returns it, repeadedly asking until a valid move is entered.
// Returns a move with piece '\0' if the player quits.
Move prompt_player_move(const char* prompt, const Board& board) {
    string player_move_str = "";
    Move player_move = {'\0', {0, 0}};

    while (true) {
        cout << prompt;
        cin >> player_move_str; //numberletterX or O
        for (char& c : player_move_str) 
        {
            if(isalpha(c))
               c = toupper(c); //pass by reference because we are changing the value
        }
        if (player_move_str == "CONCEDE" || player_move_str == "QUIT") return {'\0', {0, 0}};

        // Validate input and break if valid
        if (player_move_str.length() == 3)
            if (isdigit(player_move_str.at(0)) && isalpha(player_move_str.at(1)))
                if (player_move_str.at(2) == 'X' || player_move_str.at(2) == 'O') break;
        cout << "Invalid input.\n";
    }

    player_move = {
        player_move_str.at(2), {
            (ushort)(player_move_str.at(0) - '0' - 1), //char into int, then subtracts 1 
            (ushort)(player_move_str.at(1) - 'A')
        }
    };

    // Ask again if the cell is already occupied or if it's out of bounds
    if (player_move.cell.x >= board.size || player_move.cell.y >= board.size) {
        cout << "Cell out of bounds.\n";
        return prompt_player_move(prompt, board); //recursion
    }

    if (board.get_cell(player_move.cell.x, player_move.cell.y) != '*') {
        cout << "Cell already occupied.\n";
        return prompt_player_move(prompt, board);
    }
    
    return player_move;
}

// Loads the coordinates of each empty cell on the board and places a random piece on one of them.
// Returns the coordinates of the piece placed.
Move ai_random(const Board& board) {
    if (board.used == board.area) {
        throw runtime_error("Tried to play on a full board!");
        return {'\0', {0, 0}};
    }

    Point* free_cells = board.get_free_cells();

    Move m = {
        rand() % 2 ? 'X' : 'O',
        free_cells[rand() % (board.area - board.used)]
    };

    delete[] free_cells; // Free the memory allocated by get_free_cells()
    return m;
}

Move ai_clever(const Board& board) {
    if (board.used == board.area) {
        throw runtime_error("Tried to play on a full board!");
        return {'\0', {0, 0}};
    }

    Point* free_cells = board.get_free_cells(); 

    Move m = {'*', {0, 0}};

    delete[] free_cells;
    return m;
}

/*
Assignment 5 Report
===================
Description of Computers Playing Strategy
-----------------------------------------
ai_random:
    Given an array of empty cells on a board, it will choose one at random and place a random
    piece on it.

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