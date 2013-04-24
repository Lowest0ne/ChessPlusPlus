#include "AppState.hpp"

namespace chesspp
{
    AppStateGame::AppStateGame( Application* _app, sf::RenderWindow *_display )
        : AppState(_display)
        , app(_app)
        , graphics(display)
    {
        currentPiece = NULL;
        pieceSelected = false;
        updateMoves = false;
        chess.newGame();
    }

    int AppStateGame::id() { return 1; }

    void AppStateGame::OnRender()
    {
        graphics.drawBackground();

		static const int SIZE = 80;  // To Be defined somwhere

		// Valid moves are drawn for the piece being hovered over
		// Or the piece that is currently selected
		// Draw them first so that they are below the pieces
		// We need the board to know if the valid move is an enemy
		Piece** board = chess.getBoard();
		if (currentPiece)
		{
			const std::bitset<BOARD_SIZE> moves = currentPiece->moves(chess.getPieces());
			for (unsigned int x = 0; x < WIDTH; x++)
			{
				for (unsigned int y = 0; y < WIDTH; y++)
				{
					unsigned int t_arrPos = x + y * WIDTH;
					if (!moves[t_arrPos]) continue;

					if (currentPiece->color() == chess.getTurn())
					{
						if (board[t_arrPos] && board[t_arrPos]->color() != currentPiece->color())
						{
                            graphics.drawCaptureImage( x * SIZE, y * SIZE);
						}
						else
						{
                            graphics.drawValidImage( x * SIZE, y * SIZE);
						}
					}
					else
					{
                        graphics.drawEnemyImage( x * SIZE, y * SIZE);
					}

				}
			}
		}

		const Piece* pieceList = chess.getPieces();
		for (unsigned int i = 0; i < PIECE_COUNT; i++)
		{
			// Draw the selected piece last, so that it is on top
			if (pieceList[i].captured() || (pieceSelected && currentPiece == &pieceList[i]))
					continue;

			int texture_x = 0;
			int texture_y = 0;
			switch(pieceList[i].type())
			{
			case PAWN  : texture_x = 0; break;
			case ROOK  : texture_x = 1; break;
			case KNIGHT: texture_x = 2; break;
			case BISHOP: texture_x = 3; break;
			case QUEEN : texture_x = 4; break;
			case KING  : texture_x = 5; break;

			default: break;
			}
			texture_y = pieceList[i].color();
            sf::IntRect section(texture_x * SIZE, texture_y * SIZE, SIZE, SIZE);
            
            graphics.drawPieceImage(section, pieceList[i].xPos() * SIZE, pieceList[i].yPos() * SIZE);
            
		}


		// Draw the selected Piece
		if (pieceSelected && currentPiece)
		{
			
		int texture_x = 0;
			switch(currentPiece->type())
			{
			case PAWN  : texture_x = 0; break;
			case ROOK  : texture_x = 1; break;
			case KNIGHT: texture_x = 2; break;
			case BISHOP: texture_x = 3; break;
			case QUEEN : texture_x = 4; break;
			case KING  : texture_x = 5; break;

			default: break;
			}
            
            sf::IntRect section(texture_x * SIZE, currentPiece->color() * SIZE, SIZE, SIZE);
            sf::Vector2i mPos = sf::Mouse::getPosition(*display);
            
            graphics.drawPieceImage(section, mPos.x - (SIZE / 2), mPos.y - (SIZE / 2));
		}
		
		// Once everything is drawn, then do logic
		if (updateMoves)
		{
			chess.updateMoves();
			updateMoves = false;
		}   
        
        
    }

    void AppStateGame::OnLButtonPressed(int x, int y)
    {
        if (currentPiece) pieceSelected = true;
    }
    void AppStateGame::OnMouseMoved(int x, int y)
    {
			if (pieceSelected) return;

			x /= 80;  // cell_size not defined
			y /= 80;  // cell_size not defined
			int t_arrPos = x + y * WIDTH;

			if (chess.getBoard()[t_arrPos])
			{
				currentPiece = chess.getBoard()[t_arrPos];
			}
			else
			{
				currentPiece = NULL;
			}
        //board->setCurrent(x,y);
    }
    void AppStateGame::OnLButtonReleased(int x, int y)
    {
			static const int screenSizeX = 640;
			static const int screenSizeY = 640;
			static const int sizeX = 8;
			static const int sizeY = 8;
            static const int width = 8;

			int xPos = (( x * sizeX) / screenSizeX);
			int yPos = (( y * sizeY) / screenSizeY);

			if (pieceSelected)
			{
				updateMoves = chess.makeMove((Board_Map)currentPiece->arrPos(), (Board_Map)(xPos + yPos * width));
				pieceSelected = false;
				currentPiece = NULL;
			}
    }
}
