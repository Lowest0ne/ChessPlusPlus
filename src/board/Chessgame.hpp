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


class Board{
private:
	Piece *board[BOARD_SIZE];

public:
	Board(void);

	void reset();

	bool move(Board_Map from, Board_Map to);

	friend std::ostream& operator<<(std::ostream& out, const Board& board);
	Piece** operator[](int at);
	
	Piece** get(void) { return board; }	
};
class Game{
private:
	Piece* pieceList;
	Board  board;
	Piece_Color turn;

	void reset(void);

	bool updateMoves(Piece_Color color);

public:
	Game(void);
	~Game(void);

	void newGame(void);
	void newGame(std::ifstream& in);

	bool makeMove(Board_Map from, Board_Map to);

	bool updateMoves();

	Piece_Color getTurn(void) const;

	void writePieceList(std::ostream& out = std::cout);

	friend std::ostream& operator<<(std::ostream& out, const Game& game);

	// For Quick UI
	const Piece* getPieces(void) const { return pieceList; }
	Piece** getBoard(void)  { return board.get(); }
};

#endif