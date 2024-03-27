#include<stdio.h>
#include<raylib.h>
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1600 

Rectangle InitializeRect(Rectangle file,Rectangle prog ,int font_size,int n){
    /*top gray bar is 2 times font_size thickness and the buttons should be 10% of pixel padding away from th eedges
    this means box height is 80% of gray bar thickness width is 4 chracter wide for file and 7 character wide for program*/
    file.x=2*font_size/10.0;
    file.y=2*font_size/10.0;
    file.width=3*font_size; 
    file.height=2*font_size*0.8;

    prog.x=2*file.x+file.width;
    prog.y=2*font_size/10.0;
    prog.width=6*font_size; 
    prog.height=2*font_size*0.8;
    
    if(n==1)
        return file;
    else   
        return prog;
}
int main(){
    //Color grey={255,255,2,255};
    int Font_Size=12;//default font size
    Rectangle file_button;
    Rectangle prog_button;
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT, "code editor");
    SetTargetFPS(60);
    while(!WindowShouldClose()){

        if(IsKeyPressed(KEY_KP_ADD) && Font_Size<50)
            Font_Size+=Font_Size/3;

        if(IsKeyPressed(KEY_KP_SUBTRACT) && Font_Size>12)
            Font_Size-=Font_Size/3;

        file_button=InitializeRect(file_button,prog_button,Font_Size,1);//bad design can be fixed later
        prog_button=InitializeRect(file_button,prog_button,Font_Size,2);
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawRectangle(0,0,Font_Size,WINDOW_HEIGHT,LIGHTGRAY);
            DrawRectangle(0,0,WINDOW_WIDTH,2*Font_Size,GRAY);

            GetFontDefault();

            DrawRectangleRec(file_button,WHITE);
            DrawText("File",file_button.x+3,file_button.y+3,Font_Size,BLACK);

            DrawRectangleRec(prog_button,WHITE);
            DrawText("Program",prog_button.x+3,prog_button.y+3,Font_Size,BLACK);

            
            for(int i=0;i<WINDOW_HEIGHT/Font_Size;i++){
                DrawText("1",Font_Size/3,(i+2)*Font_Size,Font_Size,BLACK);//"1" should be cahned with line no.
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}