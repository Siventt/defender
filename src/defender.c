#include <stdio.h>

#include "raylib.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaraciones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ASSETS_PATH "./assets/"


#define ORIGINAL_ALTO  240
#define ORIGINAL_ANCHO 292
#define ORIGINAL_TESELA 16
#define ORIGINAL_Y_TESELA 15
#define ORIGINAL_X_TESELA 18 // Para 1920x1080 serian 30

typedef struct {
    int pixel;
    int tesela; // 16 pixels

    int y_tesela[ORIGINAL_Y_TESELA]; // Hay 15 teselas por columna
    int x_tesela[ORIGINAL_X_TESELA]; // Hay 18 teselas por fila

} Medida;

void IniciarMedidas(Medida *medida);

void DibujarInterfaz(Medida *medida);

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
    
    IniciarMedidas(&medidas);

    //

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);



        

        DibujarInterfaz(&medidas);

        // Dibuja la cuadricula
        for(int i = 0; i < ORIGINAL_X_TESELA; i++)
            for(int j = 0; j < ORIGINAL_Y_TESELA; j++)
                DrawRectangleLines(medidas.x_tesela[i], medidas.y_tesela[j], medidas.tesela, medidas.tesela, RED);

        // Dibuja solo una tesela
        //DrawRectangleLines(0, medidas.y_tesela[1], medidas.tesela, medidas.tesela, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Definiciones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void IniciarMedidas(Medida *medidas)
{
    medidas->pixel = GetScreenHeight() / ORIGINAL_ALTO;
    medidas->tesela = medidas->pixel * ORIGINAL_TESELA;

    for (int i = 0; i < ORIGINAL_Y_TESELA; i++)
        medidas->y_tesela[i] = medidas->tesela * i;

    for (int i = 0; i < ORIGINAL_X_TESELA; i++)
        medidas->x_tesela[i] = medidas->tesela * i;

}


void DibujarInterfaz(Medida *medidas)
{
    // Linea horizontal
    DrawLineEx((Vector2){0, medidas->y_tesela[2] + medidas->pixel*2}, 
               (Vector2){medidas->x_tesela[17] + medidas->tesela, medidas->y_tesela[2] + medidas->pixel*2}, 
               medidas->pixel*2, DARKBLUE);

    // Recuadro del minimapa
    DrawRectangleLinesEx((Rectangle){medidas->x_tesela[5] + medidas->pixel*2, -medidas->pixel, 
                                     medidas->tesela*8, medidas->tesela*2 + medidas->pixel*4}, 
                         medidas->pixel*2, DARKBLUE);
}
