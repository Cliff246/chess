#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "bitmanip.h"

#define GRID_2_INDEX(x,y) ((8 * y) + x)
#define COMBINE(a,b) (char)((b << 4) | a)
#define WHITESIDE false
#define BLACKSIDE true
//PIECE BIT ORDER
//1st-3th bits = id
//4th bit = side
/*
     a  b  c  d  e  f  g  h
   1 r  h  b  q  k  b  h  r
   2 p  p  p  p  p  p  p  p
   3 
   4
   5
   6
   7 p  p  p  p  p  p  p  p
   8 r  h  b  q  k  b  h  r
*/
#define W_PAWN      0x01
#define W_HORSE     0x02
#define W_BISHOP    0x03
#define W_ROOK      0x04
#define W_QUEEN     0x05
#define W_KING      0x06

#define B_PAWN      0x09
#define B_HORSE     0x0A
#define B_BISHOP    0x0B
#define B_ROOK      0x0C
#define B_QUEEN     0x0D
#define B_KING      0x0E


#define BOARD_BIT_SIZE 32

#define PRINTF_LINE(fmt,...) {printf("%d : ",__LINE__);printf(fmt, __VA_ARGS__);}

#define NO_CHECK 0
#define WHITE_CHECK 1
#define BLACK_CHECK 2

#define NO_PROMOTE 0
#define PROMOTE_QUEEN 1
#define PROMOTE_KNIGHT 2
#define PROMOTE_ROOK 3
#define PROMOTE_BISHOP 4

typedef struct
{
    char ary[BOARD_BIT_SIZE];
    unsigned long long move;
    bool turn;
    bool captured;
    bool castle_ks_white ,castle_qs_white, castle_ks_black,castle_qs_black;
    char promoted;    
    char checkmate : 2;
    char check : 2;
}board_t;

typedef struct 
{
    char type;
    char index;
    bool side;
}piece_t;

typedef struct{int x; int y;} xy_t; 
xy_t get_piecexy(char index);
char *get_movestring(board_t *board, bool side, char start, char end);
bool isodd(int num);
char *piece_to_char(char piece);
void printbin(long bin, char len);
void get_piece(board_t *board, char index, piece_t *ref);
char piece_ifsize(char piece, bool side);
void set_piece(board_t *board, char index, piece_t toset);
void reset_board(board_t *tboard);
void print_board(board_t *board);

#define MAX_PIECE_STR_LEN 3


typedef struct { char pnum; char pch[MAX_PIECE_STR_LEN];} pieceinfo_t;
pieceinfo_t piece_numbers[7][2] = {
    {0x0, " "}, {0x1, "P "}, {0x2, "Kn"}, {0x3, "B "}, {0x4, "R "}, {0x5, "Q "}, {0x6, "K "}
};

xy_t get_piecexy(char index)
{
    static xy_t current_xy;
    current_xy.x = index % 8;
    current_xy.y = index / 8;
    return current_xy;
}

bool isodd(int num)
{
    return (num % 2 == 1)? true : false;
}

//creates new string
char *get_movestring(board_t *board, bool side, char start, char end)
{
    piece_t tomove;
    piece_t dest;  
    get_piece(board, start, &tomove);
    get_piece(board, end, &dest);

      
    bool iscastle = false;

    char sidechr = (tomove.side == WHITESIDE) ? '+':'-' ;
    
    char ptype = 0;
    char px = 0xff, py = 0xff;
    {
        xy_t temp = get_piecexy(tomove.index);
        px = temp.x;
        py = temp.y;
    }
    
    
    //side piecelength = how long it is to spell the 
    //"MOVE"

    unsigned digits_in_turn = (unsigned)(floor(log10((double)board->move))) + 1;
    unsigned num_of_periods = (board->turn == WHITESIDE) ? 1:3;
    unsigned extentions_len = 0; 
    unsigned add_file = 0;
    unsigned add_rank = 0;
    if(board->check > NO_CHECK || board->checkmate > NO_CHECK || board->captured == true)
        extentions_len = 1;
    
    unsigned strsize =  (digits_in_turn) /*i hope this does not reach the int limit*/ + (num_of_periods) /*. or ...*/ + (1) /* Space*/ + (2)/*cordnates1 take up at most 2 char*/+ extentions_len/*if a piece captured */ + (1) /*space*/ +  + (1)/*side takes up 1 char*/ +  MAX_PIECE_STR_LEN; 



    char *temp = (char *)calloc( strsize + 1,sizeof(char));
    
    switch(side)
    {
        default:
            break;
    };
    return temp;
}

void printbin(long bin, char len)
{
#define BUFSIZE 65
    char buffer[BUFSIZE] = {0};
    for (int i = 0, b = len - 1; i < len; i++, b--)
    {
        char bit = GETBIT(bin, b);
        buffer[i] = (char)(bit + '0');
    }
    printf("piece = %s\n", buffer);
}

char *piece_to_char(char piece)
{
    
   char* c = "A";
   for(int i = 0; i<7; i++)
    {
        PRINTF_LINE("break %d \n", piece);
        
        if(piece == piece_numbers[i][0].pnum)
        {  
            
            c = (piece_numbers[i][0].pch);
            for(int i = 0; i < MAX_PIECE_STR_LEN - 1; i++)
            {
                printf("%x:%x ", piece_numbers[i][0].pch[i], c[i]);   
                   
            }

            PRINTF_LINE("\n%d piece:\"%s\"  \n",piece, c);
            break;
        }
    }
    return c;
}

void get_piece(board_t *board, char index, piece_t *ref)
{
    char *ary = (board->ary);
    int arypos =  index / 2;
    unsigned char data = ary[arypos];
    data = (isodd(index))? (data & 0xf0) >> 4 : (data & 0x0f);
    unsigned char piece = data & 0x7, side = data & 0x8;
    ref->index = index;     
    ref->side = side;
    ref->type = piece;
    PRINTF_LINE("%d %d %d \n", index, side, piece);
    PRINTF_LINE("piece = %s\n", piece_to_char(piece));
}

char piece_ifsize(char piece, bool side)
{
    if(side == WHITESIDE)
    {
        switch(piece)
        {
            
            case(0x01):
                return W_PAWN;
            case(0x02):
                return W_HORSE;
            case(0x03):
                return W_BISHOP;
            case(0x04):
                return W_ROOK;
            case(0x05):
                return W_QUEEN;
            case(0x06):
                return W_KING;
        }
    }
    else
    {
        switch(piece)
        {
            case(0x01):
                return B_PAWN;
            case(0x02):
                return B_HORSE;
            case(0x03):
                return B_BISHOP;
            case(0x04):
                return B_ROOK;
            case(0x05):
                return B_QUEEN;
            case(0x06):
                return B_KING;
        }
    }
    return -1;
}

void set_piece(board_t *board, char index,  piece_t toset)
{
    char *ary = (board->ary);
    piece_t temp;
    if(isodd(index))
    {
        get_piece(board, index - 1, &temp);
        ary[index/2] = COMBINE(temp.type,toset.type );
    }
    else
    {
        get_piece(board, index + 1, &temp);
        ary[index/2] = COMBINE(toset.type, temp.type);
    }
    toset.index = index;
}
void movepiece(board_t *board, bool side, char start, char end)
{
	

	piece_t source = {.index = 0, .side = false, .type = 0 };
    piece_t dest = {.index = 0, .side = false, .type = 0 };
    get_piece(board, start, &source);
    get_piece(board, end, &dest);
    if(side == WHITESIDE)
    {
        source.side = WHITESIDE;
        dest.side = BLACKSIDE;
    } 

    
}


void print_board(board_t *board)
{
    
    char bufferboard[8][9][MAX_PIECE_STR_LEN + 1];
    
    memset(bufferboard, 0x00, sizeof(bufferboard)/ sizeof(bufferboard[0]));

    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            piece_t temp;
            get_piece(board, GRID_2_INDEX(x,y), &temp);
            PRINTF_LINE("%d\n", temp.type);
            strcpy_s(bufferboard[x][y], MAX_PIECE_STR_LEN, piece_to_char(temp.type));
            
        }
        bufferboard[x][8][0] = ' '; 
    }
    for(int i = 0; i < 8; i++)
    {
        for(int h = 0; h < 8; h++)
        {
           printf("%s ", bufferboard[h][i]);
        }
        printf("\n");
    }
}

void reset_board(board_t *tboard)
{
    if(tboard != NULL)
    {
        tboard->check = NO_CHECK;
        tboard->checkmate = NO_CHECK;
        tboard->captured = false;
        tboard->turn = WHITESIDE;
        tboard->move = 0;
        tboard->castle_ks_black = true;
        tboard->castle_ks_white = true;
        tboard->castle_qs_black = true;
        tboard->castle_qs_white = true;
        tboard->promoted = NO_PROMOTE;
        
        char *board = (tboard->ary);
        memset(board, 0x00, 32);
        (board)[0] = COMBINE(W_ROOK, W_HORSE);
        (board)[1] = COMBINE(W_BISHOP, W_QUEEN);
        (board)[2] = COMBINE(W_KING, W_BISHOP);
        (board)[3] = COMBINE(W_HORSE, W_ROOK);
        (board)[4] = COMBINE(W_PAWN, W_PAWN);
        (board)[5] = COMBINE(W_PAWN, W_PAWN);
        (board)[6] = COMBINE(W_PAWN, W_PAWN);
        (board)[7] = COMBINE(W_PAWN, W_PAWN);
        (board)[24] = COMBINE(B_PAWN, B_PAWN);
        (board)[25] = COMBINE(B_PAWN, B_PAWN);
        (board)[26] = COMBINE(B_PAWN, B_PAWN);
        (board)[27] = COMBINE(B_PAWN, B_PAWN);
        (board)[28] = COMBINE(B_ROOK,B_HORSE);
        (board)[29] = COMBINE(B_BISHOP,B_QUEEN);
        (board)[30] = COMBINE(B_KING,B_BISHOP);
        (board)[31] = COMBINE(B_HORSE,B_ROOK);
    }
    else
    {
        perror("Board is null");
    }   
}
int main(int argc, char* argv[])
{
    printf("board");
    board_t tboard;
    reset_board(&tboard);
    print_board(&tboard);
    piece_t piece = {.side=BLACKSIDE, .type = B_KING};
    set_piece(&tboard, GRID_2_INDEX(7,7), piece);
    print_board(&tboard);
    //piece_t piece;
    //get_piece(tboard, GRID_2_INDEX(0,0), &piece);
    //PRINTF_LINE("%d %d \n",piece.type,piece.side);
    return 0;
}
