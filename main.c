
#include "raylib.h"

// ------------------------------------------------------------------------------------------
// Pantalla
// ------------------------------------------------------------------------------------------
static int Pantalla_Largo, Pantalla_Alto, Pantalla_MitadX, Pantalla_MitadY, Pantalla_X, Pantalla_X2, Pantalla_Medida;


// ------------------------------------------------------------------------------------------
// Medidas
// ------------------------------------------------------------------------------------------
static float Medida, Medida2,Medida3; 
static int Monte_Altura, Monte_Y;
static float Monte_uvx; // Scroll x
static int Marcador_X, Marcador_Y, Marcador_Altura, Marcador_Largo;
static int Radar_X, Radar_Largo, Radar_Alto,Radar_Alto2;

// ------------------------------------------------------------------------------------------
// Nave Principal
// ------------------------------------------------------------------------------------------
static float Nave_X, Nave_Y, Nave_Medida, Nave_Medida2;
static int Nave_X1, Nave_X2, Nave_X3, Nave_X4; // Posiciones segun Orientacion nave: 0-1     0: 1-Izda min  2-Izda max  3-Dcha min 4-Dcha max
static float Nave_Velocidad,Nave_Velocidad_Max,Nave_Desplazamiento,Nave_Desplazamiento_Max,Nave_Direccion;



// capturas mame: 292 x 240 pixeles mame , mostrado en 320x240 ¡Importante

// escenario montañas  2064 x 89 pixeles 

// marcador radar 128 x 32 pixeles



//------------------------------------------------------------------------------------
// Define funciones
//------------------------------------------------------------------------------------
void Inicia_Variables();
void Imagen_Dibuja(Texture2D, int, int, int, int, int, int, int, int, int,int);
void Monte_Dibuja(Texture2D, int, int, int, int, int, int, int, int);
void Marcador_Dibuja(Texture2D);


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

    InitWindow(0, 0, "Defender v1");
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

        float valor2=0.0f;


        // Actualiza
        //----------------------------------------------------------------------------------

        if (IsKeyDown(KEY_Z)) {


            if (Nave_Direccion == 0) {
                //if ((Nave_X + Nave_Desplazamiento) < Nave_X2) {
              //  if (Nave_Desplazamiento > -Medida3) {
                        if (Nave_Velocidad < Medida3)  Nave_Velocidad += Medida3;
                    Nave_Velocidad *= 1.03f;
                    Nave_Desplazamiento += Medida3;
                    Nave_Desplazamiento *= 1.02;
              //  }
            }
            else {
                if (Nave_Velocidad > -Medida3)  Nave_Velocidad -= Medida3;
                Nave_Velocidad *= 1.03f;
                Nave_Desplazamiento -= Medida3;
                Nave_Desplazamiento *= 1.02;
            }


            if (Nave_Desplazamiento < -Nave_Desplazamiento_Max) Nave_Desplazamiento = -Nave_Desplazamiento_Max;
            if (Nave_Desplazamiento > Nave_Desplazamiento_Max) Nave_Desplazamiento = Nave_Desplazamiento_Max;


        }

        if (IsKeyPressed(KEY_X)) {
            if (Nave_Direccion == 0) {
                Nave_Direccion = 1; // izquierda
            } else {
                Nave_Direccion = 0; // derecha
            }
            Nave_X += Nave_Desplazamiento*2;
           // Nave_X += Nave_Desplazamiento;

          //  Nave_Desplazamiento = 0;
            Nave_Desplazamiento  *= -1;
        }


        if (Nave_Direccion == 0) {

       //     if (Nave_X > (Nave_X1 + Nave_Desplazamiento)) {
        //        valor2 = ((Nave_X1 + Nave_Desplazamiento) - Nave_X) * 0.02;

              if (Nave_X > Nave_X1) {
                    //valor2 = (Nave_X1 - Nave_X) * 0.02;
                    valor2 = ((Nave_X1 - Nave_Desplazamiento) - Nave_X) * 0.03;
                Nave_X += valor2;
                Monte_uvx -= (valor2 / Medida);

              }


        } else {

       //     if (Nave_X < (Nave_X3 - Nave_Desplazamiento)) {
        //       valor2 = ( (Nave_X3 - Nave_Desplazamiento) -Nave_X) * 0.02;
              if (Nave_X < Nave_X3  ) {
                  valor2 = ((Nave_X3 - Nave_Desplazamiento) - Nave_X) * 0.03;
                  //  valor2 = (Nave_X3 - Nave_X) * 0.02;
                    Nave_X += valor2;
                    Monte_uvx -= (valor2/Medida);
              }

        }


        Nave_Desplazamiento *= 0.98f;

        Nave_Velocidad *= 0.99f;
        if (Nave_Velocidad > Nave_Velocidad_Max) Nave_Velocidad = Nave_Velocidad_Max;
        if (Nave_Velocidad < -Nave_Velocidad_Max) Nave_Velocidad = -Nave_Velocidad_Max;



        Monte_uvx += Nave_Velocidad; // Scroll  Montañas


        if (Monte_uvx > 2064) Monte_uvx -= 2064; // Tope uvx


        if (IsKeyPressed(KEY_S)) {
            TakeScreenshot("captura.png");
        }

        //----------------------------------------------------------------------------------



        // Dibuja
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);


        // Dibuja Montañas
        Monte_Dibuja(i_monte, Pantalla_X, Monte_Y, Pantalla_Medida, Monte_Altura, Monte_uvx, 0, 292, 89);

        // Dibuja Nave
        if (Nave_Direccion == 0) {
//            Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64, 0, 15, 6, 1, Nave_Direccion);
            Imagen_Dibuja(i_sprites, Nave_X+ Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64, 0, 15, 6, 1, Nave_Direccion);
        }
        else {
            Imagen_Dibuja(i_sprites, Nave_X+ Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64, 0, 15, 6, 1, Nave_Direccion);
        }

        // Marcadores
        Marcador_Dibuja(i_sprites);

        // Limites para nave
        DrawLine(Nave_X1, 0, Nave_X1, Pantalla_Alto, RED);
        DrawLine(Nave_X2, 0, Nave_X2, Pantalla_Alto, RED);
        DrawLine(Nave_X3, 0, Nave_X3, Pantalla_Alto, BLUE);
        DrawLine(Nave_X4, 0, Nave_X4, Pantalla_Alto, RED);

        // Mascara
        DrawRectangle(0, 0, Pantalla_X, Pantalla_Alto, BLACK);
        DrawRectangle(Pantalla_X2, 0, Pantalla_X, Pantalla_Alto, BLACK);


        DrawText(TextFormat("Nave_Desplazamiento %f", Nave_Desplazamiento), 10, 10, 30, WHITE);


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

    Medida = Pantalla_Alto / 240.0f; // Medida de un pixel segun resolucion

    Pantalla_Medida = (Pantalla_Alto * 320) / 240;
    Pantalla_X = (Pantalla_Largo - Pantalla_Medida) / 2;
    Pantalla_X2 = Pantalla_Largo - Pantalla_X;

    // No Quitar - Pantalla completa sin margenes a los lados
    /*
    Pantalla_Medida = Pantalla_Largo;
    Pantalla_X = 0;
    Pantalla_X2 = Pantalla_Largo;
    */

    Medida2 = Medida * 2.0f;
    Medida3 = Medida * 0.3f;

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
    Radar_Largo = Medida * 140.0f;
    Radar_Alto = Medida * 32.0f;
    Marcador_Y = Radar_Alto + Medida;


}




void Monte_Dibuja(Texture2D i_monte, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto) {
    origin.x = 0;
    origin.y = 0;
    Rectangle sourceRec = { uvx, uvy, uvlargo, uvalto };
    Rectangle destRec = { x, y, largo, alto };
    DrawTexturePro(i_monte, sourceRec, destRec, origin, 0, WHITE);
}


void Imagen_Dibuja(Texture2D i_sprites, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto, int centrado,int espejo) {

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
    Imagen_Dibuja(i_sprites, Radar_X, Medida, Radar_Largo, Radar_Alto, 0, 96, 128, 32, 0,0); // Radar

}