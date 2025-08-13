
#include "raylib.h"

// capturas mame: 292 x 240 pixeles mame , mostrado en pantalla a 320x240 ¡Importante

// escenario montañas  2064 x 89 pixeles (No usado)

// marcador radar 128 x 32 pixeles

// Tileado:  129 tiles de largo, 16x16 pixeles cada uno. total 129*16 = 2064;
// Scroll: 0 - 2064;

// Enemigos: el enemigo se mueve en un rectangulo: x: 0 - 2064    y:ejm 0 - Pantalla_Alto




// ------------------------------------------------------------------------------------------
// Dispositivo
// ------------------------------------------------------------------------------------------
static int Pantalla_Largo, Pantalla_Alto, Pantalla_MitadX, Pantalla_MitadY, Pantalla_X, Pantalla_X2, Pantalla_Medida;


// ------------------------------------------------------------------------------------------
// Medidas
// ------------------------------------------------------------------------------------------
static float Tile_Medida, Tile_Medida2, Tile_Scroll, Tile_Medida16; // Medidas para tileado
static float Medida, Medida2, Medida3; // Medidas para Naves y Enemigos, Marcadores, disparos,explosiones
static int Monte_Altura, Monte_Y;
static float Scroll, Scroll_Fin; // Scroll x
static int Marcador_X, Marcador_Y, Marcador_Altura, Marcador_Largo;
static int Radar_X, Radar_Largo, Radar_Alto, Radar_Alto2;


// ------------------------------------------------------------------------------------------
// Nave Principal
// ------------------------------------------------------------------------------------------
static float Nave_X, Nave_Y, Nave_Medida, Nave_Medida2;
static int Nave_X1, Nave_X2, Nave_X3, Nave_X4; // Posiciones segun Orientacion nave: 0-1     0: 1-Izda min  2-Izda max  3-Dcha min 4-Dcha max
static float Nave_Velocidad, Nave_Velocidad_Max, Nave_Desplazamiento, Nave_Desplazamiento_Max, Nave_Direccion;


// ------------------------------------------------------------------------------------------
// Enemigos
// ------------------------------------------------------------------------------------------
static float Enemigo_X, Enemigo_Y; // Ejemplo debug

// No quitar estas medidas, establecerlas dentro de la estructura enemigo, sirven para mostrar su imagen y el sistema de colision para disparos, naves, ... 
// : ejm .medida_largo=Enemigo_Medida11, .medida_alto=Enemigo_Medida4  = Baiter
static float Enemigo_Medida4, Enemigo_Medida5, Enemigo_Medida6,Enemigo_Medida7, Enemigo_Medida8,Enemigo_Medida9,Enemigo_Medida11; // Medidas segun resolucion


// ------------------------------------------------------------------------------------------
// Tileado
// ------------------------------------------------------------------------------------------
static int Tile_N;
int Tile[130];


// ------------------------------------------------------------------------------------------
// Animacion
// ------------------------------------------------------------------------------------------
static int Frame_c,Frame; // velocidad contador,  frame 0,16,32 uvx


//------------------------------------------------------------------------------------
// Define funciones
//------------------------------------------------------------------------------------
void Inicia_Variables();
void Imagen_Dibuja(Texture2D, int, int, int, int, int, int, int, int, int, int);
void Monte_Dibuja(Texture2D, int, int, int, int, int, int, int, int);
void Marcador_Dibuja(Texture2D);
void Enemigo_Dibuja(Texture2D);
void Tileado_Dibuja(Texture2D);


//------------------------------------------------------------------------------------
// Variables para dibujo
//------------------------------------------------------------------------------------
static Vector2 position;
static double rotation;
static Vector2 origin;


//------------------------------------------------------------------------------------
//  P R I N C I P A L
//------------------------------------------------------------------------------------
int main(void)
{

    // Inicialización
    //--------------------------------------------------------------------------------------

//    InitWindow(1024, 768, "Defender v2");
    InitWindow(0, 0, "Defender v2");
    ToggleBorderlessWindowed();
    ClearWindowState(FLAG_WINDOW_TOPMOST);

    Inicia_Variables();

    Texture2D i_sprites = LoadTexture("assets/sprites.png");
    Texture2D i_monte = LoadTexture("assets/monte1.png");

    SetTargetFPS(60);


    //--------------------------------------------------------------------------------------

    // Bucle Principal
    while (!WindowShouldClose())
    {

        float valor2 = 0.0f; // Quitar esto cuando se cree la funcion para nave principal



        // Animacion
        //---------------------------------------------------------------------------------------------------------
        Frame_c += 1;
        if (Frame_c == 5) { // Velocidad de animacion
            Frame_c = 0;
            Frame += 16;
            if (Frame == 48) Frame = 0;
        }

        // Control Nave
        //---------------------------------------------------------------------------------------------------------
        if (IsKeyDown(KEY_Z)) {
            if (Nave_Direccion == 0) {
                if (Nave_Velocidad < Medida3)  Nave_Velocidad += Medida3;
                Nave_Desplazamiento += Medida3;
            }
            else {
                if (Nave_Velocidad > -Medida3)  Nave_Velocidad -= Medida3;
                Nave_Desplazamiento -= Medida3;
            }
            Nave_Velocidad *= 1.03f;
            Nave_Desplazamiento *= 1.02;
            if (Nave_Desplazamiento < -Nave_Desplazamiento_Max) Nave_Desplazamiento = -Nave_Desplazamiento_Max;
            if (Nave_Desplazamiento > Nave_Desplazamiento_Max) Nave_Desplazamiento = Nave_Desplazamiento_Max;
        }

        if (IsKeyPressed(KEY_X)) {
            if (Nave_Direccion == 0) {
                Nave_Direccion = 1; // izquierda
            }
            else {
                Nave_Direccion = 0; // derecha
            }
            Nave_X += Nave_Desplazamiento * 2;
            Nave_Desplazamiento *= -1;
        }


        if (Nave_Direccion == 0) { // Derecha
            if (Nave_X > Nave_X1) {
                valor2 = ((Nave_X1 - Nave_Desplazamiento) - Nave_X) * 0.03;
                Nave_X += valor2;
                Scroll -= (valor2 / Tile_Medida16);
                Tile_Scroll += valor2;
            }
        } else { // Izquierda
            if (Nave_X < Nave_X3) {
                valor2 = ((Nave_X3 - Nave_Desplazamiento) - Nave_X) * 0.03;
                Nave_X += valor2;
                Scroll -= (valor2 / Tile_Medida16);
                Tile_Scroll += valor2;
            }
        }

        Nave_Desplazamiento *= 0.98f;
        Nave_Velocidad *= 0.99f;
        if (Nave_Velocidad > Nave_Velocidad_Max) Nave_Velocidad = Nave_Velocidad_Max;
        if (Nave_Velocidad < -Nave_Velocidad_Max) Nave_Velocidad = -Nave_Velocidad_Max;


        // Scroll
        //---------------------------------------------------------------------------------------------------------
        Scroll += Nave_Velocidad;// Montañas 0-2064 (129 tiles x 16 pixeles ) = 2064 pixeles
        Tile_Scroll -= Nave_Velocidad * Tile_Medida16;
        if (Tile_Scroll < Tile_Medida) {
            Tile_Scroll += Tile_Medida;
            Tile_N += 1;
        }
        if (Tile_Scroll > Tile_Medida) {
            Tile_Scroll -= Tile_Medida;
            Tile_N -= 1;
        }
        if (Tile_N > 128)  Tile_N = 0;// Limites
        if (Tile_N < 0)  Tile_N = 128;
        if (Scroll > 2064) Scroll -= 2064; 
        if (Scroll < 0) Scroll += 2064; 


        // Rutinas
        //---------------------------------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_S)) {
            TakeScreenshot("captura.png");
        }





        //---------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------
        //      D I B U J A
        //---------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------

        BeginDrawing();
        ClearBackground(BLACK);


        // Dibuja Montañas
      //  Monte_Dibuja(i_monte, Pantalla_X, Monte_Y, Pantalla_Medida, Monte_Altura, Scroll, 0, 292, 89);

        // Dibuja Nave
        Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64+Frame, 0, 15, 6, 1, Nave_Direccion);

        // Dibuja Enemigo
        Enemigo_Dibuja(i_sprites);

        // Marcadores
        Marcador_Dibuja(i_sprites);

        // Limites para nave
        /*
        DrawLine(Nave_X1, 0, Nave_X1, Pantalla_Alto, RED);
        DrawLine(Nave_X2, 0, Nave_X2, Pantalla_Alto, RED);
        DrawLine(Nave_X3, 0, Nave_X3, Pantalla_Alto, BLUE);
        DrawLine(Nave_X4, 0, Nave_X4, Pantalla_Alto, RED);
        */

        Tileado_Dibuja(i_sprites);

        // Mascara
        DrawRectangle(0, 0, Pantalla_X, Pantalla_Alto, BLACK);
        DrawRectangle(Pantalla_X2, 0, Pantalla_X, Pantalla_Alto, BLACK);

        // Info
      //  DrawText(TextFormat("Scroll %f", Scroll), 10, 10, 30, WHITE);
      //  DrawText(TextFormat("Tile_N %i", Tile_N), 10, 50, 30, WHITE);
      //  DrawText(TextFormat("Enex %f", enex), 10, 90, 30, GREEN);
      //  DrawText(TextFormat("FIn %f", Scroll_Fin), 10, 130, 30, GREEN);
     //   DrawText(TextFormat("Enemigo_X %f", Enemigo_X), 10, 180, 30, GREEN);


        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Inicialización
    //--------------------------------------------------------------------------------------
    UnloadTexture(i_sprites);
    UnloadTexture(i_monte);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}



void Inicia_Variables() {

    Pantalla_Largo = GetScreenWidth();
    Pantalla_Alto = GetScreenHeight();
    Pantalla_MitadX = Pantalla_Largo / 2;
    Pantalla_MitadY = Pantalla_Alto / 2;

    Monte_Altura = (Pantalla_Alto * 89) / 240;
    Monte_Y = Pantalla_Alto - Monte_Altura;

    Tile_Medida = (Pantalla_Largo / 18.0f);
    Tile_Medida2 = Tile_Medida / 2.0f;
    Tile_Medida16 = Tile_Medida / 16.0f;

    Medida =  Pantalla_Alto / 240.0f; // Medida de un pixel segun resolucion
    Medida2 = Medida * 2.0f;
    Medida3 = Medida * 0.3f;

    Pantalla_Medida = (Pantalla_Alto * 320) / 240;
    Pantalla_X = (Pantalla_Largo - Pantalla_Medida) / 2;
    Pantalla_X2 = Pantalla_Largo - Pantalla_X;

    // No Quitar - Pantalla completa sin margenes a los lados
    /*
    Pantalla_Medida = Pantalla_Largo;
    Pantalla_X = 0;
    Pantalla_X2 = Pantalla_Largo;
    */


    Nave_Medida = Medida * 15.0f; //15x6 pixeles
    Nave_Medida2 = Medida * 6.0f;
    Nave_X1 = Pantalla_X + (Medida * 60.0f);
    Nave_X2 = Pantalla_X + (Medida * 100.0f);
    Nave_X3 = Pantalla_X2 - (Medida * 60.0f);
    Nave_X4 = Pantalla_X2 - (Medida * 100.0f);

    // Posicion inicial nave
    Nave_X = Nave_X1;
    Nave_Y = Pantalla_MitadY;
    Nave_Velocidad_Max = Medida * 1.6f;
    Nave_Desplazamiento_Max = (Nave_X2 - Nave_X1);
    Nave_Desplazamiento = 0;
    Nave_Direccion = 0;

    // Marcadores
    Marcador_Altura = 35 * Medida;
    Marcador_Largo = 145 * Medida;
    Marcador_X = Pantalla_MitadX - (Marcador_Largo / 2);

    // Radar
    Radar_X = Marcador_X + (Medida * 2);
    Radar_Largo = Marcador_Largo - (Medida*4); // Medida * 140.0f;
    Radar_Alto = Medida * 32.0f;
    Marcador_Y = Radar_Alto + Medida;


    // Enemigos
    Enemigo_X = 128;
    Enemigo_Y = Pantalla_MitadY + Medida2;

    // Rectangulo para colision , (Los enemigos no tienen el origin en 0,0, el rectangulo se debe centrar segun sus medidas)
    // ejm Baiter
    // x1 del rectangulo = Enemigo_X-(Enemigo_Medida11/2)
    // y1 del rectangulo = Enemigo_Y-(Enemigo_Medida4/2)
    // Seria bueno que todas estas divisiones /2 esten dentro de la estructura del enemigo para evitar divisiones
    Enemigo_Medida4 = Medida * 4.0f;
    Enemigo_Medida5 = Medida * 5.0f;
    Enemigo_Medida6 = Medida * 6.0f;
    Enemigo_Medida7 = Medida * 7.0f;
    Enemigo_Medida8 = Medida * 8.0f;
    Enemigo_Medida9 = Medida * 9.0f;
    Enemigo_Medida11 = Medida * 11.0f;


    // Animacion
    Frame = 0; //0-16-32
    Frame_c = 0;


    // Scroll
    Scroll_Fin = Tile_Medida16 * 2064;
    for (int i = 0; i < 129; i++) {
        Tile[i] = GetRandomValue(0, 1);
    }
    Tile_N = 0;
    Tile_Scroll = 0.0f;
    Tile[1] = 2;
    Tile[123] = 3;


}


void Tileado_Dibuja(Texture2D i_sprites) {

    int i, valor;
    int x = -Tile_Medida + Tile_Scroll;
    int y = 900;

    int num;


    for (i = 0; i < 20; i++) {

        num = Tile_N + i;
        if (num > 129) num = num - 129;
        if (num < 1) num = num + 129;

        valor = Tile[num];

        Imagen_Dibuja(i_sprites, x, y, Tile_Medida, Tile_Medida, 240, valor * 16, 16, 16, 0, 0);

        DrawText(TextFormat("%i", num), x + Tile_Medida2, y + Tile_Medida2, 30, GREEN);

        x += Tile_Medida;

    }


}


void Enemigo_Dibuja(Texture2D i_sprites) {

    float x;

    Enemigo_X += 3;
    if (Enemigo_X > 2064.0f) Enemigo_X -= 2064.0f;
    if (Enemigo_X < 0.0f) Enemigo_X += 2064.0f;

    x = (Enemigo_X - (Scroll)) * Tile_Medida16;
    if (x < 0) x += Scroll_Fin;

    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida9, Enemigo_Medida8, 128 + Frame, 0, 9, 8, 1, 0); // Lander
//    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida9, Enemigo_Medida8, 176 + Frame, 16, 9, 8, 1, 0); // Mutant
//    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida11, Enemigo_Medida4, 224, 0, 11, 4, 1, 0); // Baiter
//    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida6, Enemigo_Medida7, 128 + Frame, 16, 7, 7, 1, 0); // Bomber
//    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida7, Enemigo_Medida7, 176 + Frame, 0, 7, 7, 1, 0); // Pod
//    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida5, Enemigo_Medida4, 96, 48, 5, 4, 1, 0); // Swarmer

}



void Monte_Dibuja(Texture2D i_monte, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto) {
    origin.x = 0;
    origin.y = 0;
    Rectangle sourceRec = { uvx, uvy, uvlargo, uvalto };
    Rectangle destRec = { x, y, largo, alto };
    DrawTexturePro(i_monte, sourceRec, destRec, origin, 0, WHITE);
}


void Imagen_Dibuja(Texture2D i_sprites, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto, int centrado, int espejo) {

    if (centrado == 1) {
        origin.x = largo / 2.0f;
        origin.y = alto / 2.0f;
    }
    else {
        origin.x = 0;
        origin.y = 0;
    }

    if (espejo == 1) uvy += 16;
    Rectangle sourceRec = { uvx, uvy, uvlargo, uvalto };
    Rectangle destRec = { x, y, largo, alto };
    DrawTexturePro(i_sprites, sourceRec, destRec, origin, 0, WHITE);
}


void Marcador_Dibuja(Texture2D i_sprites) {

    DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, BLUE); // Borde Radar central
    DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, BLUE);
    Imagen_Dibuja(i_sprites, Radar_X, Medida, Radar_Largo, Radar_Alto, 0, 64, 128, 32, 0, 0); // Radar

}