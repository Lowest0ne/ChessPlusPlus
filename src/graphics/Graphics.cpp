#include "Graphics.hpp"

namespace chesspp
{
    namespace graphics
    {
        GraphicsHandler::GraphicsHandler( sf::RenderWindow *_display ) : display(_display)
        {
            try 
            {
                config::BoardConfig boardConfig;
                config::GraphicsConfig graphicsConfig;

                board = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_board()));
                pieces = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_pieces()));
                validMove = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_validMove()));

                cell_size = boardConfig.getCellWidth();
            }
            catch(Exception &e) 
            {
#ifdef _DEBUG
                cout << "Error: " << e.what() << endl;
#endif
            }
        }

        void GraphicsHandler::drawSpriteAtCell(sf::Sprite &s, const int x, const int y)
        {
            s.setPosition(x * cell_size, y * cell_size);
            display->draw(s);
        }
        void GraphicsHandler::drawBackground()
        {
            display->draw(board);
        }
        void GraphicsHandler::drawPiece(Piece *p)
        {
            pieces.setTextureRect(sf::IntRect(p->type()*cell_size, p->color()*cell_size, cell_size, cell_size));
            drawSpriteAtCell(pieces, p->xPos(), p->yPos());
        }
        void GraphicsHandler::drawPieceAt(Piece *p, const sf::Vector2i &pos)
        {
            pieces.setTextureRect(sf::IntRect(p->type()*cell_size, p->color()*cell_size, cell_size, cell_size));
            pieces.setPosition(pos.x - (cell_size / 2), pos.y - (cell_size / 2));

            display->draw(pieces);
        }
        void GraphicsHandler::drawValidMove(const int x, const int y)
        {
            drawSpriteAtCell(validMove, x, y);
        }
        void GraphicsHandler::drawBoard(Piece **b)
        {
            drawBackground();


        }
    }
}
