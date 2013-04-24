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

                board_image = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_board()));
                piece_image = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_pieces()));
                validMove_image = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_validMove()));
                enemyMove_image = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_enemyMove()));
                captureMove_image = sf::Sprite(TextureManager::getInstance().Load(graphicsConfig.getSpritePath_captureMove()));

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
            display->draw(board_image);
        }
        void GraphicsHandler::drawPiece(const Piece &p)
        {
            piece_image.setTextureRect(sf::IntRect(p.type()*cell_size, p.color()*cell_size, cell_size, cell_size));
            drawSpriteAtCell(piece_image, p.xPos(), p.yPos());
        }
        void GraphicsHandler::drawPieceAt(Piece *p, const sf::Vector2i &pos)
        {
            piece_image.setTextureRect(sf::IntRect(p->type()*cell_size, p->color()*cell_size, cell_size, cell_size));
            piece_image.setPosition(pos.x - (cell_size / 2), pos.y - (cell_size / 2));

            display->draw(piece_image);
        }
        void GraphicsHandler::drawValidMove(const int x, const int y)
        {
            drawSpriteAtCell(validMove_image, x, y);
        }
        void GraphicsHandler::drawBoard(const Piece *piece_array)
        {
            std::cout << "GraphicsHandler::drawBoard does nothing\n";
        }
        
        void GraphicsHandler::drawPieceImage(sf::IntRect image_section, int x, int y)
        {
            piece_image.setTextureRect(image_section);
            piece_image.setPosition(x, y);
            display->draw(piece_image);
        }
        
        void GraphicsHandler::drawValidImage(int x, int y)
        {
            validMove_image.setPosition(x, y);
            display->draw(validMove_image);
        
        }
        void GraphicsHandler::drawEnemyImage(int x, int y)
        {
            enemyMove_image.setPosition(x, y);
            display->draw(enemyMove_image);
        
        }
        void GraphicsHandler::drawCaptureImage(int x, int y)
        {
            captureMove_image.setPosition(x, y);
            display->draw(captureMove_image);        
        }
    }
}
