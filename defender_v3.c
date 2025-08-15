
#include "raylib.h"
#include <stdio.h>  // Para sprintf

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
static int Pantalla_Completa = 0;

// ------------------------------------------------------------------------------------------
// Medidas
// ------------------------------------------------------------------------------------------
static float Tile_Medida, Tile_Medida2, Tile_Scroll, Tile_Medida16, Tile_Monte, Tile_Medida_Monte; // Medidas para tileado
static float Medida, Medida2, Medida3, Medida4;; // Medidas para Naves y Enemigos, Marcadores, disparos, explosiones
static float Scroll, Scroll_Fin, Radar_M; 
static int Marcador_X, Marcador_Y, Marcador_Altura, Marcador_Largo;
static int Radar_X, Radar_Largo, Radar_Alto, Radar_Alto2, Radar_CentroX, Radar_CentroM;



// ------------------------------------------------------------------------------------------
// Nave Principal
// ------------------------------------------------------------------------------------------
static float Nave_X, Nave_Y, Nave_Medida, Nave_Medida2;
static int Nave_X1, Nave_X2, Nave_X3, Nave_X4; // Posiciones segun Orientacion nave: 0-1     0: 1-Izda min  2-Izda max  3-Dcha min 4-Dcha max
static int Nave_Y1, Nave_Y2; // Topes de pantalla
static float Nave_Velocidad, Nave_Velocidad_Max, Nave_Desplazamiento, Nave_Desplazamiento_Max, Nave_Direccion,Nave_Ascenso;



// ------------------------------------------------------------------------------------------
// Marcadores
// ------------------------------------------------------------------------------------------
static int Puntos; 
static char Puntos_Texto[32]; // Buffer para el texto
static int Puntos_X, Puntos_Y;
static int Puntos_Ancho;
static int Bombas,Bombas_X,Bombas_Y;
static int Vidas, Vidas_X, Vidas_Y;

//static int distante;

// ------------------------------------------------------------------------------------------
// Enemigos 
// ------------------------------------------------------------------------------------------
static float Enemigo_X, Enemigo_Y;  
// Ejemplo para enemigos, su rango x es de 0 a 2064. 

// No quitar estas medidas, establecerlas dentro de la estructura enemigo, sirven para mostrar su imagen y el sistema de colision para disparos, naves, ... 
// : ejm .medida_largo=Enemigo_Medida11, .medida_alto=Enemigo_Medida4  = Baiter
static float Enemigo_Medida3,Enemigo_Medida4, Enemigo_Medida5, Enemigo_Medida6, Enemigo_Medida7, Enemigo_Medida8, Enemigo_Medida9, Enemigo_Medida11,Enemigo_Medida13; // Medidas segun resolucion


// ------------------------------------------------------------------------------------------
// Humanoide 
// ------------------------------------------------------------------------------------------
static float Humanoide_X, Humanoide_Y; 
// Ejemplo para humanoides



// ------------------------------------------------------------------------------------------
// Tileado
// ------------------------------------------------------------------------------------------
static int Tile_N;
int Tile[130][5];
int Atlas[160][2];



// ------------------------------------------------------------------------------------------
// Animacion
// ------------------------------------------------------------------------------------------
static int Frame_c, Frame; // velocidad contador,  frame 0,16,32 uvx



// ------------------------------------------------------------------------------------------
// Archivo para tileado
// ------------------------------------------------------------------------------------------
#define STORAGE_DATA_FILE   "assets/nivel.data"


//------------------------------------------------------------------------------------
// Define funciones
//------------------------------------------------------------------------------------
void Inicia_Variables();
void Imagen_Dibuja(Texture2D, int, int, int, int, int, int, int, int, int, int);
void Marcador_Dibuja(Texture2D,Font);
void Enemigo_Dibuja(Texture2D);
void Tileado_Dibuja(Texture2D);
void Humanoide_Dibuja(Texture2D);
void Puntua(Font,int);
void Carga();


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

//    InitWindow(1024, 768, "Defender v3");
    InitWindow(0, 0, "Defender v3");
    ToggleBorderlessWindowed();
    ClearWindowState(FLAG_WINDOW_TOPMOST);

    Inicia_Variables();

    Font fuente;
    fuente = LoadFont("assets/fuente.png");

    Texture2D i_sprites = LoadTexture("assets/sprites.png");

    SetTargetFPS(60);


    Carga();
    Puntua(fuente,0); // Renueva datos puntuacion

    HideCursor();


    //--------------------------------------------------------------------------------------

    // Bucle Principal
    while (!WindowShouldClose())
    {

        float valor2 = 0.0f; // Quitar esto cuando se cree la funcion para nave principal

        // Animaciones
        //---------------------------------------------------------------------------------------------------------
        Frame_c += 1;
        if (Frame_c == 5) { // Velocidad de animacion
            Frame_c = 0;
            Frame += 16;
            if (Frame == 48) Frame = 0;
        }

        // Control Nave
        //---------------------------------------------------------------------------------------------------------
        if (IsKeyDown(KEY_P)) { 
            Puntua(fuente,10);
        }

        if (IsKeyDown(KEY_Q)) {
            Nave_Y -= Nave_Ascenso;
            if (Nave_Y < Nave_Y1) Nave_Y = Nave_Y1;
        }
        if (IsKeyDown(KEY_A)) {
            Nave_Y += Nave_Ascenso;
            if (Nave_Y > Nave_Y2) Nave_Y = Nave_Y2;
        }

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
        }
        else { // Izquierda
            if (Nave_X < Nave_X3) {
                valor2 = ((Nave_X3 - Nave_Desplazamiento) - Nave_X) * 0.03;
                Nave_X += valor2;
                Scroll -= (valor2 / Tile_Medida16);
                Tile_Scroll += valor2;
            }
        }

        Nave_Desplazamiento *= 0.98f;
        Nave_Velocidad *=  0.99f;
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


        // Captura de pantalla
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
        Tileado_Dibuja(i_sprites);// 

        // Dibuja Nave
        Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64 + Frame, 0, 15, 6, 1, Nave_Direccion);

        // Dibuja Enemigo
        Enemigo_Dibuja(i_sprites);

        // Dibuja Humanoides
        Humanoide_Dibuja(i_sprites);

        // Marcadores
        Marcador_Dibuja(i_sprites,fuente);

        // Limites para nave
        /*
        DrawLine(Nave_X1, 0, Nave_X1, Pantalla_Alto, RED);
        DrawLine(Nave_X2, 0, Nave_X2, Pantalla_Alto, RED);
        DrawLine(Nave_X3, 0, Nave_X3, Pantalla_Alto, BLUE);
        DrawLine(Nave_X4, 0, Nave_X4, Pantalla_Alto, RED);
        */


     // Mascara
        if (Pantalla_Completa == 0) {
            DrawRectangle(0, 0, Pantalla_X, Pantalla_Alto, BLACK);
            DrawRectangle(Pantalla_X2, 0, Pantalla_X, Pantalla_Alto, BLACK);
        }

    //    DrawLine(Pantalla_MitadX, 0, Pantalla_MitadX, Pantalla_Alto, GREEN);

            
        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Inicialización
    //--------------------------------------------------------------------------------------
    UnloadFont(fuente);
    UnloadTexture(i_sprites);
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;

}



void Inicia_Variables() {


    int num;

    Pantalla_Largo = GetScreenWidth();
    Pantalla_Alto = GetScreenHeight();
    Pantalla_MitadX = Pantalla_Largo / 2;
    Pantalla_MitadY = Pantalla_Alto / 2;

    Medida = Pantalla_Alto / 240.0f; // Medida de un pixel segun resolucion
    Medida2 = Medida * 2.0f;
    Medida3 = Medida * 0.3f;
    Medida4 = Medida * 3;

    Tile_Medida_Monte = Medida * 16; // tile cuadrado, Tile_Medida_Monte = Tile_Medida (sustituir); Aqui se encoge altura
    Tile_Monte = Pantalla_Alto - (Medida * 90);


    // Pantalla completa sin margenes a los lados
    if (Pantalla_Completa == 0) {
        Pantalla_Medida = (Pantalla_Alto * 320) / 240;
        Pantalla_X = (Pantalla_Largo - Pantalla_Medida) / 2;
        Pantalla_X2 = Pantalla_Largo - Pantalla_X;
        Tile_Medida = (Pantalla_Medida / 18.0f);
    } else {
        Pantalla_Medida = Pantalla_Largo;
        Pantalla_X = 0;
        Pantalla_X2 = Pantalla_Largo;
        Tile_Medida = (Pantalla_Largo / 18.0f);
    }

    Tile_Medida2 = Tile_Medida / 2.0f;
    Tile_Medida16 = Tile_Medida / 16.0f;

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
    Radar_Largo = Marcador_Largo - (Medida * 4);
    Radar_Alto = Medida * 32.0f;
    Marcador_Y = Radar_Alto + Medida;
    Radar_CentroM= 16 * Medida;
    Radar_CentroX = Pantalla_MitadX - (Radar_CentroM / 2);
    Radar_M = (140.0f * Medida) / 2.0f;

    // Humanoide
    Humanoide_X = 700;
    Humanoide_Y = Pantalla_Alto - (Medida*14);

    // Enemigos
    Enemigo_X = Nave_X;// 128;
    Enemigo_Y = Tile_Medida2 + Marcador_Y;

    // Rectangulo para colision , (Los enemigos no tienen el origin en 0,0, el rectangulo se debe centrar segun sus medidas)
    // ejm Baiter : medidas 11x4
    // x1 del rectangulo = Enemigo_X-(Enemigo_Medida11/2)
    // y1 del rectangulo = Enemigo_Y-(Enemigo_Medida4/2)
    // Seria bueno que todas estas divisiones /2 esten dentro de la estructura del enemigo para evitar divisiones
    Enemigo_Medida3 = Medida * 3.0f;
    Enemigo_Medida4 = Medida * 4.0f;
    Enemigo_Medida5 = Medida * 5.0f;
    Enemigo_Medida6 = Medida * 6.0f;
    Enemigo_Medida7 = Medida * 7.0f;
    Enemigo_Medida8 = Medida * 8.0f;
    Enemigo_Medida9 = Medida * 9.0f;
    Enemigo_Medida11 = Medida * 11.0f;
    Enemigo_Medida13 = Medida * 13.0f;


    // Bombas
    Bombas = 3;
    Bombas_X = Marcador_X - (Medida*7);
    Bombas_Y = Marcador_Altura -(Medida*6);

    // Puntos
    Puntos = 0;
    Puntos_X = Medida * 18;
    Puntos_Y = Medida * 21;

    // Vidas
    Vidas = 3;
    Vidas_X= Marcador_X - (Medida * 62);
    Vidas_Y = Marcador_Altura - (Medida * 20);


    // Topes de Movimiento nave principal
    Nave_Y1 = Marcador_Altura + Enemigo_Medida4;
    Nave_Y2 = Pantalla_Alto - Enemigo_Medida4;
    Nave_Ascenso = Medida * 1.5; // Velocidad de elevacion nave principal

    // Animacion
    Frame = 0; //0-16-32
    Frame_c = 0;


    // Scroll
    Scroll_Fin = Tile_Medida16 * 2064;
    for (int i = 0; i < 129; i++) {
        Tile[i][0] = 0;
        Tile[i][1] = 0;
        Tile[i][2] = 0;
        Tile[i][3] = 0;
        Tile[i][4] = 0;
    }
    Tile_N = 0;
    Tile_Scroll = 0.0f;


    // Atlas para tileado
    num = 1;
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 16; x++) {
            Atlas[num][0] = x * 16;
            Atlas[num][1] = (y * 16) + 96;
            num += 1;
        }

    }
}


void Tileado_Dibuja(Texture2D i_sprites) {

    int i, y, valor;
    float x = Pantalla_X+( - Tile_Medida + Tile_Scroll )-Medida;
    int py;
    int uvx, uvy;
    int num;

    for (i = 0; i < 20; i++) {
        num = Tile_N + i;
        if (num > 129) num = num - 129;
        if (num < 1) num = num + 129;
        py = Tile_Monte; // Pantalla_Alto - (Tile_Medida * 5);// 500;// Monte_Altura;
        for (y = 0; y < 5; y++) {
            valor = Tile[num][y];

            if (valor != 0) {
                uvx = Atlas[valor][0];
                uvy = Atlas[valor][1];
                Imagen_Dibuja(i_sprites, x, py, Tile_Medida, Tile_Medida_Monte, uvx, uvy, 16, 16, 0, 0);
            }
            py += Tile_Medida_Monte;
        }
        x += Tile_Medida;
    }

}



void Enemigo_Dibuja(Texture2D i_sprites) {

    float x;

    Enemigo_X += 3; // El enemigo se mueve


    // Limites: x:0-2064  y:0-Pantalla_alto
    if (Enemigo_X > 2064.0f) Enemigo_X -= 2064.0f;
    if (Enemigo_X < 0.0f) Enemigo_X += 2064.0f;

    // Posicion Circular
    x = (Enemigo_X - (Scroll)) * Tile_Medida16;
    if (x < 0) x += Scroll_Fin;

    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida9, Enemigo_Medida8, 128 + Frame, 0, 9, 8, 1, 0); // Lander
    //    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida9, Enemigo_Medida8, 176 + Frame, 16, 9, 8, 1, 0); // Mutant
    //    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida11, Enemigo_Medida4, 224, 0, 11, 4, 1, 0); // Baiter
    //    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida6, Enemigo_Medida7, 128 + Frame, 16, 7, 7, 1, 0); // Bomber
    //    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida7, Enemigo_Medida7, 176 + Frame, 0, 7, 7, 1, 0); // Pod
    //    Imagen_Dibuja(i_sprites, x, Enemigo_Y, Enemigo_Medida5, Enemigo_Medida4, 240, 0, 5, 4, 1, 0); // Swarmer

}



void Humanoide_Dibuja(Texture2D i_sprites) {

    float x;

    // Limites: x:0-2064  y:0-Pantalla_alto
    if (Humanoide_X > 2064.0f) Enemigo_X -= 2064.0f;
    if (Humanoide_X < 0.0f) Enemigo_X += 2064.0f;

    // Posicion Circular
    x = (Humanoide_X - (Scroll)) * Tile_Medida16;
    if (x < 0) x += Scroll_Fin;

    Imagen_Dibuja(i_sprites, x, Humanoide_Y, Enemigo_Medida3, Enemigo_Medida8, 145, 241, 3, 8, 1, 0); // Humanoid
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


void Marcador_Dibuja(Texture2D i_sprites,Font fuente) {

    int x,y;
    int distante;
    int i;
   
    DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, BLACK); // Marcador Completo
    DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, DARKBLUE); // Borde Radar central
    DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, DARKBLUE); // Linea horizontal
    Imagen_Dibuja(i_sprites, Radar_X, Medida, Radar_Largo, Radar_Alto, Tile_N, 64, 128, 32, 0, 0); // Radar
    Imagen_Dibuja(i_sprites, Radar_CentroX, 0, Radar_CentroM, Marcador_Altura, 0, 29, 16, 35, 0, 0); // Radar blanco central

    // Iconos en Radar
    // -------------------------------------------------------------------------------------------------------------
    // 
    // Enemigos:
    distante = ((Enemigo_X) - Scroll);
    distante -= (Pantalla_MitadX / Tile_Medida16);
    if (distante > 1032)  distante -= 2064;
    if (distante < -1032) distante += 2064;
    x = Pantalla_MitadX + (distante / 1032.0f) * Radar_M;
    y = (Enemigo_Y / 33) * Medida;
    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 117, 61, 2, 2, 1, 0); // Enemigo en Radar ( Lander )
//    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 112+Frame, 57, 2, 2, 1, 0); // Enemigo en Radar ( Mutante )
//    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 126, 61, 2, 2, 1, 0); // Enemigo en Radar ( Baiter )
//    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 129, 61, 2, 2, 1, 0); // Enemigo en Radar ( Bomber )
//    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 17+Frame, 59, 2, 2, 1, 0); // Enemigo en Radar ( Pod )
//    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 132, 61, 2, 2, 1, 0); // Enemigo en Radar ( Swarmer )


    // Humanoides:
    distante = ((Humanoide_X)-Scroll);
    distante -= (Pantalla_MitadX / Tile_Medida16);
    if (distante > 1032)  distante -= 2064;
    if (distante < -1032) distante += 2064;
    x = Pantalla_MitadX + (distante / 1032.0f) * Radar_M;
    y =   (Humanoide_Y / 33)* Medida;
    Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 123, 61, 2, 2, 1, 0); // Humanoide en Radar


    // Nave Principal
    x = (((Pantalla_MitadX - (Nave_X + Nave_Desplazamiento)) / 6)/Tile_Medida16)*2;
    y = (Nave_Y / 33) * Medida;
    Imagen_Dibuja(i_sprites, Pantalla_MitadX-x, y, Medida4, Medida4, 113, 60, 3, 3, 1, 0); // Nave Principal en Radar

    // -------------------------------------------------------------------------------------------------------------



    // Puntuacion
    position.x = (Marcador_X - Puntos_Ancho)-Puntos_X;
    position.y = Puntos_Y;
    DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, WHITE);


    // Bombas
    for (i = 0; i < Bombas;i++) {
        y = Bombas_Y - (Enemigo_Medida4 * i);
        Imagen_Dibuja(i_sprites, Bombas_X, y, Enemigo_Medida7, Enemigo_Medida3, 121+Frame, 57, 7, 3, 1, 0); // Bombas
    }

    // Vidas
    for (i = 0; i < Vidas;i++) {
        x = Vidas_X + (i * Enemigo_Medida13);
        Imagen_Dibuja(i_sprites, x, Vidas_Y, Enemigo_Medida13, Enemigo_Medida4, 161 + Frame, 58, 13, 4, 1, 0); // Naves
    }

}


void Puntua(Font fuente,int pun) { // Se llama a esta funcion para que sume puntos y actualize el tamaño del texto
    Puntos += pun;
    if (Puntos == 0) {
        sprintf_s(Puntos_Texto, sizeof(Puntos_Texto), "%02d", Puntos); // cuando los puntos = 0, se ponen 00        
    } else {
        sprintf_s(Puntos_Texto, sizeof(Puntos_Texto), "%d", Puntos);
    }
    Puntos_Ancho = MeasureTextEx(fuente, Puntos_Texto, fuente.baseSize * Medida, Medida4).x;
}


void Carga() { // Carga de Tileado
    int i, r, valor, pos;
    pos = 0;
    for (r = 0; r < 5; r++) {
        for (i = 0; i < 130; i++) {
            valor = Tile[i][r];
            Tile[i][r] = LoadStorageValue(pos);
            pos += 1;
        }
    }
}


int LoadStorageValue(unsigned int position) // Carga archivo
{
    int value = 0;
    int dataSize = 0;
    unsigned char* fileData = LoadFileData(STORAGE_DATA_FILE, &dataSize);
    if (fileData != NULL)
    {
        if (dataSize < ((int)(position * 4))) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", STORAGE_DATA_FILE, position);
        else
        {
            int* dataPtr = (int*)fileData;
            value = dataPtr[position];
        }
        UnloadFileData(fileData);
        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", STORAGE_DATA_FILE, value);
    }
    return value;
}