#ifndef CHESSPPPIECE_H
#define CHESSPPPIECE_H

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

/* Piece Data Memebers
 * unsigned int me
 * bit  0 -  2 piece type
 * bit  3 -  5 pos x
 * bit  6 -  8 pos y
 * bit  9      piece color
 * bit 10      moved
 * bit 11      captured
 * bit 12      causing check
 * bit 13      in check
 * bit 14      un-used
 * bit 15      un-used
 * bit 16      un-used
 * bit 17      un-used
 * bit 18      un-used
 * bit 19      un-used
 * bit 20      un-used
 * bit 21      un-used
 * bit 22      un-used
 * bit 23      un-used
 * bit 24      un-used
 * bit 25      un-used
 * bit 26      un-used
 * bit 27      un-used
 * bit 28      un-used
 * bit 29      un-used
 * bit 30      un-used
 * bit 31      un-used
*/

/* Piece Defines
 *
*/
#define SHFT_TYP  0
#define SHFT_PSX  3
#define SHFT_PSY  6
#define SHFT_CLR  9
#define SHFT_MVD 10
#define SHFT_CPT 11
#define SHFT_CHK 12
#define SHFT_CKD 13

#define MASK_TYP (7 << SHFT_TYP)
#define MASK_PSX (7 << SHFT_PSX)
#define MASK_PSY (7 << SHFT_PSY)
#define MASK_CLR (1 << SHFT_CLR)
#define MASK_MVD (1 << SHFT_MVD)
#define MASK_CPT (1 << SHFT_CPT)
#define MASK_CHK (1 << SHFT_CHK)
#define MASK_CKD (1 << SHFT_CKD)

#define WHITE_KING 20
#define BLACK_KING 4

/* Math shortcuts
 */
#define IsValid(x) (x >= MIN_POS && x <= MAX_POS)

 

enum Piece_Type{
    P_NONE = 0,
    PAWN   = 1,
    ROOK   = 2,
    KNIGHT = 3,
    BISHOP = 4,
    QUEEN  = 5,
    KING   = 6
};
enum Piece_Color{
    WHITE  = 0,
    BLACK  = 1
};
enum Board_Map{
  a8 = 0, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1
};
enum Board_Direction{
    D_NONE = 0,
    NORTH  = 1,
    EAST   = NORTH << 1,
    SOUTH  = EAST  << 1,
    WEST   = SOUTH << 1,
    NORTH_EAST = NORTH | EAST,
    SOUTH_EAST = SOUTH | EAST,
    SOUTH_WEST = SOUTH | WEST,
    NORTH_WEST = NORTH | WEST
};

//class Board;

class Piece{
private:
    std::bitset<BOARD_SIZE> m_moves;
    unsigned int me;
    
public:

    // General Get Methods
    int  type(void)       const { return (me & MASK_TYP ) >> SHFT_TYP;   }
    int  color(void)      const { return (me & MASK_CLR ) >> SHFT_CLR;   }
    bool moved(void)      const { return (me & MASK_MVD ) >> SHFT_MVD;   }
    bool captured(void)   const { return (me & MASK_CPT ) >> SHFT_CPT;   }
    int  xPos(void)       const { return (me & MASK_PSX ) >> SHFT_PSX;   }
    int  yPos(void)       const { return (me & MASK_PSY ) >> SHFT_PSY;   }
    bool checking(void)   const { return (me & MASK_CHK ) >> SHFT_CHK;   }
    bool checked(void)    const { return (me & MASK_CKD ) >> SHFT_CKD;   }
    int  arrPos(void)     const { return ( xPos() + ( yPos() * WIDTH ) );}//FIX
    
    const std::bitset<BOARD_SIZE>& moves_with_friends(void) const
    {
        return m_moves;
    }
    
    const std::bitset<BOARD_SIZE> moves_actual(const Piece* pieceList) const;
    
    
    
    Piece(Board_Map pos, Piece_Type type, Piece_Color color = WHITE);
    Piece(const Piece& other);
    Piece(void);
    void generate(Board_Map pos, Piece_Type type, Piece_Color color = WHITE);

    // MAKE MOVES !
    std::bitset<BOARD_SIZE> moves(const Piece* pieceList);  // Return moves without same color
    
    void update(Piece* pieces, Piece* board[]);    
    void pawn_moves(Piece* pieces, Piece* board[]);
    void rook_moves(Piece* pieces, Piece* board[]);
    void knight_moves(Piece* pieces, Piece* board[]);
    void bishop_moves(Piece* pieces, Piece* board[]);
    void queen_moves(Piece* pieces, Piece* board[]);
    void king_moves(Piece* pieces, Piece* board[]);

    void king_checked(Piece* pieces, Piece* board[], std::bitset<BOARD_SIZE>& check_mask);
    
    
    void shootPath(Board_Direction d, Piece* board[], std::bitset<BOARD_SIZE>& set, Piece& piece);
    void addPos(Piece* board[], int x, int y);

    void printSet(std::bitset<BOARD_SIZE>& set, std::ostream& out = std::cout);    
    
    // END of making moves
    
    void checkPinned(Piece* pieces, Piece* board[]);
    void pinPath(Piece* pieces, Piece* board[], Board_Direction d);
    void andMoves(std::bitset<BOARD_SIZE> mask);
    

    // Never mutate
    bool diagonal(void);
    bool horizontal(void);


    // Mutated

    void setCheck(bool c);

    bool move(Board_Map pos);
    void setMoved(bool move);

    // Capturing Functions
    void capture(void);
    std::bitset<BOARD_SIZE> makeCheckMask(Piece* pieces, Piece* board[]);

    // overloads
    inline Piece& operator=(const Piece& other)
    { this->me = other.me; return *this; }

    // Fancy console things
    const char* type_str(void) const;
    const char* color_str(void) const;

    static const char* Piece_Type_Str [];
    static const char* Piece_Color_Str[];
    friend std::ostream& operator<<(std::ostream& out, const Piece& piece);
    char asChar(void) const;
};




#endif
