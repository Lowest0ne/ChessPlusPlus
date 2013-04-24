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

class Game{
private:
	Piece* pieceList;
    
    Piece* m_board[BOARD_SIZE];
    
	Piece_Color turn;

	void reset(void);

	bool updateMoves(Piece_Color color);
    bool board_move(Board_Map from, Board_Map to);

public:
	Game(void);
	~Game(void);

	void newGame(void);
	void newGame(std::ifstream& in);

	bool makeMove(Board_Map from, Board_Map to);

	bool updateMoves();

	Piece_Color getTurn(void) const;

	// For Quick UI
	const Piece* getPieces(void) const { return pieceList; }
	Piece** getBoard(void)  { return m_board; }
};

#endif