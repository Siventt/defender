#include <stdio.h>

#include "raylib.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaraciones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ASSETS_PATH "./assets/"


#define ORIGINAL_ALTO  240
#define ORIGINAL_ANCHO 292
#define ORIGINAL_TILE 16
#define ORIGINAL_Y_TILE 15
#define ORIGINAL_X_TILE 18

typedef struct {
    int tesela; // Dimensiones de una tesela en pixeles
    int tile; // Dimensiones de un tile en pixeles
    int y_tileS[ORIGINAL_Y_TILE]; // Hay 15 tiles por columna
    int x_tileS[ORIGINAL_X_TILE]; // Hay 18 tiles por fila

    
} Medida;

void CalcularMedidas(Medida *medidas);

void DibujarInterfaz(Medida *medidas);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// P R I N C I P A L
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main()
{
    // Cargar configuración
    int ventana_ancho = 1920;
    int ventana_alto = 1080;

    //

    InitWindow(ventana_ancho, ventana_alto, "Defender");
    ToggleBorderlessWindowed();

    //

    Texture2D t_atlas = LoadTexture(ASSETS_PATH"atlas.png");

    //

    Medida medidas;
    
    CalcularMedidas(&medidas);

    //

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        

        DibujarInterfaz(&medidas);

        // Dibuja la cuadricula
        for(int i = 0; i < ORIGINAL_X_TILE; i++)
            for(int j = 0; j < ORIGINAL_Y_TILE; j++)
                DrawRectangleLines(medidas.x_tileS[i], medidas.y_tileS[j], medidas.tile, medidas.tile, RED);

        // Dibuja solo un tile
        DrawRectangleLines(0, medidas.y_tileS[1], medidas.tile, medidas.tile, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Definiciones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void CalcularMedidas(Medida *medidas)
{
    medidas->tile = GetScreenHeight() / ORIGINAL_Y_TILE; // Se asume medida vertical menor
    medidas->tesela = medidas->tile / ORIGINAL_TILE;
    for (int i = 0; i < ORIGINAL_Y_TILE; i++)
        medidas->y_tileS[i] = medidas->tile * i;
    for (int i = 0; i < ORIGINAL_X_TILE; i++)
        medidas->x_tileS[i] = medidas->tile * i;
}


void DibujarInterfaz(Medida *medidas)
{
    // Linea horizontal
    DrawLineEx((Vector2){0, medidas->y_tileS[2] + medidas->tesela*2}, 
               (Vector2){medidas->x_tileS[17] + medidas->tesela, medidas->y_tileS[2] + medidas->tesela*2}, 
               medidas->tesela*2, DARKBLUE);

    // Recuadro del minimapa
    DrawRectangleLinesEx((Rectangle){medidas->x_tileS[5] + medidas->tesela*2, -medidas->tesela, 
                                     medidas->tile*8, medidas->tile*2 + medidas->tesela*4}, 
                         medidas->tesela*2, DARKBLUE);
}
