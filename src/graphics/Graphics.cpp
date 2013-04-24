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
            drawBackground();
            
            for (unsigned int i = 0; i < PIECE_COUNT; i++)
            {
                // Draw the selected piece last, so that it is on top
                if (piece_array[i].captured())// || (pieceSelected && currentPiece == &piece_array[i]))
                        continue;

                int texture_x = 0;
                int texture_y = 0;
                switch(piece_array[i].type())
                {
                case PAWN  : texture_x = 0; break;
                case ROOK  : texture_x = 1; break;
                case KNIGHT: texture_x = 2; break;
                case BISHOP: texture_x = 3; break;
                case QUEEN : texture_x = 4; break;
                case KING  : texture_x = 5; break;

                default: break;
                }
                texture_y = piece_array[i].color();
                piece_image.setTextureRect(sf::IntRect(texture_x * cell_size, texture_y * cell_size, cell_size, cell_size));
                piece_image.setPosition(piece_array[i].xPos() * cell_size, piece_array[i].yPos() *cell_size);
                display->draw(piece_image); 
            }


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
