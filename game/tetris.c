#include<raylib.h>

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
static Color backgroundColor=(Color){139,69,19,255};
static Color TextColor=(Color){255,253,208,255}; //Cream Color with full Alpha
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
int main(void){
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
                DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, YELLOW);
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
                        DrawLine(offset.x, offset.y, offset.x + SQUARE_SIZE, offset.y, LIGHTGRAY );
                        DrawLine(offset.x, offset.y, offset.x, offset.y + SQUARE_SIZE, LIGHTGRAY );
                        DrawLine(offset.x + SQUARE_SIZE, offset.y, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, LIGHTGRAY );
                        DrawLine(offset.x, offset.y + SQUARE_SIZE, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, LIGHTGRAY );
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
                        DrawRectangle(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, YELLOW);
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
        case 0: { incomingPiece[1][1] = MOVING1; incomingPiece[2][1] = MOVING1; incomingPiece[1][2] = MOVING1; incomingPiece[2][2] = MOVING1; } break;    //Cube
        case 1: { incomingPiece[1][0] = MOVING2; incomingPiece[1][1] = MOVING2; incomingPiece[1][2] = MOVING2; incomingPiece[2][2] = MOVING2; } break;    //L
        case 2: { incomingPiece[1][2] = MOVING3; incomingPiece[2][0] = MOVING3; incomingPiece[2][1] = MOVING3; incomingPiece[2][2] = MOVING3; } break;    //L inversa
        case 3: { incomingPiece[0][1] = MOVING4; incomingPiece[1][1] = MOVING4; incomingPiece[2][1] = MOVING4; incomingPiece[3][1] = MOVING4; } break;    //Recta
        case 4: { incomingPiece[1][0] = MOVING5; incomingPiece[1][1] = MOVING5; incomingPiece[1][2] = MOVING5; incomingPiece[2][1] = MOVING5; } break;    //Creu tallada
        case 5: { incomingPiece[1][1] = MOVING6; incomingPiece[2][1] = MOVING6; incomingPiece[2][2] = MOVING6; incomingPiece[3][2] = MOVING6; } break;    //S
        case 6: { incomingPiece[1][2] = MOVING7; incomingPiece[2][2] = MOVING7; incomingPiece[2][1] = MOVING7; incomingPiece[3][1] = MOVING7; } break;    //S inversa
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