#include "Chess.hpp"

chesspp::Game::Game(void)
	:m_pieces(NULL)
{
    reset();
}
chesspp::Game::~Game(void)
{
	reset();
}

	
void chesspp::Game::reset(void)
{

	delete [] m_pieces;
	m_pieces = NULL;
    
    for (unsigned int i = 0 ; i < BOARD_SIZE; i++)
		m_board[i] = NULL;
}

bool chesspp::Game::updateMoves(Piece_Color color)
{
	// Update the moves of this color
	unsigned int offset = color == WHITE ? PIECE_COUNT / 2 : 0;
	for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
	{
		if (m_pieces[i].captured()) continue;
		m_pieces[i].update(m_pieces, m_board);
	}

	// Remove moves from pinned pieces
	m_pieces[color == WHITE ? WHITE_KING : BLACK_KING].checkPinned(m_pieces, m_board);


	// See that the color has a move
	bool hasMoves = false;
	for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
	{
		if (m_pieces[i].captured())
			continue;
		if (m_pieces[i].moves_actual(m_pieces).any())
		{
			hasMoves = true;
			break;
		}
	}
	return hasMoves;
}

void chesspp::Game::newGame(void){
	reset();    
    m_pieces = new Piece[PIECE_COUNT];
	turn = WHITE;

	m_pieces[ 0].generate(a8, ROOK  , BLACK);
	m_pieces[ 1].generate(b8, KNIGHT, BLACK);
	m_pieces[ 2].generate(c8, BISHOP, BLACK);
	m_pieces[ 3].generate(d8, QUEEN , BLACK);
	m_pieces[ 4].generate(e8, KING  , BLACK);
	m_pieces[ 5].generate(f8, BISHOP, BLACK);
	m_pieces[ 6].generate(g8, KNIGHT, BLACK);
	m_pieces[ 7].generate(h8, ROOK, BLACK);
		
	m_pieces[ 8].generate(a7, PAWN, BLACK);
	m_pieces[ 9].generate(b7, PAWN, BLACK);
	m_pieces[10].generate(c7, PAWN, BLACK);
	m_pieces[11].generate(d7, PAWN, BLACK);
	m_pieces[12].generate(e7, PAWN, BLACK);
	m_pieces[13].generate(f7, PAWN, BLACK);
	m_pieces[14].generate(g7, PAWN, BLACK);
	m_pieces[15].generate(h7, PAWN, BLACK);
		
	m_pieces[16].generate(a1, ROOK  , WHITE);
	m_pieces[17].generate(b1, KNIGHT, WHITE);
	m_pieces[18].generate(c1, BISHOP, WHITE);
	m_pieces[19].generate(d1, QUEEN , WHITE);
	m_pieces[20].generate(e1, KING  , WHITE);
	m_pieces[21].generate(f1, BISHOP, WHITE);
	m_pieces[22].generate(g1, KNIGHT, WHITE);
	m_pieces[23].generate(h1, ROOK, WHITE);
		
	m_pieces[24].generate(a2, PAWN, WHITE);
	m_pieces[25].generate(b2, PAWN, WHITE);
	m_pieces[26].generate(c2, PAWN, WHITE);
	m_pieces[27].generate(d2, PAWN, WHITE);
	m_pieces[28].generate(e2, PAWN, WHITE);
	m_pieces[29].generate(f2, PAWN, WHITE);
	m_pieces[30].generate(g2, PAWN, WHITE);
	m_pieces[31].generate(h2, PAWN, WHITE);
	
	// Create the board, then update moves.
	// Can't be done in same loop, this would update moves with an un-filled board
  	std::for_each(m_pieces, m_pieces + PIECE_COUNT, [&](Piece& p)	{
		m_board[p.arrPos()] = &p;
	});

	updateMoves(turn);
	Piece_Color temp = turn == WHITE ? BLACK : WHITE;
	updateMoves(temp);
}
void chesspp::Game::newGame(std::ifstream& in){
	reset();
	turn = WHITE;
	m_pieces = new Piece[PIECE_COUNT];
	
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
		m_pieces[i].generate(t_map, t_type, t_color);
	}	
		
	std::for_each(m_pieces, m_pieces + PIECE_COUNT, [&](Piece& p)	{
		m_board[p.arrPos()] = &p;
	});
	
	Piece_Color temp = turn;
	updateMoves(temp);
	temp = temp == WHITE ? BLACK : WHITE;
	updateMoves(temp);
}

bool chesspp::Game::makeMove(Board_Map from, Board_Map to){
	if  (from == to)
		return false;

	Piece* toMove = m_board[from];
	if (turn != toMove->color())
		return false;

	bool pieceMoved = toMove->moved();

	if (!toMove->move(to))
		return false;
	if (!board_move(from, to))
	{
		toMove->move(from);
		if (!pieceMoved) toMove->setMoved(false);
		return false;
	}

	// After a valid move, the king shouldn't be in check anymore
	// 4 and 24 are being used more and more often, I might define these
	// As well as the positions of the other pieces... not sure
	// Anyway, this is mostly to see if the makeCheckMask funtion is working
	if (m_pieces[WHITE_KING].checked())  m_pieces[WHITE_KING].setCheck(false);
	if (m_pieces[BLACK_KING].checked())  m_pieces[BLACK_KING].setCheck(false);

	
	turn = turn == WHITE? BLACK : WHITE;
	return true;
}

bool chesspp::Game::updateMoves()
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

Piece_Color chesspp::Game::getTurn(void) const
{
	return turn;
}


bool chesspp::Game::board_move(Board_Map from, Board_Map to)
{
	if (!m_board[from] || m_board[from]->captured()) return false;
	
	if (m_board[to]) m_board[to]->capture();
	
	m_board[to] = m_board[from];
	m_board[from] = NULL;
	return true;
}




