#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <bitset>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Piece.hpp"

/*  Game Constants
 *
*/
#define MAX_POS 7
#define MIN_POS 0
#define WIDTH (MAX_POS + 1)
#define BOARD_SIZE (WIDTH * WIDTH)
#define PIECE_COUNT 32

namespace chesspp
{
    class Game{
    private:

        /*An array of pieces of size PIECE_COUNT
         *Pieces are stored in a strict order, which allows for quick access of
         *specific pieces, as well as all pieces of a certain color
        */
        Piece* m_pieces;

        /*An array of Piece* of size BOARD_SIZE
         *This array must be updated whenever a piece moves (as well as the
         *piece's actual position.
         *It provides a way to know the piece on a specific part of the board
         */
        Piece* m_board[BOARD_SIZE];

        /*Who's tuen is it?
        */
        Piece_Color turn;

        /*Frees all memory from m_pieces, as well as clears m_board
         *called by destructor, as well as newGame()
        */
        void reset(void);

        /*Given a color, update that color's moves
         *They are separated to allow the color that has just moved to be
         *calculated first.  A piece's move might have unblocked a check
        */
        bool updateMoves(Piece_Color color);

        /*move the Piece on m_board
        */
        bool board_move(Board_Map from, Board_Map to);

    public:
        Game(void);
        ~Game(void);

        // Create a new game, free mem form old game, shine the pieces etc...
        void newGame(void);

        /*Technically works, check out res/games/new_gameV2.txt
         *Ideally, moves are stored in a list.  The list could be the save-game
         *Thus, all Chess games construct to a new game, then load from the file
         *the moves
        */
        void newGame(std::ifstream& in);

        /*From and To are coming from mouse clicks and may not be valid at all
         *This returns true if the args were valid and the move was done
        */
        bool makeMove(Board_Map from, Board_Map to);

        /*This function updates the moves for each piece
         *It returns false if a color has no moves, signalling checkmate
        */
        bool updateMoves();

        /*Who's tuen is it?
        */
        Piece_Color getTurn(void) const;

        // For Quick UI
        const Piece* getPieces(void) const { return m_pieces; }
        Piece** getBoard(void)  { return m_board; } // Working on const
    };

}

#endif