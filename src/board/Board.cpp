#include "chessgame.h"

Board::Board(void){
	this->reset();
}
void Board::reset(void){
	for (int i = 0 ; i < BOARD_SIZE; i++)
		board[i] = NULL;
}
std::ostream& operator<<(std::ostream& out, const Board& board){
	Piece* p;
	out << std::endl;
	for (int i = 0; i < WIDTH; i++)
	{
		out << WIDTH - i  << " | ";
		for (int j = 0; j < WIDTH; j++)
		{
			p = board.board[i * WIDTH + j];
			if (p && !p->captured())
				out << board.board[i * WIDTH + j]->asChar();
			else
				out << '*';
			out << ' ';
		}
		out << '\n';
	}
	return out << "-------------------\n"
	           << "    a b c d e f g h"  << std::endl;
}

Piece** Board::operator[](int at)
{
	return &(board[at]);
}

bool Board::move(Board_Map from, Board_Map to)
{
	if (!board[from]) return false;
	
	if (board[to]) board[to]->capture();
	
	board[to] = board[from];
	board[from] = NULL;
	return true;
}

//
