#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define WIDTH 800
#define HEIGHT 450

typedef enum {
    NONE,
    SNAKE,
    PING_PONG,
    TETRIS
} GameSelection;
int SnakeGame(void);
int Tetris(void);
int PingPong(void);
int levelSelector(GameSelection game);

int main() {
    // Initialization
    InitWindow(WIDTH, HEIGHT, "Game Selection Window");
    SetTargetFPS(60);

    GameSelection selection = NONE;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePoint = GetMousePosition();
            if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 100, 200, 50 })) {
                selection = SNAKE;
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 200, 200, 50 })) {
                selection = PING_PONG;
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 300, 200, 50 })) {
                selection = TETRIS;
                break;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        //get the image loded to vram in form of texture
        Texture2D snake=LoadTexture("Snake.png");
        Texture2D ping=LoadTexture("ping1.png");
        Texture2D tetris=LoadTexture("tetris1.png");
        // Draw buttons
        DrawRectangle(50, 100, 200, 50, BLUE);
        DrawText("Snake", 80, 115, 20, WHITE);
        DrawRectangle(50, 200, 200, 50, GREEN);
        DrawText("Ping Pong", 80, 215, 20, WHITE);
        DrawRectangle(50, 300, 200, 50, RED);
        DrawText("Tetris", 85, 315, 20, WHITE);

        Vector2 mousepoint=GetMousePosition();
        if (CheckCollisionPointRec(mousepoint, (Rectangle){ 50, 100, 200, 50 })) {
                DrawTexture(snake,350,90,LIGHTGRAY);
        } 
        else if (CheckCollisionPointRec(mousepoint, (Rectangle){ 50, 200, 200, 50 })) {
                DrawTexture(ping,300,70,LIGHTGRAY);
        }
        else if (CheckCollisionPointRec(mousepoint, (Rectangle){ 50, 300, 200, 50 })) {
                DrawTexture(tetris,350,70,LIGHTGRAY);
        }

        EndDrawing();
    }

    // Close window
    CloseWindow();

    // Launch selected game
    switch (selection) {
        case SNAKE:
            SnakeGame();
            // Call function to start Snake game
            break;
        case PING_PONG:
            PingPong();
            // Call function to start Ping Pong game
            break;
        case TETRIS:
            Tetris();
            // Call function to start Tetris game
            break;
        default:
            break;
    }

    return 0;
}

int levelSelector(GameSelection game){
    int level=15;
    InitWindow(200,200,"Select game level");
    SetTargetFPS(30);
    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && game == SNAKE) {
            Vector2 mousePoint = GetMousePosition();
            if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 0, 200, 40 })) {
                level = 15;
                //set fps to 15 for that game
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 80, 200, 40 })) {
                level = 30;
                //set fps to 30 for that game
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 160, 200, 40 })) {
                level = 45;
                //set fps to 45 for that game
                break;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && game == PING_PONG) {
            Vector2 mousePoint = GetMousePosition();
            if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 0, 200, 40 })) {
                level = 30;
                //set fps to 15 for that game
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 80, 200, 40 })) {
                level = 45;
                //set fps to 30 for that game
                break;
            } else if (CheckCollisionPointRec(mousePoint, (Rectangle){ 50, 160, 200, 40 })) {
                level = 60;
                //set fps to 45 for that game
                break;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw buttons
        DrawRectangle(50, 0, 200, 40, DARKGRAY);
        DrawText("Easy", 80, 15, 20, WHITE);
        DrawRectangle(50, 80, 200, 40, DARKGRAY);
        DrawText("Medium", 80, 95, 20, WHITE);
        DrawRectangle(50, 160, 200, 40, DARKGRAY);
        DrawText("Hard", 85, 175, 20, WHITE);

        EndDrawing();
    }

    // Close window
    CloseWindow();
    return level;
}
//----------------------------------------------------------------------------------
//Tetris
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SQUARE_SIZE             20

#define GRID_HORIZONTAL_SIZE    12
#define GRID_VERTICAL_SIZE      20

#define LATERAL_SPEED           10
#define TURNING_SPEED           12
#define FAST_FALL_AWAIT_COUNTER 30

#define FADING_TIME             33

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GridSquare{EMPTY,MOVING1,MOVING2,MOVING3,MOVING4,MOVING5,MOVING6,MOVING7,FULL,BLOCK,FADING} GridSquare;

//----------------------------------------------------------------------------------
// Matrices
//----------------------------------------------------------------------------------
static GridSquare grid[GRID_HORIZONTAL_SIZE][GRID_VERTICAL_SIZE];
static GridSquare piece[4][4];
static GridSquare incomingPiece[4][4];

// These variables keep track of the active piece position
static int piecePositionX = 0;
static int piecePositionY = 0;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth=800;
static const int screenHeight=450;

static bool GameOver=false;
static bool Pause=false;

//Game Parameters
static Color backgroundColor=(Color){255,253,208,255};//(Color){139,69,19,255}
static Color TextColor=BLACK; //(Color){255,253,208,255}Cream Color with full Alpha
static Color fadingColor;


static bool beginPlay=true;  // This var is only true at the begining of the game, used for the first matrix creations
static bool lineToDelete=false;
static bool pieceActive=false;
static bool detection=false;

//Statistics
static int level=1;
static int lines=0;

// Counters
static int gravityMovementCounter = 0;
static int lateralMovementCounter = 0;
static int turnMovementCounter = 0;
static int fastFallMovementCounter = 0;

static int fadeLineCounter = 0;

// Based on level
static int gravitySpeed = 30;



//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void UpdateDrawFrame(void); //Update and Draw(One Frame)
static void DrawGame(void);        // Draw game (one frame)
static void InitGame(void);        //Initialize the Game
static void UpdateGame(void);      // Update Game(One Frame)

// Additional module functions
static bool Createpiece();
static void GetRandompiece();
static void ResolveFallingMovement(bool *detection, bool *pieceActive);
static bool ResolveLateralMovement();
static bool ResolveTurnMovement();
static void CheckDetection(bool *detection);
static void CheckCompletion(bool *lineToDelete);
static int DeleteCompleteLines();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int Tetris(void){
    //Initialization
     //---------------------------------------------------------
     InitWindow(screenWidth,screenHeight, "classicGame: Tetris");
     
     InitGame();
     
     SetTargetFPS(60);
     //--------------------------------------------------------------------------------------

    // Main game loop
    while(!WindowShouldClose()){ //Detect Window close button or Esc key
        //Update and Draw
        //--------------------------------------------------------------------------------------
         UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }        
    
    CloseWindow(); //Closes window
    //--------------------------------------------------------------------------------------
    
    return 0;
  
}

void InitGame(void){
    
   // Initialize game statistics
    level = 1;
    lines = 0;

    fadingColor = GRAY;

    piecePositionX = 0;
    piecePositionY = 0;

    Pause = false;

    beginPlay = true;
    pieceActive = false;
    detection = false;
    lineToDelete = false;

    // Counters
    gravityMovementCounter = 0;
    lateralMovementCounter = 0;
    turnMovementCounter = 0;
    fastFallMovementCounter = 0;

    fadeLineCounter = 0;
    gravitySpeed = 30;

    // Initialize grid matrices
    for (int i = 0; i < GRID_HORIZONTAL_SIZE; i++)
    {
        for (int j = 0; j < GRID_VERTICAL_SIZE; j++)
        {
            if ((j == GRID_VERTICAL_SIZE - 1) || (i == 0) || (i == GRID_HORIZONTAL_SIZE - 1)) grid[i][j] = BLOCK;
            else grid[i][j] = EMPTY;
        }
    }

    // Initialize incoming piece matrices
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j< 4; j++)
        {
            incomingPiece[i][j] = EMPTY;
        }
    }
    
}

//Draw Game(one Frame)
void DrawGame(void){
    BeginDrawing();
    
    ClearBackground(backgroundColor);
    if(!GameOver){
    //Draw GamePlay Area
    Vector2 offset;
    offset.x = screenWidth/2 - (GRID_HORIZONTAL_SIZE*SQUARE_SIZE/2) - 50;
    offset.y = screenHeight/2 - ((GRID_VERTICAL_SIZE - 1)*SQUARE_SIZE/2) + SQUARE_SIZE*2;

    offset.y -= 50;     // NOTE: Harcoded position!

    int controller = offset.x;

    for (int j = 0; j < GRID_VERTICAL_SIZE; j++)
    {
        for (int i = 0; i < GRID_HORIZONTAL_SIZE; i++)
        {
            // Draw each square of the grid
            if (grid[i][j] == EMPTY)
            {
                DrawLine(offset.x, offset.y, offset.x + SQUARE_SIZE, offset.y, BLACK );
                DrawLine(offset.x, offset.y, offset.x, offset.y + SQUARE_SIZE, BLACK );
                DrawLine(offset.x + SQUARE_SIZE, offset.y, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, BLACK );
                DrawLine(offset.x, offset.y + SQUARE_SIZE, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, BLACK );
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == FULL)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, GRAY);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING1)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, RED);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING2)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, BLUE);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING3)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, DARKGREEN);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING4)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ORANGE);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING5)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, DARKPURPLE);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING6)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, PINK);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == MOVING7)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, GOLD);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == BLOCK)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, BLACK);
                offset.x += SQUARE_SIZE;
            }
            else if (grid[i][j] == FADING)
            {
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, fadingColor);
                offset.x += SQUARE_SIZE;
            }
        }

        offset.x = controller;
        offset.y += SQUARE_SIZE;
    }

    //Draw Incoming Piece (hardcoded)
    offset.x=500;
    offset.y=45;
    
    int controler=offset.x;
    
    for(int j=0;j<4;j++){
        
        for(int i=0;i<4;i++){
            
                if (incomingPiece[i][j] == EMPTY)
                    {
                        DrawLine(offset.x, offset.y, offset.x + SQUARE_SIZE, offset.y, BLACK );
                        DrawLine(offset.x, offset.y, offset.x, offset.y + SQUARE_SIZE, BLACK );
                        DrawLine(offset.x + SQUARE_SIZE, offset.y, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, BLACK );
                        DrawLine(offset.x, offset.y + SQUARE_SIZE, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, BLACK );
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING1)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, RED);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING2)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, BLUE);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING3)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, DARKGREEN);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING4)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ORANGE);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING5)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, DARKPURPLE);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING6)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, PINK);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING7)
                    {
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, GOLD);
                        offset.x += SQUARE_SIZE;
                    }
                
        }
        
         offset.x = controler;
         offset.y += SQUARE_SIZE;
    
    }
    DrawText("INCOMING:", offset.x, offset.y - 100, 10, TextColor);
    DrawText(TextFormat("LINES:      %04i",lines), offset.x, offset.y + 20, 10, TextColor);
    
    if (Pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, TextColor);
    }
    else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, TextColor);
    EndDrawing();
}

void UpdateGame(void){
    if(!GameOver){
        if(IsKeyPressed('P')) Pause=!Pause;
        if(!Pause){
            if(!lineToDelete){
            if (!pieceActive)
                {
                    // Get another piece
                    pieceActive = Createpiece();

                    // We leave a little time before starting the fast falling down
                    fastFallMovementCounter = 0;
                }
            else   // piece falling
            {
                 // Counters update
                fastFallMovementCounter++;
                gravityMovementCounter++;
                lateralMovementCounter++;
                turnMovementCounter++;

                // We make sure to move if we've pressed the key this frame
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) lateralMovementCounter = LATERAL_SPEED;
                if (IsKeyPressed(KEY_UP)) turnMovementCounter = TURNING_SPEED;
                
                // Fall down
                if (IsKeyDown(KEY_DOWN) && (fastFallMovementCounter >= FAST_FALL_AWAIT_COUNTER))
                {
                    // We make sure the piece is going to fall this frame
                    gravityMovementCounter += gravitySpeed;
                }
                
                if (gravityMovementCounter >= gravitySpeed)
                    {
                        // Basic falling movement
                        CheckDetection(&detection);

                        // Check if the piece has collided with another piece or with the boundings
                        ResolveFallingMovement(&detection, &pieceActive);

                        // Check if we fullfilled a line and if so, erase the line and pull down the the lines above
                        CheckCompletion(&lineToDelete);

                        gravityMovementCounter = 0;
                    }
                // Move laterally at player's will
                if (lateralMovementCounter >= LATERAL_SPEED)
                {
                    // Update the lateral movement and if success, reset the lateral counter
                    if (!ResolveLateralMovement()) lateralMovementCounter = 0;
                }
                
                // Turn the piece at player's will
                if (turnMovementCounter >= TURNING_SPEED)
                {
                    // Update the turning movement and reset the turning counter
                    if (ResolveTurnMovement()) turnMovementCounter = 0;
                }
            }
            
            // Game over logic
                for (int j = 0; j < 2; j++)
                {
                    for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                    {
                        if (grid[i][j] == FULL)
                        {
                            GameOver = true;
                        }
                    }
                }
            }
           else
            {
                // Animation when deleting lines
                fadeLineCounter++;

                if (fadeLineCounter%8 < 4) fadingColor = MAROON;
                else fadingColor = GRAY;

                if (fadeLineCounter >= FADING_TIME)
                {
                    int deletedLines = 0;
                    deletedLines = DeleteCompleteLines();
                    fadeLineCounter = 0;
                    lineToDelete = false;

                    lines += deletedLines;
                }
            }
        }
    }
    else{
        if(IsKeyPressed(KEY_ENTER)){
            InitGame();
            GameOver=false;
        }
    }
}
//Update and Draw (One Frame)
void UpdateDrawFrame(void){
    UpdateGame();
    DrawGame();
}

//--------------------------------------------------------------------------------------
// Additional module functions
//--------------------------------------------------------------------------------------
static bool Createpiece()
{
    piecePositionX = (int)((GRID_HORIZONTAL_SIZE - 4)/2);
    piecePositionY = 0;

    // If the game is starting and you are going to create the first piece, we create an extra one
    if (beginPlay)
    {
        GetRandompiece();
        beginPlay = false;
    }

    // We assign the incoming piece to the actual piece
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j< 4; j++)
        {
            piece[i][j] = incomingPiece[i][j];
        }
    }

    // We assign a random piece to the incoming one
    GetRandompiece();

    // Assign the piece to the grid
    for (int i = piecePositionX; i < piecePositionX + 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (piece[i - (int)piecePositionX][j] == MOVING1) grid[i][j] = MOVING1;
            if (piece[i - (int)piecePositionX][j] == MOVING2) grid[i][j] = MOVING2;
            if (piece[i - (int)piecePositionX][j] == MOVING3) grid[i][j] = MOVING3;
            if (piece[i - (int)piecePositionX][j] == MOVING4) grid[i][j] = MOVING4;
            if (piece[i - (int)piecePositionX][j] == MOVING5) grid[i][j] = MOVING5;
            if (piece[i - (int)piecePositionX][j] == MOVING6) grid[i][j] = MOVING6;
            if (piece[i - (int)piecePositionX][j] == MOVING7) grid[i][j] = MOVING7;
        }
    }

    return true;
}
static void GetRandompiece()
{
    int random = GetRandomValue(0, 6);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            incomingPiece[i][j] = EMPTY;
        }
    }

    switch (random)
    {
        case 0: { incomingPiece[1][1] = MOVING7; incomingPiece[2][1] = MOVING7; incomingPiece[1][2] = MOVING7; incomingPiece[2][2] = MOVING7; } break;    //Cube
        case 1: { incomingPiece[1][0] = MOVING2; incomingPiece[1][1] = MOVING2; incomingPiece[1][2] = MOVING2; incomingPiece[2][2] = MOVING2; } break;    //L
        case 2: { incomingPiece[1][2] = MOVING3; incomingPiece[2][0] = MOVING3; incomingPiece[2][1] = MOVING3; incomingPiece[2][2] = MOVING3; } break;    //L inversa
        case 3: { incomingPiece[0][1] = MOVING4; incomingPiece[1][1] = MOVING4; incomingPiece[2][1] = MOVING4; incomingPiece[3][1] = MOVING4; } break;    //Recta
        case 4: { incomingPiece[1][0] = MOVING5; incomingPiece[1][1] = MOVING5; incomingPiece[1][2] = MOVING5; incomingPiece[2][1] = MOVING5; } break;    //T
        case 5: { incomingPiece[1][1] = MOVING6; incomingPiece[2][1] = MOVING6; incomingPiece[2][2] = MOVING6; incomingPiece[3][2] = MOVING6; } break;    //S
        case 6: { incomingPiece[1][2] = MOVING1; incomingPiece[2][2] = MOVING1; incomingPiece[2][1] = MOVING1; incomingPiece[3][1] = MOVING1; } break;    //S inversa
    }
}

static void CheckDetection(bool *detection)
{
    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
    {
        for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
        {
            if ((grid[i][j] == MOVING1 || grid[i][j]==MOVING2 || grid[i][j]==MOVING3 || grid[i][j] == MOVING4 || grid[i][j]==MOVING5 || grid[i][j]==MOVING6 || grid[i][j]==MOVING7) && ((grid[i][j+1] == FULL) || (grid[i][j+1] == BLOCK))) *detection = true;
        }
    }
}

static void CheckCompletion(bool *lineToDelete)
{
    int calculator = 0;

    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
    {
        calculator = 0;
        for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
        {
            // Count each square of the line
            if (grid[i][j] == FULL)
            {
                calculator++;
            }

            // Check if we completed the whole line
            if (calculator == GRID_HORIZONTAL_SIZE - 2)
            {
                *lineToDelete = true;
                calculator = 0;
                // points++;

                // Mark the completed line
                for (int z = 1; z < GRID_HORIZONTAL_SIZE - 1; z++)
                {
                    grid[z][j] = FADING;
                }
            }
        }
    }
}

static void ResolveFallingMovement(bool *detection, bool *pieceActive)
{
    // If we finished moving this piece, we stop it
    if (*detection)
    {
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if (grid[i][j] == MOVING1 || grid[i][j]==MOVING2 || grid[i][j]==MOVING3 || grid[i][j] == MOVING4 || grid[i][j]==MOVING5 || grid[i][j]==MOVING6 || grid[i][j]==MOVING7)
                {
                    grid[i][j] = FULL;
                    *detection = false;
                    *pieceActive = false;
                }
            }
        }
    }
    else    // We move down the piece
    {
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if (grid[i][j] == MOVING1)
                {
                    grid[i][j+1] = MOVING1;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING2)
                {
                    grid[i][j+1] = MOVING2;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING3)
                {
                    grid[i][j+1] = MOVING3;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING4)
                {
                    grid[i][j+1] = MOVING4;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING5)
                {
                    grid[i][j+1] = MOVING5;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING6)
                {
                    grid[i][j+1] = MOVING6;
                    grid[i][j] = EMPTY;
                }
                if (grid[i][j] == MOVING7)
                {
                    grid[i][j+1] = MOVING7;
                    grid[i][j] = EMPTY;
                }
            }
        }

        piecePositionY++;
    }
}

static bool ResolveLateralMovement()
{  
    bool collision = false;

    // Piece movement
    if (IsKeyDown(KEY_LEFT))        // Move left
    {
        // Check if is possible to move to left
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if (grid[i][j] == MOVING1 || grid[i][j] == MOVING2 || grid[i][j]== MOVING3 || grid[i][j] == MOVING4 || grid[i][j] == MOVING5 || grid[i][j]== MOVING6 || grid[i][j]==MOVING7)
                {
                    // Check if we are touching the left wall or we have a full square at the left
                    if ((i-1 == 0) || (grid[i-1][j] == FULL)) collision = true;
                }
            }
        }

        // If able, move left
        if (!collision)
        {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)             // We check the matrix from left to right
                {
                    // Move everything to the left
                    if (grid[i][j] == MOVING1)
                    {
                        grid[i-1][j] = MOVING1;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING2)
                    {
                        grid[i-1][j] = MOVING2;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING3)
                    {
                        grid[i-1][j] = MOVING3;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING4)
                    {
                        grid[i-1][j] = MOVING4;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING5)
                    {
                        grid[i-1][j] = MOVING5;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING6)
                    {
                        grid[i-1][j] = MOVING6;
                        grid[i][j] = EMPTY;
                    }
                    if (grid[i][j] == MOVING7)
                    {
                        grid[i-1][j] = MOVING7;
                        grid[i][j] = EMPTY;
                    }
                }
            }

            piecePositionX--;
        }
    }
    else if (IsKeyDown(KEY_RIGHT))  // Move right
    {
        // Check if is possible to move to right
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if (grid[i][j] == MOVING1 || grid[i][j]==MOVING2 || grid[i][j]==MOVING3 || grid[i][j] == MOVING4 || grid[i][j]==MOVING5 || grid[i][j]==MOVING6 || grid[i][j]==MOVING7)
                {
                    // Check if we are touching the right wall or we have a full square at the right
                    if ((i+1 == GRID_HORIZONTAL_SIZE - 1) || (grid[i+1][j] == FULL))
                    {
                        collision = true;

                    }
                }
            }
        }

        // If able move right
        if (!collision)
        {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = GRID_HORIZONTAL_SIZE - 1; i >= 1; i--)             // We check the matrix from right to left
                {
                    // Move everything to the right
                    if (grid[i][j] == MOVING1)
                    {
                        grid[i+1][j] = MOVING1;
                        grid[i][j] = EMPTY;
                    }
                    if(grid[i][j]==MOVING2)
                    {
                        grid[i+1][j]=MOVING2;
                        grid[i][j]=EMPTY;
                    }
                    if(grid[i][j]==MOVING3)
                    {
                        grid[i+1][j]=MOVING3;
                        grid[i][j]=EMPTY;
                    }
                    if (grid[i][j] == MOVING4)
                    {
                        grid[i+1][j] = MOVING4;
                        grid[i][j] = EMPTY;
                    }
                    if(grid[i][j]==MOVING5)
                    {
                        grid[i+1][j]=MOVING5;
                        grid[i][j]=EMPTY;
                    }
                    if(grid[i][j]==MOVING6)
                    {
                        grid[i+1][j]=MOVING6;
                        grid[i][j]=EMPTY;
                    }
                    if(grid[i][j]==MOVING7)
                    {
                        grid[i+1][j]=MOVING7;
                        grid[i][j]=EMPTY;
                    }
                }
            }

            piecePositionX++;
        }
    }

    return collision;
}

static int DeleteCompleteLines()
{
    int deletedLines = 0;

    // Erase the completed line
    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
    {
        while (grid[1][j] == FADING)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                grid[i][j] = EMPTY;
            }

            for (int j2 = j-1; j2 >= 0; j2--)
            {
                for (int i2 = 1; i2 < GRID_HORIZONTAL_SIZE - 1; i2++)
                {
                    if (grid[i2][j2] == FULL)
                    {
                        grid[i2][j2+1] = FULL;
                        grid[i2][j2] = EMPTY;
                    }
                    else if (grid[i2][j2] == FADING)
                    {
                        grid[i2][j2+1] = FADING;
                        grid[i2][j2] = EMPTY;
                    }
                }
            }

             deletedLines++;
        }
    }

    return deletedLines;
}

static bool ResolveTurnMovement()
{
    // Input for turning the piece
    if (IsKeyDown(KEY_UP))
    {
        GridSquare aux;
        bool checker = false;

         // Check all turning possibilities
        if ((grid[piecePositionX + 3][piecePositionY] == MOVING1) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] == MOVING1) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] != MOVING1)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] == MOVING1) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != MOVING1)) checker = true;

        if ((grid[piecePositionX][piecePositionY] == MOVING1) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] == MOVING1) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] == MOVING1) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] == MOVING1) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != MOVING1)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] == MOVING1) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] == MOVING1) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] == MOVING1) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] == MOVING1) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != MOVING1)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] == MOVING1) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] == MOVING1) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] == MOVING1) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] == MOVING1) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] == MOVING1) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != MOVING1)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY] == MOVING2) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] == MOVING2) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] != MOVING2)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] == MOVING2) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != MOVING2)) checker = true;

        if ((grid[piecePositionX][piecePositionY] == MOVING2) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] == MOVING2) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] == MOVING2) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] == MOVING2) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != MOVING2)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] == MOVING2) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] == MOVING2) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] == MOVING2) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] == MOVING2) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != MOVING2)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] == MOVING2) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] == MOVING2) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] == MOVING2) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] == MOVING2) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] == MOVING2) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != MOVING2)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY] ==MOVING3) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] ==MOVING3) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] !=MOVING3)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] ==MOVING3) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] !=MOVING3)) checker = true;

        if ((grid[piecePositionX][piecePositionY] ==MOVING3) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] ==MOVING3) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] ==MOVING3) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] ==MOVING3) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] !=MOVING3)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] ==MOVING3) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] ==MOVING3) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] ==MOVING3) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] ==MOVING3) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] !=MOVING3)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] ==MOVING3) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] ==MOVING3) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] ==MOVING3) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] ==MOVING3) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] ==MOVING3) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] !=MOVING3)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY] == MOVING4) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] == MOVING4) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] != MOVING4)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] == MOVING4) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != MOVING4)) checker = true;

        if ((grid[piecePositionX][piecePositionY] == MOVING4) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] == MOVING4) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] == MOVING4) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] == MOVING4) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != MOVING4)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] == MOVING4) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] == MOVING4) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] == MOVING4) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] == MOVING4) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != MOVING4)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] == MOVING4) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] == MOVING4) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] == MOVING4) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] == MOVING4) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] == MOVING4) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != MOVING4)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY] ==MOVING5) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] ==MOVING5) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] !=MOVING5)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] ==MOVING5) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] !=MOVING5)) checker = true;

        if ((grid[piecePositionX][piecePositionY] ==MOVING5) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] ==MOVING5) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] ==MOVING5) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] ==MOVING5) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] !=MOVING5)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] ==MOVING5) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] ==MOVING5) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] ==MOVING5) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] ==MOVING5) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] !=MOVING5)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] ==MOVING5) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] ==MOVING5) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] ==MOVING5) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] ==MOVING5) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] ==MOVING5) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] !=MOVING5)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY] ==MOVING6) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] ==MOVING6) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] !=MOVING6)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] ==MOVING6) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] !=MOVING6)) checker = true;

        if ((grid[piecePositionX][piecePositionY] ==MOVING6) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] ==MOVING6) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] ==MOVING6) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] ==MOVING6) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] !=MOVING6)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] ==MOVING6) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] ==MOVING6) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] ==MOVING6) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] ==MOVING6) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] !=MOVING6)) checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] ==MOVING6) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] ==MOVING6) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] ==MOVING6) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] ==MOVING6) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] !=MOVING6)) checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] ==MOVING6) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] !=MOVING6)) checker = true;
            
        

        if (!checker)
        {
            aux = piece[0][0];
            piece[0][0] = piece[3][0];
            piece[3][0] = piece[3][3];
            piece[3][3] = piece[0][3];
            piece[0][3] = aux;

            aux = piece[1][0];
            piece[1][0] = piece[3][1];
            piece[3][1] = piece[2][3];
            piece[2][3] = piece[0][2];
            piece[0][2] = aux;

            aux = piece[2][0];
            piece[2][0] = piece[3][2];
            piece[3][2] = piece[1][3];
            piece[1][3] = piece[0][1];
            piece[0][1] = aux;

            aux = piece[1][1];
            piece[1][1] = piece[2][1];
            piece[2][1] = piece[2][2];
            piece[2][2] = piece[1][2];
            piece[1][2] = aux;
        }

        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if (grid[i][j] == MOVING1 || grid[i][j]== MOVING2 || grid[i][j]==MOVING3 || grid[i][j] == MOVING4 || grid[i][j]== MOVING5 || grid[i][j]==MOVING6 || grid[i][j]==MOVING7)
                {
                    grid[i][j] = EMPTY;
                }
            }
        }

        for (int i = piecePositionX; i < piecePositionX + 4; i++)
        {
            for (int j = piecePositionY; j < piecePositionY + 4; j++)
            {
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING1) grid[i][j] = MOVING1;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING2) grid[i][j] = MOVING2;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING3) grid[i][j] = MOVING3;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING4) grid[i][j] = MOVING4;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING5) grid[i][j] = MOVING5;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING6) grid[i][j] = MOVING6;
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING7) grid[i][j] = MOVING7;
            }
        }

        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------
//Ping Pong
//----------------------------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 15
#define BALL_SPEED_X 10
#define BALL_SPEED_Y 10

int PingPong() {
    // Initialization
    int level=levelSelector(PING_PONG);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ping Pong");

    SetTargetFPS(level);

    // Score
    int score_a = 0;
    int score_b = 0;

    // Paddle A
    Rectangle paddle_a = { 20, SCREEN_HEIGHT / 2 - 50, 20, 100 };

    // Paddle B
    Rectangle paddle_b = { SCREEN_WIDTH - 40, SCREEN_HEIGHT / 2 - 50, 20, 100 };

    // Ball
    Rectangle ball = { SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10, 20, 20 };
    Vector2 ball_speed = { BALL_SPEED_X, BALL_SPEED_Y };

    while (!WindowShouldClose()) {
        // Update
        if (IsKeyDown(KEY_W) && paddle_a.y > 0) paddle_a.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_S) && paddle_a.y + paddle_a.height < SCREEN_HEIGHT) paddle_a.y += PADDLE_SPEED;
        if (IsKeyDown(KEY_UP) && paddle_b.y > 0) paddle_b.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_DOWN) && paddle_b.y + paddle_b.height < SCREEN_HEIGHT) paddle_b.y += PADDLE_SPEED;

        ball.x += ball_speed.x;
        ball.y += ball_speed.y;

        // Collision with walls
        if ((ball.y + ball.height) >= SCREEN_HEIGHT || ball.y <= 0) ball_speed.y *= -1;

        // Collision with paddles
        if (CheckCollisionRecs(ball, paddle_a) || CheckCollisionRecs(ball, paddle_b)) {
            ball_speed.x *= -1;
            //ball_speed.y = GetRandomValue(-5, 5); // Randomize ball's vertical direction
        }

        // Score update
        if (ball.x + ball.width >= SCREEN_WIDTH) {
            score_a++;
            ball.x = SCREEN_WIDTH / 2 - 10;
            ball.y = SCREEN_HEIGHT / 2 - 10;
            ball_speed.x = -BALL_SPEED_X;
            ball_speed.y = GetRandomValue(-5, 5); // Randomize ball's vertical direction
        }

        if (ball.x <= 0) {
            score_b++;
            ball.x = SCREEN_WIDTH / 2 - 10;
            ball.y = SCREEN_HEIGHT / 2 - 10;
            ball_speed.x = BALL_SPEED_X;
            ball_speed.y = GetRandomValue(-5, 5); // Randomize ball's vertical direction
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);


  
        // Draw paddles and ball
        DrawRectangleRec(paddle_a, BLUE);
        DrawRectangleRec(paddle_b, BLUE);
        DrawRectangleRec(ball, YELLOW);

        // Draw score
        char scoreText[20];
        sprintf(scoreText, "Player A: %d  Player B: %d", score_a, score_b);
        DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 20) / 2, 10, 20, WHITE);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();

    return 0;
}

//----------------------------------------------------------------------------------
//snake
//----------------------------------------------------------------------------------
#define GRID_SIZE 16
#define MAX_CELLS 625 //400*400/(16*16)=screen area / grid area

typedef struct {
    int x;
    int y;
} Vector2Int;

typedef struct {
    Vector2Int position;
    Vector2Int speed;
    Vector2Int cells[MAX_CELLS];
    int maxCells;
} Snake;

typedef struct {
    Vector2Int position;
} Apple;

static int level;

void UpdateSnake(Snake *snake,Apple *apple, int screenWidth, int screenHeight);
void DrawSnake(Snake snake);
void DrawApple(Apple apple);
void ResetGame(Snake *snake, Apple *apple, int screenWidth, int screenHeight);
int ifGridOnSnake(Snake *snake,Apple *apple);

int SnakeGame() {
    const int screenWidth = 400;
    const int screenHeight = 400;

    level=levelSelector(SNAKE);
    
    InitWindow(screenWidth, screenHeight, "Basic Snake Game");
    
    SetTargetFPS(level);

    Snake snake = { {160, 160}, {GRID_SIZE, 0}, {{0}}, 4 };//starting pos. of snake and its length hard coded
    Apple apple = { {320, 320} };//initial pos. of apple hard coded

    while (!WindowShouldClose()) {
        // Input handling
        if (IsKeyDown(KEY_RIGHT) && snake.speed.x != -GRID_SIZE) {
            snake.speed = (Vector2Int){GRID_SIZE, 0};
        } else if (IsKeyDown(KEY_LEFT) && snake.speed.x != GRID_SIZE) {
            snake.speed = (Vector2Int){-GRID_SIZE, 0};
        } else if (IsKeyDown(KEY_DOWN) && snake.speed.y != -GRID_SIZE) {
            snake.speed = (Vector2Int){0, GRID_SIZE};
        } else if (IsKeyDown(KEY_UP) && snake.speed.y != GRID_SIZE) {
            snake.speed = (Vector2Int){0, -GRID_SIZE};
        }

        UpdateSnake(&snake,&apple, screenWidth, screenHeight);

        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawText(TextFormat("Score : %d",(snake.maxCells-4)*10),10,10,20,GREEN);
        DrawSnake(snake);
        DrawApple(apple);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void UpdateSnake(Snake *snake,Apple *apple, int screenWidth, int screenHeight) {
    // Move snake
    snake->position.x += snake->speed.x;
    snake->position.y += snake->speed.y;

    // Wrap snake position
    if (snake->position.x < 0) snake->position.x = screenWidth - GRID_SIZE;
    else if (snake->position.x >= screenWidth) snake->position.x = 0;
    if (snake->position.y < 0) snake->position.y = screenHeight - GRID_SIZE;
    else if (snake->position.y >= screenHeight) snake->position.y = 0;

    // Check collision with apple
    if (CheckCollisionRecs((Rectangle){snake->position.x, snake->position.y, GRID_SIZE, GRID_SIZE}, 
                           (Rectangle){apple->position.x, apple->position.y, GRID_SIZE, GRID_SIZE})) {
        // Increase snake length
        snake->maxCells++;

        // Generate new apple position
        apple->position.x = GetRandomValue(0, screenWidth / GRID_SIZE - 1) * GRID_SIZE;
        apple->position.y = GetRandomValue(0, screenHeight / GRID_SIZE - 1) * GRID_SIZE;
        while(ifGridOnSnake(snake,apple)){
            apple->position.x = GetRandomValue(0, screenWidth / GRID_SIZE - 1) * GRID_SIZE;
            apple->position.y = GetRandomValue(0, screenHeight / GRID_SIZE - 1) * GRID_SIZE;
        }
    }

    //checks collision with itself
    for (int i = 1; i < snake->maxCells; i++) {
        if (snake->position.x == snake->cells[i].x && snake->position.y == snake->cells[i].y) {
            // Reset game
            ResetGame(snake, apple, screenWidth, screenHeight);
            break;
        }
    }

    // Update snake cells
    for (int i = snake->maxCells - 1; i > 0; i--) {
        snake->cells[i] = snake->cells[i - 1];
    }
    snake->cells[0] = snake->position;
}

void DrawSnake(Snake snake) {
    // Draw snake body
    for (int i = 0; i < snake.maxCells; i++) {
        DrawRectangle(snake.cells[i].x, snake.cells[i].y, GRID_SIZE, GRID_SIZE, GREEN);
    }
}

void DrawApple(Apple apple) {
    DrawRectangle(apple.position.x, apple.position.y, GRID_SIZE, GRID_SIZE,RED);
}

void ResetGame(Snake *snake, Apple *apple, int screenWidth, int screenHeight) {
    snake->position = (Vector2Int){160, 160};
    snake->speed = (Vector2Int){GRID_SIZE, 0};
    snake->maxCells = 4;

    apple->position = (Vector2Int){320, 320};
}

int ifGridOnSnake(Snake *snake,Apple *apple){
    for(int i=0;i<snake->maxCells;i++){
        if(snake->cells[i].x==apple->position.x && snake->cells[i].y==apple->position.y)
            return 1;
    }
    return 0;
}