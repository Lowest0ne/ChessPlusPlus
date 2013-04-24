#include "Chessgame.hpp"

Game::Game(void)
	:pieceList(NULL)
    ,turn(WHITE)
{
    
}
Game::~Game(void){
	this->reset();
}

	
void Game::reset(void){
	delete [] this->pieceList;
	pieceList = NULL;
	this->board.reset();
}


void Game::newGame(void){
	reset();    
    pieceList = new Piece[PIECE_COUNT];
	turn = WHITE;

	pieceList[ 0].generate(a8, ROOK  , BLACK);
	pieceList[ 1].generate(b8, KNIGHT, BLACK);
	pieceList[ 2].generate(c8, BISHOP, BLACK);
	pieceList[ 3].generate(d8, QUEEN , BLACK);
	pieceList[ 4].generate(e8, KING  , BLACK);
	pieceList[ 5].generate(f8, BISHOP, BLACK);
	pieceList[ 6].generate(g8, KNIGHT, BLACK);
	pieceList[ 7].generate(h8, ROOK, BLACK);
		
	pieceList[ 8].generate(a7, PAWN, BLACK);
	pieceList[ 9].generate(b7, PAWN, BLACK);
	pieceList[10].generate(c7, PAWN, BLACK);
	pieceList[11].generate(d7, PAWN, BLACK);
	pieceList[12].generate(e7, PAWN, BLACK);
	pieceList[13].generate(f7, PAWN, BLACK);
	pieceList[14].generate(g7, PAWN, BLACK);
	pieceList[15].generate(h7, PAWN, BLACK);
		
	pieceList[16].generate(a1, ROOK  , WHITE);
	pieceList[17].generate(b1, KNIGHT, WHITE);
	pieceList[18].generate(c1, BISHOP, WHITE);
	pieceList[19].generate(d1, QUEEN , WHITE);
	pieceList[20].generate(e1, KING  , WHITE);
	pieceList[21].generate(f1, BISHOP, WHITE);
	pieceList[22].generate(g1, KNIGHT, WHITE);
	pieceList[23].generate(h1, ROOK, WHITE);
		
	pieceList[24].generate(a2, PAWN, WHITE);
	pieceList[25].generate(b2, PAWN, WHITE);
	pieceList[26].generate(c2, PAWN, WHITE);
	pieceList[27].generate(d2, PAWN, WHITE);
	pieceList[28].generate(e2, PAWN, WHITE);
	pieceList[29].generate(f2, PAWN, WHITE);
	pieceList[30].generate(g2, PAWN, WHITE);
	pieceList[31].generate(h2, PAWN, WHITE);
	
	// Create the board, then update moves.
	// Can't be done in same loop, this would update moves with an un-filled board
  	std::for_each(pieceList, pieceList + PIECE_COUNT, [&](Piece& p)	{
		*(board[p.arrPos()]) = &p;
	});

	updateMoves(turn);
	Piece_Color temp = turn == WHITE ? BLACK : WHITE;
	updateMoves(temp);
}
void Game::newGame(std::ifstream& in){
	reset();
	turn = WHITE;
	pieceList = new Piece[PIECE_COUNT];
	
	in.ignore(80,'\n');
	for (unsigned int i = 0; i < PIECE_COUNT; i++)
	{
		if (!in)
		{
			std::cout << "File Error: Eof / Bad Read\n";
			return;
		}
		Piece_Color t_color;
		switch(in.get())
		{
			case 'B': t_color = BLACK; break;
			case 'W': t_color = WHITE; break;
			default: std::cout << "File Error: COLOR\n"; return;
		}
		Piece_Type t_type;
		switch(in.get())
		{
			case 'P': t_type = PAWN; break;
			case 'R': t_type = ROOK; break;
			case 'N': t_type = KNIGHT; break;
			case 'B': t_type = BISHOP; break;
			case 'Q': t_type = QUEEN; break;
			case 'K': t_type = KING; break;
			default: std::cout << "File Error: TYPE\n"; return;
		}
		in.get();
		int xPos, yPos;
		char junk;
		in >> xPos >> junk >> yPos;
		in.ignore(80, '\n');
		
		Board_Map t_map = (Board_Map)(yPos * WIDTH + xPos);
		pieceList[i].generate(t_map, t_type, t_color);
	}	
		
	std::for_each(pieceList, pieceList + PIECE_COUNT, [&](Piece& p)	{
		*(board[p.arrPos()]) = &p;
	});
	
	Piece_Color temp = turn;
	updateMoves(temp);
	temp = temp == WHITE ? BLACK : WHITE;
	updateMoves(temp);
}

std::ostream& operator<<(std::ostream& out, const Game& game){
	return out << game.board;
}

void Game::writePieceList(std::ostream& out){
	for (int i = 0; i < PIECE_COUNT; i++)
		out << pieceList[i] << std::endl << std::endl;
}

bool Game::makeMove(Board_Map from, Board_Map to){
	if  (from == to)
		return false;


	Piece* toMove = board.get()[from];
	if (turn != toMove->color())
		return false;

	bool pieceMoved = toMove->moved();

	if (!toMove->move(to))
		return false;
	if (!board.move(from, to))
	{
		toMove->move(from);
		if (!pieceMoved) toMove->setMoved(false);
		return false;
	}

	// After a valid move, the king shouldn't be in check anymore
	// 4 and 24 are being used more and more often, I might define these
	// As well as the positions of the other pieces... not sure
	// Anyway, this is mostly to see if the makeCheckMask funtion is working
	if (pieceList[WHITE_KING].checked())  pieceList[WHITE_KING].setCheck(false);
	if (pieceList[BLACK_KING].checked())  pieceList[BLACK_KING].setCheck(false);

	
	turn = turn == WHITE? BLACK : WHITE;
	return true;
}

bool Game::updateMoves()
{
	// newGame should not call this method
	// The color that is updated first is the color that is not active
	// This is because makeMove() will have switched the active color
	// The color before makeMove() needs to be calculated first
	Piece_Color temp = turn == WHITE ? BLACK : WHITE;
	if (updateMoves(temp))
	{
		temp = temp == WHITE ? BLACK : WHITE;
		if (!updateMoves(temp))
			std::cout << temp << " loses\n";
	}
	else		
		std::cout << temp << " loses\n";
	return true; // maybe return false if the game is over :)
}

Piece_Color Game::getTurn(void) const
{
	return turn;
}

bool Game::updateMoves(Piece_Color color)
{
	// Update the moves of this color
	unsigned int offset = color == WHITE ? PIECE_COUNT / 2 : 0;
	for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
	{
		if (pieceList[i].captured()) continue;
		pieceList[i].update(pieceList, board.get());
	}

	// Remove moves from pinned pieces
	pieceList[color == WHITE ? WHITE_KING : BLACK_KING].checkPinned(pieceList, board.get());


	// See that the color has a move
	bool hasMoves = false;
	for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
	{
		if (pieceList[i].captured())
			continue;
		if (pieceList[i].moves().any())
		{
			hasMoves = true;
			break;
		}
	}
	return hasMoves;
}


