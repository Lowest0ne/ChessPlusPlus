#include "Piece.hpp"

const char* Piece::Piece_Type_Str [] = {"None", "Pawn", "Rook", "Night", "Bishop", "Queen", "King"};
const char* Piece::Piece_Color_Str[] = {"White", "Black"};

Piece::Piece(Board_Map position, Piece_Type type, Piece_Color color)
    :me((type      << SHFT_TYP) +
        (color     << SHFT_CLR) +
        ((position % WIDTH) << SHFT_PSX) +
        ((position / WIDTH) << SHFT_PSY))
{
}
Piece::Piece(const Piece& other) :me(other.me)
{
}
Piece::Piece(void)
    :me(0)
{
}
void Piece::generate(Board_Map position, Piece_Type type, Piece_Color color){
    me = (type      << SHFT_TYP) +
         (color     << SHFT_CLR) +
         ((position % WIDTH) << SHFT_PSX) +
         ((position / WIDTH) << SHFT_PSY)
         ;
}


void Piece::capture(void){
    me |= MASK_CPT;
    m_moves = 0;

}
void Piece::setMoved(bool move){
    me &= ~(MASK_MVD);
    me |= ((move? 1 : 0) << SHFT_MVD);
}

bool Piece::move(Board_Map pos){
    if (pos < MIN_POS || pos >= BOARD_SIZE) return false;
    if (this->captured()) return false;
    if (!m_moves[pos]) return false;

    // Remove the old values
    me &= ~( MASK_PSX | MASK_PSY);
    // Add the new ones
    me |= ((pos%WIDTH) << SHFT_PSX) | ((pos/WIDTH) << SHFT_PSY) | MASK_MVD;
    return true;
}


const char* Piece::type_str(void)const {return Piece_Type_Str[type()];  }
const char* Piece::color_str(void)const{return Piece_Color_Str[color()];}
std::ostream& operator<<(std::ostream& out, const Piece& p){


    return out << p.type_str() << ", " << p.color_str() << ":\n"
               << "(" << p.xPos() << "," << p.yPos() << "," << p.arrPos() << ")\n"
               << "Moved: " << std::boolalpha << p.moved() << '\n'
               << "Captd: " << std::boolalpha << p.captured() << '\n'
               << "Chckg: " << std::boolalpha << p.checking() << '\n'
               << "Chckd: " << std::boolalpha << p.checked() << '\n'
               << std::bitset<32>((unsigned long long)p.me);
}
char Piece::asChar(void) const{
    return (*type_str()) + ( color() * ('a' - 'A'));
}


// MOVING FUNCTIONS

void Piece::update(Piece* pieces, Piece *board[]){
    m_moves = 0;
    switch(this->type())
    {
        case PAWN  : this->pawn_moves(pieces, board);   break;
        case ROOK  : this->rook_moves(pieces, board);   break;
        case KNIGHT: this->knight_moves(pieces, board); break;
        case BISHOP: this->bishop_moves(pieces, board); break;
        case QUEEN : this->queen_moves(pieces, board);  break;
        case KING  : this->king_moves(pieces, board);   break;
        default: std::cout << "Invalid Piece: " << this->type_str() << std::endl;
    }

    // Situation the king is in check
    // This is NOT removing moves that cause check..
    // (A king in check can move to a place that would also be check)
    int myKing = this->color() == WHITE ? WHITE_KING : BLACK_KING;
    std::bitset<BOARD_SIZE> check_mask;
    if (pieces[myKing].checked())
    {
        check_mask = makeCheckMask(pieces, board);
        if (this->type() == KING)
        {
            king_checked(pieces, board, check_mask);
        }
        else
        {
            m_moves &= check_mask;
        }
    }

    // Put the king in check: WIP, though I think it works, not sure how integrated it is
    int k_idx = this->color() == WHITE ? BLACK_KING : WHITE_KING;
    int k_pos = pieces[k_idx].arrPos();
    if (m_moves[k_pos])
    {
        board[k_pos]->setCheck(true);
    }


}
void Piece::pawn_moves  (Piece* pieces, Piece* board[]){
    int offset = this->color() == BLACK ? 1 : -1;
    int m_pos = this->arrPos();

    int t_pos = 0;

    if (this->yPos() == MIN_POS || this->yPos() == MAX_POS)
    {
        std::cout << "Hey, I shouldn't be a pawn anymore\n";
        return;
    }


    if (this->xPos() > MIN_POS)
    {
        t_pos = m_pos  - 1 + (offset * WIDTH);
        if (board[t_pos] && board[t_pos]->color() != this->color())
            m_moves[t_pos] = 1;
    }
    if (this->xPos() < MAX_POS)
    {
        t_pos = m_pos + 1 + (offset * WIDTH);
        if (board[t_pos] && board[t_pos]->color() != this->color())
            m_moves[t_pos] = 1;
    }

    t_pos = m_pos + (offset * WIDTH);
    if (!board[t_pos])
    {
        m_moves[t_pos] = 1;
        if (!moved() && !board[t_pos + (offset * WIDTH)])
            m_moves[t_pos + (offset * WIDTH)] = 1;
    }
}
void Piece::rook_moves  (Piece* pieces, Piece* board[]){

    shootPath(NORTH, board, this->m_moves, *this);
    shootPath(EAST , board, this->m_moves, *this);
    shootPath(SOUTH, board, this->m_moves, *this);
    shootPath(WEST , board, this->m_moves, *this);

}
void Piece::knight_moves(Piece* pieces, Piece* board[]){
    //out << "\nKnight Moves: " << *this << "\n\n";

    addPos(board, -1, -2);
    addPos(board,  1, -2);
    addPos(board,  2, -1);
    addPos(board,  2,  1);
    addPos(board,  1,  2);
    addPos(board, -1,  2);
    addPos(board, -2,  1);
    addPos(board, -2, -1);

    //printSet(m_moves, out);
}
void Piece::bishop_moves(Piece* pieces, Piece* board[]){

    shootPath(NORTH_EAST, board, this->m_moves, *this);
    shootPath(SOUTH_EAST, board, this->m_moves, *this);
    shootPath(SOUTH_WEST, board, this->m_moves, *this);
    shootPath(NORTH_WEST, board, this->m_moves, *this);

}
void Piece::queen_moves (Piece* pieces, Piece* board[]){

    shootPath(NORTH     , board, this->m_moves, *this);
    shootPath(NORTH_EAST, board, this->m_moves, *this);
    shootPath(EAST      , board, this->m_moves, *this);
    shootPath(SOUTH_EAST, board, this->m_moves, *this);
    shootPath(SOUTH     , board, this->m_moves, *this);
    shootPath(SOUTH_WEST, board, this->m_moves, *this);
    shootPath(WEST      , board, this->m_moves, *this);
    shootPath(NORTH_WEST, board, this->m_moves, *this);

}
void Piece::king_moves  (Piece* pieces, Piece* board[]){

    // Add the radius, ignoreing out of bounds and my color
    addPos(board, -1, -1);
    addPos(board,  0, -1);
    addPos(board,  1, -1);
    addPos(board, -1,  0);
    addPos(board,  1,  0);
    addPos(board, -1,  1);
    addPos(board,  0,  1);
    addPos(board,  1,  1);


    // Remove moves that the enemy can move to

    int offset = this->color() == WHITE ? 0 : PIECE_COUNT / 2;
    Piece* enemy = NULL;
    for (int i = offset; i < offset + PIECE_COUNT / 2; i++)
    {
        enemy = &pieces[i];
        if (enemy->captured()) continue;
        switch(enemy->type())
        {
        case PAWN:
            {
            int x_off = enemy->xPos() - 1;
            int y_off = enemy->yPos() + (enemy->color() == WHITE ? -1 : 1);

            if (IsValid(x_off) && IsValid(y_off))
                this->m_moves[x_off + y_off * WIDTH] = 0;
            x_off += 2;
            if (IsValid(x_off) && IsValid(y_off))
                this->m_moves[x_off + y_off * WIDTH] = 0;
            break;
            }
        case ROOK:
        case KNIGHT:
        case BISHOP:
        case QUEEN:
                this->m_moves &= (~enemy->moves_with_friends());
                break;
        case KING:
            this->m_moves &= (~enemy->moves_with_friends());
            break;
        default: std::cout << "ERROR in king moves\n";
        }
    }

}

void Piece::king_checked(Piece* pieces, Piece* board[], std::bitset<BOARD_SIZE>& check_mask)
{
    // quick and dirty way to figure out if the move captures the attacking piece
    // There can be, at most, two pieces attacking the king at a time

    unsigned int offset = this->color() == WHITE ? 0 : PIECE_COUNT / 2;
    int hold[2];
    hold[0] = -1;
    hold[1] = -1;
    Piece* enemy = NULL;
    for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
    {
        enemy = &pieces[i];
        if (!enemy->moves_with_friends()[this->arrPos()]) continue;

        if (this->m_moves[enemy->arrPos()])
            hold[ (hold[0] == -1 ? 0 : 1) ] = enemy->arrPos();


        // Lets also try to remove the trajectory here
        // Quick and dirty, as usual
        if (enemy->horizontal())
        {
            if (enemy->xPos() == this->xPos())
            {
                if (IsValid(this->yPos() - 1))
                {
                    this->m_moves[this->arrPos() - WIDTH] = 0;
                }
                if (IsValid(this->yPos() + 1))
                {
                    this->m_moves[this->arrPos() + WIDTH] = 0;
                }
            }
            else if (enemy->yPos() == this->yPos())
            {
                if (IsValid(this->xPos() - 1))
                {
                    this->m_moves[this->arrPos() - 1] = 0;
                }
                if (IsValid(this->xPos() + 1))
                {
                    this->m_moves[this->arrPos() + 1] = 0;
                }
            }
        }
        if (enemy->diagonal())
        {
            // We need to do seemingly redundant checks in the case enemy is a queen
            // For example, she may be a diagonal piece, but if she is vertically aligned,
            // Those digaonals should not effect the king
            if (enemy->xPos() < this->xPos()) // it is to the west
            {
                if (enemy->yPos() < this->yPos()) // to the north
                {
                    if (IsValid(this->xPos() + 1) && IsValid(this->yPos() + 1))
                        this->m_moves[this->arrPos() + 1 + WIDTH] = 0;
                }
                else if (enemy->yPos() > this->yPos())
                {
                    if (IsValid(this->xPos() + 1) && IsValid(this->yPos() - 1))
                        this->m_moves[this->arrPos() + 1 - WIDTH] = 0;
                }
            }
            else if (enemy->xPos() > this->xPos()) // it is to the east
            {
                if (enemy->yPos() < this->yPos()) // to the north
                {
                    if (IsValid(this->xPos() - 1) && IsValid(this->yPos() + 1))
                        this->m_moves[this->arrPos() - 1 + WIDTH] = 0;
                }
                else if (enemy->yPos() > this->yPos())
                {
                    if (IsValid(this->xPos() - 1) && IsValid(this->yPos() - 1))
                        this->m_moves[this->arrPos() - 1 - WIDTH] = 0;
                }
            }


        }
    }

    this->m_moves &= (~check_mask);
    if (hold[0] != -1) m_moves[hold[0]] = 1;
    if (hold[1] != -1) m_moves[hold[1]] = 1;
}

void Piece::shootPath(Board_Direction d, Piece* board[], std::bitset<BOARD_SIZE>& set, Piece& piece){
    int t_x = piece.xPos();
    int t_y = piece.yPos();

    int x_off = d & EAST  ? 1 : d & WEST  ? -1 : 0;
    int y_off = d & SOUTH ? 1 : d & NORTH ? -1 : 0;


    t_x += x_off;
    t_y += y_off;


    int t_arr = t_x + t_y * WIDTH;


    while (IsValid(t_x) && IsValid(t_y))
    {
        set[t_arr] = 1;
        if (board[t_arr])
            break;
        t_x += x_off;
        t_y += y_off;
        t_arr = t_x + (t_y * WIDTH);
    }
}
void Piece::addPos(Piece* board[], int x, int y){
    int t_x = x + this->xPos();
    int t_y = y + this->yPos();
    if (!IsValid(t_x) || !IsValid(t_y))
        return;

    int t_arrPos = t_x + t_y * WIDTH;

    if (!board[t_arrPos] || board[t_arrPos]->color() != this->color())
        m_moves[t_arrPos] = 1;
}
void Piece::printSet(std::bitset<BOARD_SIZE>& set, std::ostream& out){

    out << '\n';
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            out << (int)set[i * WIDTH + j] << " ";
        }
        out << '\n';
    }
    out << std::endl;
}



void Piece::setCheck(bool c)
{
    if (this->type() != KING) return;

    if   (c) this->me |=  MASK_CKD;
    else     this->me &= ~MASK_CKD;
}
std::bitset<BOARD_SIZE> Piece::makeCheckMask(Piece* pieces, Piece* board[])
{
    std::bitset<BOARD_SIZE> c_mask(0);

    int myKing = this->color() == WHITE ? WHITE_KING : BLACK_KING;
    int myKingArrPos = pieces[myKing].arrPos();
    int offset = this->color() == WHITE? 0 : PIECE_COUNT / 2;
    int count = 0;

    for (Piece* p = pieces + offset; p < pieces + offset + PIECE_COUNT / 2; p++)
    {
        if (!p->moves_with_friends()[myKingArrPos]) continue;

        // If two pieces are checking the king
        // then only the king can move
        if (++count > 1)
        {
            c_mask = 0;
            return c_mask;
        }

        if (p->type() == KING)
        {
            std::cout << "The king is checking the king somehow\n";
            c_mask = 0;
            return c_mask;
        }

        // Figure out which direction the attack is comming from
        Board_Direction toShoot = D_NONE;
        if (p->horizontal())  // Rook or Queen
        {
            if      (p->xPos() == pieces[myKing].xPos())
                toShoot = p->yPos() > pieces[myKing].yPos() ? NORTH : SOUTH;

            else if (p->yPos() == pieces[myKing].yPos())
                toShoot = p->xPos() > pieces[myKing].xPos() ? WEST  : EAST;
        }
        if (p->diagonal() && toShoot == D_NONE)  // Bishop or Queen
        {
            if (p->xPos() > pieces[myKing].xPos())
                toShoot = p->yPos() > pieces[myKing].yPos() ? NORTH_WEST : SOUTH_WEST;
            else
                toShoot = p->yPos() > pieces[myKing].yPos() ? NORTH_EAST : SOUTH_EAST;
        }

        shootPath(toShoot, board, c_mask, *p);
        c_mask[p->arrPos()] = 1;
    }
    return c_mask;
}

const std::bitset<BOARD_SIZE> Piece::moves_actual(const Piece* pieceList) const
{
    std::bitset<BOARD_SIZE> ret(this->m_moves);
    unsigned int offset = this->color() == WHITE ? PIECE_COUNT / 2 : 0;
    for (unsigned int i = offset; i < offset + PIECE_COUNT / 2; i++)
    {
        ret[pieceList[i].arrPos()]= 0;
    }
    return ret;
}

bool Piece::diagonal(void)
{
    return this->type() == BISHOP || this->type() == QUEEN;
}
bool Piece::horizontal(void)
{
    return this->type() == ROOK || this->type() == QUEEN;
}

void Piece::checkPinned(Piece* pieces, Piece* board[])
{
    if (this->type() != KING) return;
    pinPath(pieces, board, NORTH);
    pinPath(pieces, board, NORTH_EAST);
    pinPath(pieces, board, EAST);
    pinPath(pieces, board, SOUTH_EAST);
    pinPath(pieces, board, SOUTH);
    pinPath(pieces, board, SOUTH_WEST);
    pinPath(pieces, board, WEST);
    pinPath(pieces, board, NORTH_WEST);
}

void Piece::pinPath(Piece* pieces, Piece* board[], Board_Direction d)
{
    int x_off = ( d & EAST  ? 1 : ( d & WEST  ? -1 : 0 ) );
    int y_off = ( d & SOUTH ? 1 : ( d & NORTH ? -1 : 0 ) );

    int x_pos = x_off + this->xPos();
    int y_pos = y_off + this->yPos();

    std::bitset<BOARD_SIZE> p_mask(0);
    Piece* kFriend = NULL;
    while (IsValid(x_pos) && IsValid(y_pos))
    {
        int arrPos = x_pos + y_pos * WIDTH;

        if (!board[arrPos])
        {
            p_mask[arrPos] = 1;
        }
        else if (board[arrPos]->color() == this->color())
        {
            if (kFriend) return;
            else kFriend = board[arrPos];
        }
        else
        {
            if (x_off != 0 && y_off != 0 && board[arrPos]->diagonal()) // a diagonal direction d
            {
                if (kFriend)
                {
                    std::cout << "diagonal pin\n";
                    printSet(p_mask);
                    kFriend->andMoves(p_mask);
                }
                return;
            }
            if ((x_off != 0 || y_off != 0) && board[arrPos]->horizontal()) // horizontal
            {
                if (kFriend)
                {
                    std::cout << "horizontal pin\n";
                    printSet(p_mask);
                    kFriend->andMoves(p_mask);
                }
                return;
            }

            //  We get here if we found a black piece that could not move
            //  In the direction we were traveling
            return;
        }
        x_pos += x_off;
        y_pos += y_off;
    }
}

    void Piece::andMoves(std::bitset<BOARD_SIZE> mask)
    {
        this->m_moves &= mask;
    }
//