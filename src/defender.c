#include <stdio.h>
#include <math.h>   // angulos joystick
#include <raylib.h> // incluye <stdbool.h>
//#include <raymob.h>
#include "paleta.h" // colores
#include "input_actions.h"

#pragma region DeclaracionesAdelantadas

// ----------------------------------------------------------------------------
// Dispositivo
// ----------------------------------------------------------------------------
static int Pantalla_Largo, Pantalla_Alto, Pantalla_MitadX, Pantalla_MitadY, 
           Pantalla_X, Pantalla_X2, Pantalla_Medida;
static int Pantalla_Completa = 0;
static int Joystick = 2; // 0=Teclado 1=Android  2-Windows
static int gamepad = 0;
#define MAX_TOUCH_POINTS 6 // Puntos para tocar (Android)

/*
static int Pantalla_Bomba = 0; // 0-1 activada
static int Pantalla_Bomba_Modo = 0; // 0= blanco/negro  1= negro/blanco
static int Pantalla_BombaC,Pantalla_BombaCF,Pantalla_BombaM; // contadores
static int Mundo_Destruido = 0;
static int Mundo_Aniquilado = 0;
static int Pantalla_BombaT;
*/


// ----------------------------------------------------------------------------
// Medidas
// ----------------------------------------------------------------------------
// Para tileado
static float Tile_Medida, Tile_Medida2, Tile_Medida3, Tile_Scroll, Tile_Medida16,
             Tile_Monte, Tile_Medida_Monte, Tile_Medida_Monte16, Tile_Monte_Fin; 
// Medidas para Naves y Enemigos, Marcadores, disparos, explosiones
static float Medida, Medida2, Medida3, Medida4;; 
static float Scroll, Scroll_Fin, Radar_M;
static int Marcador_X, Marcador_Y, Marcador_Altura, Marcador_Largo;
static int Radar_X, Radar_Largo, Radar_Alto, 
           Radar_Alto2, Radar_CentroX, Radar_CentroM;
static int IndiceX;
float Medidas[15];

//static int casillax, casillay;


// ------------------------------------------------------------------------------------------
// Nave Principal
// ------------------------------------------------------------------------------------------
static float Nave_X, Nave_Y, Nave_Medida, Nave_Medida2, Nave_Medida3, Nave_Medida4;
static int Nave_X1, Nave_X2, Nave_X3, Nave_X4; // Posiciones segun Orientacion nave: 0-1     0: 1-Izda min  2-Izda max  3-Dcha min 4-Dcha max
static int Nave_Y1, Nave_Y2; // Topes de pantalla
static float Nave_Velocidad, Nave_Velocidad_Max, Nave_Desplazamiento, Nave_Desplazamiento_Max, Nave_Direccion, Nave_Ascenso, Nave_Scroll;
static float Nave_Propulsion = 1.02f;
static int Nave_Hiper_Espacio;
static int Nave_Hiper_Distancia;
static int Nave_HiperD;
static int Nave_Explota;
static int Nave_Invulnerable = 0;

// ------------------------------------------------------------------------------------------
// Marcadores
// ------------------------------------------------------------------------------------------

#define STORAGE_RECORD_FILE ASSETS_PATH"record.dat" // Archivo para tabla de records
#define RECORD_NUM 8

typedef struct {
    char iniciales[4];
    unsigned int puntos;
} Record;

static int Puntos, Puntos_Extra;
static char Puntos_Texto[32]; // Buffer para el texto

static char Oleada_Texto[5];
static int Puntos_X, Puntos_Y;
static int Puntos_Ancho;
static int Bombas, Bombas_X, Bombas_Y;
static int Vidas, Vidas_X, Vidas_Y;
static int Color_Gradiente = 0;

#define NUM_INICIALES 3 // Iniciales de texto - Tabla de Records -
static char Iniciales_Nombre[NUM_INICIALES + 2] = "---"; // Resultado final
static int Iniciales_Seleccion = 0;             // Posición de la inicial actual (0-2)
static bool Iniciales_Terminado = false;
static int Letras[NUM_INICIALES] = { 0, 0, 0 }; // Letras seleccionadas (0=A ... 25=Z)

char* TR_Nombres[8] = { "DRJ","SAM","LED","PGD","CRB","MRS","SSR","TMH" };
int TR_Puntuacion[8] = { 21270,18315,15920,14285,12520,11035,8265,6010 };



// ------------------------------------------------------------------------------------------
// Enemigos
// ------------------------------------------------------------------------------------------
int Enemigos_N, Swarmer_Crea, Baiter_Tiempo, Baiter_Tiempo_Fin;
float Swarmer_CreaX, Swarmer_CreaY;
static int old_lander = 0, old_mutante = 0, old_baiter = 0, old_pod = 0, old_bombardero = 0, old_swarmer = 0, old_oleadaC = 0;

// *** CAMBIA SEGUN FASE ***
static float Enemigo_Datos[7][4]; // num - 0-Velocidad de disparo  1-Velocidad enemigo
static int Enemigo_Datos2[7][4];  // num - 0-cadencia empieza  1-cadencia fin  2-cadencia disparo

#define Enemigos_Maximos 50
typedef struct {
    float x, y; // posiciones x,y
    float largo, alto;
    float altura, altura2; //Lander: Cada lander tiene una altura distinta cuando se traslada por el escenario
    int ID, num, animado, estado;
    //   float direccionX, DireccionY;
    float velocidadX, velocidadY;
    int uvx, uvy, mx, my;
    int muvx, muvy, manim; // radar
    int alterna, alterna_c, alterna_fin; //0- sube  1-baja
    int puntos;

    int cadencia, cadencia_fin, cadencia_empieza, cadencia_de_disparo, cadencia_recarga;
    bool activo;
} Enemigo;
Enemigo enemigos[Enemigos_Maximos];


// Ejemplo para enemigos, su rango x es de 0 a 2064.

// No quitar estas medidas, establecerlas dentro de la estructura enemigo, sirven para mostrar su imagen y el sistema de colision para disparos, naves, ...
// : ejm .medida_largo=Medidas[11], .medida_alto=Medidas[4]  = Baiter


// ------------------------------------------------------------------------------------------
// Humanoides
// ------------------------------------------------------------------------------------------
int Salidas[6] = { 0 };
static int Humanoides_N, Humanos_old, Humanoide_Rellena = 0, Humanoide_RellenaC = 0;
#define Humanoides_Maximos 11
typedef struct {
    int largo, alto;
    int ID, caida;
    //Vector2 origin; // posiciones x,y
    float x, y;
    int estado; // 0-Descansando  1-Cayendo   2-Caminando
    int direccion; //0-izda 1-dcha
    int pasos; // pasos hasta llegar a otro tile(8)
    int contador, contador_max;
    bool activo;
} Humanoide;
Humanoide humanoides[Humanoides_Maximos];
static int Humanoide_Abducido, Enemigo_Abductor, Humanoide_ID, Enemigo_ID, Humanoides_R, Humanoides_Abduccion_X, Humanoides_Abduccion_Y, Humanoide_Enganchado;


// ------------------------------------------------------------------------------------------
// Disparos Laser Nave Jugador
// ------------------------------------------------------------------------------------------
#define Disparos_Maximos 10
typedef struct {
    int x0, x, y, frame, direccion;
    bool activo;
} Disparo;
Disparo disparo[Disparos_Maximos];


// ------------------------------------------------------------------------------------------
// Disparos Enemigos
// ------------------------------------------------------------------------------------------
#define Proyectiles_Maximos 40
typedef struct {
    float x, y, angulo, velocidad;
    int tiempo, tiempo_fin;
    int clase; //0-disparo  1-mina
    bool activo;
} Proyectil;
Proyectil proyectil[Proyectiles_Maximos];


// ------------------------------------------------------------------------------------------
// Juego
// ------------------------------------------------------------------------------------------
static int Menu = 0; //0-Juego  1-Oleada completada
static int Menu_Espera = 0, Menu_Espera_Fin;
static int Menu_Oleada; // Espera un tiempo para mostrar menu y despues genera siguiente oleada
static int Juego_E;
static int Juego_E_Fin = 100; // Nuevo Enemigo
static int Puntos_Activo, Puntos_HX, Puntos_HY, Puntos_C, Puntos_Q, Puntos_Flash;// Puntos al rescatar humanoide
static int Frame_c, Frame; // Animacion - velocidad contador,  frame 0,16,32 uvx
static int Oleada, OleadaC, Oleada_Total; // 1-5
int Ola[5][30] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },          // oleada 0 20
        { 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 5, 1, 1, 1, 1 , 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },          // oleada 1 24
        { 5, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 5, 1, 1, 1, 4 , 1, 1, 1, 1, 5, 1, 1, 0, 0, 0 },          // oleada 2 27
        { 5, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 5, 1, 1, 1, 4 , 1, 1, 1, 4, 5, 1, 1, 1, 5, 0 },          // oleada 3 29

};
int Ola_Fin[5] = { 20,24,27,29,29 };


#define MAX_TEXTOS 25
static char* textos[MAX_TEXTOS] = { "ATTACK WAVE  ",
                                    "COMPLETED",
                                    "BONUS X 100",
                                    "HALL OF FAME",
                                    "TODAYS",
                                    "GREATEST",
                                    "ALL TIME",
                                    "LANDER",
                                    "MUTANT",
                                    "BAITER",
                                    "BOMBER",
                                    "POD",
                                    "SWARMER",
                                    "SCANNER",
                                    "JOSE LORENZO & BORJA BORDAL",//"ELECTRONICS INC.",
                                    "PRESENTS",
                                    "GAME OVER",
                                    "PRESS TO PLAY",
                                    "PLAYER ONE",
                                    "YOU HAVE QUALIFIED FOR",
                                    "THE DEFENDER HALL OF FAME",
                                    "SELECT INITIALS WITH UP DOWN",
                                    "PRESS FIRE TO ENTER INITIAL",
                                    "A",
                                    "",
};
int Iniciales_Posicion[3];

int Iniciales_Medida; // Medida recuadro android iniciales de texto
int InicialesX;


// ------------------------------------------------------------------------------------------
// Estrellas de fondo
// ------------------------------------------------------------------------------------------
#define Estrellas_N    100   // Número de estrellas en pantalla
typedef struct {
    float x;
    int y;        // Posición en pantalla
    int indice;      // Índice actual del gradiente
    int retardo;     // Contador de frames para cambio de color
    int contador;    // Cuántos cambios de color ha hecho
    int maxCambios;  // Número máximo de cambios antes de reaparecer (1 a 3)
} Estrella;
Estrella estrellas[Estrellas_N];


// ------------------------------------------------------------------------------------------
// Explosion nave jugador
// ------------------------------------------------------------------------------------------
#define Particulas_Maximas 150   // Número de estrellas en pantalla
typedef struct {
    float x, y, angulo, velocidad;
    bool activo;
} Particula;
Particula particulas[Particulas_Maximas];


// ------------------------------------------------------------------------------------------
// Explosiones e Implosiones
// ------------------------------------------------------------------------------------------
#define Explosiones_Maximas 20
typedef struct {
    float x, y, escala;
    int uvx, uvy, largo, alto;
    int num, modo; //0-explosion  >1 implosion
    bool activo;
} Explosion;
Explosion explosiones[Explosiones_Maximas];


// ------------------------------------------------------------------------------------------
// Bomba Inteligente / Mundo Explota
// ------------------------------------------------------------------------------------------
static int Pantalla_Bomba, Pantalla_Bomba_Modo, Pantalla_BombaC, Pantalla_BombaCF, Pantalla_BombaM, Mundo_Destruido, Mundo_Aniquilado, Pantalla_BombaT, Pantalla_BombaE;


// ------------------------------------------------------------------------------------------
// Tileado
// ------------------------------------------------------------------------------------------
static int Tile_N;
int Tile[130][5];
int Suelo[130][6];
int Atlas[160][2];
#define STORAGE_LEVEL_FILE   ASSETS_PATH"nivel.dat" // Archivo para tileado


// ------------------------------------------------------------------------------------------
// Controles
// ------------------------------------------------------------------------------------------
int JRegion[7][4];
static int PalancaX, PalancaY, PalancaM, PalancaD, PalancaO, PalancaA, BolaM, BolaR, BolaX, BolaY;
int Palanca_Boton[7] = { 0 };


//------------------------------------------------------------------------------------
// Define funciones
//------------------------------------------------------------------------------------
void Inicia_Variables(void);
void Reinicia_Variables(void);
void Restaura_Variables(void);
void Imagen_Dibuja(Texture2D, int, int, int, int, int, int, int, int, int, int);
void Imagen_Dibuja_Color(Texture2D, int, int, int, int, int, int, int, int, int, int, Color);
void Marcador_Dibuja(Texture2D, Font);
void Tileado_Dibuja(Texture2D);

void Puntua(Font, int);
void Carga(void);
void Nave_Control(int, Sound, Sound, Sound, Sound, Font);

void Dibuja_Regiones(void);
void Region(int, int, Sound, Sound, Sound);
void Region_Rellena(int);
void Palanca_Dibuja(Texture2D, int, int, int, int);

int LoadStorageValue(unsigned int);
int Ruta(int, int, int);

void Humanoide_Crea(int, int);
int Humanoide_Borra(int, int, int, Sound, Sound, Sound, Sound, Sound);
void Humanoide_Dibuja(Texture2D, Font, Sound, Sound, Sound, Sound, Sound, Sound, Sound, Sound);
void Humanoide_Posiciona_Nivel(int);


void Enemigo_Crea(int, float, float);
void Enemigo_Dibuja(Texture2D, Sound, Sound, Sound, Sound, Sound, Sound, Sound, Sound);
int Enemigo_Borra(int, int, int, Sound, Sound, Sound, Sound, Sound, Sound, Font);

void Disparo_Crea(int, int);
void Disparo_Dibuja(Texture2D, Sound, Sound, Sound, Sound, Sound, Sound, Font);

void Explosion_Crea(float, float, int, int, Sound, Sound, Sound, Sound, Sound);
void Explosiones_Dibuja(Texture2D);

void Abduce(Sound, Sound, Sound);
void Puntos_Dibuja(Texture2D);

void Estrellas_Fondo_Dibuja(void);
void Mundo_Explota(Sound, Sound, Sound, Sound, Sound);
void Bomba_Inteligente(Font, Sound, Sound, Sound, Sound, Sound, Sound);
void Lander_Mutante(int);

void Hiper_Espacio_Dibuja(Texture2D);
void Proyectil_Crea(float, float, float, int, int, Sound, Sound);
void Proyectil_Dibuja(Texture2D, Sound, Sound, Sound, Sound, Sound);
void Swarmer_Crea_Enjambre(float, float);

void Menu_Dibuja(Font, Texture2D, Sound, Sound, Sound);

void Particulas_Dibuja(Font, Sound, Sound, Sound, Sound, Sound, Sound, Sound, Sound, Sound);
void Particulas_Explosion(float, float);
void Enemigos_Contabiliza(int, Sound, Sound, Sound, Sound, Sound);
float GetRandomValueFloat(float, float);
void Juego_Mas_Dificil(void);


//------------------------------------------------------------------------------------
// Variables para dibujo
//------------------------------------------------------------------------------------
static Vector2 position;
static double rotation;
static Vector2 origin;

static Vector2 touchPositions[MAX_TOUCH_POINTS] = { 0 };

#pragma endregion

//-----------------------------------------------------------------------------
//  P R I N C I P A L
//-----------------------------------------------------------------------------
int main(void)
{
    InitWindow(0, 0, "Defender");
    ToggleBorderlessWindowed();
    //ToggleFullscreen();
    ClearWindowState(FLAG_WINDOW_TOPMOST);
    SetTargetFPS(60);
    HideCursor();
    InitAudioDevice();

    // ASSETS_PATH es una macro definida en CMakeLists.txt para crear una ruta dinámica.
    Font fuente = LoadFont(ASSETS_PATH"i_fuente.png");
    Texture2D i_sprites = LoadTexture(ASSETS_PATH"i_sprites.png");
    Sound s_abduce = LoadSound(ASSETS_PATH"sounds/s_abduce.wav");
    Sound s_bomba = LoadSound(ASSETS_PATH"sounds/s_bomba.wav");
    Sound s_bombardero = LoadSound(ASSETS_PATH"sounds/s_bombardero.wav");
    Sound s_caida = LoadSound(ASSETS_PATH"sounds/s_caida.wav");
    Sound s_comenzar = LoadSound(ASSETS_PATH"sounds/s_comenzar.wav");
    Sound s_explosion = LoadSound(ASSETS_PATH"sounds/s_explosion.wav");
    Sound s_humanoide = LoadSound(ASSETS_PATH"sounds/s_humanoide.wav");
    Sound s_implosion = LoadSound(ASSETS_PATH"sounds/s_implosion.wav");
    Sound s_inicio = LoadSound(ASSETS_PATH"sounds/s_inicio.wav");
    Sound s_laser = LoadSound(ASSETS_PATH"sounds/s_laser.wav");
    Sound s_libera = LoadSound(ASSETS_PATH"sounds/s_libera.wav");
    Sound s_oleada = LoadSound(ASSETS_PATH"sounds/s_oleada.wav");
    Sound s_pod = LoadSound(ASSETS_PATH"sounds/s_pod.wav");
    Sound s_proyectil = LoadSound(ASSETS_PATH"sounds/s_proyectil.wav");
    Sound s_puntos = LoadSound(ASSETS_PATH"sounds/s_puntos.wav");

    Inicia_Variables();
    Reinicia_Variables();

    Carga();
    Puntua(fuente, 0); // Renueva datos puntuacion

    Menu = 2;// 2;
    //   Puntos = 18000; Menu = 3;
    Humanoide_Posiciona_Nivel(10);
    PlaySound(s_inicio);


    //--------------------------------------------------------------------------------------

    SetExitKey(SALIR);

    // Bucle Principal
    while (!WindowShouldClose())
    {
        // Animaciones
        //---------------------------------------------------------------------------------------------------------
        Frame_c += 1;
        if (Frame_c == 5) { // Velocidad de animacion
            Frame_c = 0;
            Frame += 16;
            if (Frame == 48) Frame = 0;
            Color_Gradiente += 1;
            if (Color_Gradiente == 8) Color_Gradiente = 0;
        }

        // Otro
        if (Menu == 0) 
        {
            if (Nave_Explota == 0) {

                // Enemigos
                //---------------------------------------------------------------------------------------------------------
                // Intenta Abducir Humanoides
                if (Humanoides_R > 0) { // Humanoides en estado de reposo
                    if (Humanoide_Abducido == 0) Abduce(s_implosion, s_explosion, s_laser);
                }

                // Genera oleadas
                if (OleadaC < 30) {
                    int oleN = Oleada_Total;
                    if (oleN > 4) oleN = 4;
                    Juego_E += 1;
                    if (Juego_E > Juego_E_Fin) {
                        Juego_E = 0;
                        for (int r = 0; r < 5; r++) {
                            int kmalo = Ola[(oleN - 1)][(OleadaC + r)];
                            if (kmalo > 0) {
                                if (kmalo == 1 && Mundo_Aniquilado == 1) kmalo = 2; // Mutante
                                Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), kmalo, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                            }
                        }
                        OleadaC += 5;
                    }

                }
                else { // No hay mas oleadas de enemigos
                    if (Enemigos_N < 1) {
                        Menu = 1; // Oleada Completada
                        Humanoide_Rellena = 0;
                        Humanoide_RellenaC = 0;
                        if (IsSoundPlaying(s_laser)) StopSound(s_laser);
                        if (IsSoundPlaying(s_bomba)) StopSound(s_bomba);
                        if (IsSoundPlaying(s_bombardero)) StopSound(s_bombardero);
                        if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
                        if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
                        if (IsSoundPlaying(s_caida)) StopSound(s_caida);
                        PlaySound(s_oleada);
                    }
                }

                if (Swarmer_Crea == 1) Swarmer_Crea_Enjambre(Swarmer_CreaX, Swarmer_CreaY);
                Baiter_Tiempo -= 1;
                if (Baiter_Tiempo < 1) {
                    Baiter_Tiempo = Baiter_Tiempo_Fin;
                    Explosion_Crea(GetRandomValue(1, 2063), GetRandomValue(Marcador_Y + Tile_Medida, Tile_Monte), 3, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                }

                // Control Nave
                //---------------------------------------------------------------------------------------------------------
                if (Nave_Hiper_Espacio == 0) {

                    Nave_Control(Joystick, s_laser, s_puntos, s_bomba, s_implosion, fuente);

                    // Bomba / Mundo Explota
                    //---------------------------------------------------------------------------------------------------------
                    if (Pantalla_Bomba == 1) {
                        Pantalla_BombaC += 1;
                        if (Pantalla_BombaC > (Pantalla_BombaT / 2)) {
                            if (Pantalla_BombaE == 0) { // Bomba Inteligente
                                Bomba_Inteligente(fuente, s_explosion, s_implosion, s_laser, s_caida, s_bombardero, s_pod);
                            }
                            else {
                                if (Mundo_Aniquilado == 0) {
                                    Mundo_Destruido = 0; Mundo_Aniquilado = 1; // Ya no hay Montañas
                                    for (int i = 0; i < Enemigos_Maximos; i++) {
                                        if (enemigos[i].num == 1) Lander_Mutante(i);
                                    }
                                }
                            }
                        }
                        if (Pantalla_BombaC > Pantalla_BombaT) Pantalla_Bomba = 0; // Termina Bomba
                        Pantalla_BombaCF += 1;
                        if (Pantalla_BombaCF > 3) {
                            Pantalla_BombaCF = 0;
                            Pantalla_Bomba_Modo = 1 - Pantalla_Bomba_Modo;
                            if (Pantalla_BombaE == 1)  Mundo_Explota(s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                        }
                    }

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

                }
            }
        }
        //---------------------------------------------------------------------------------------------------------


        //casillax = (Tile_N + ((raton.x - Tile_Scroll) / Tile_Medida)) + IndiceX;
        //if (casillax > 129) casillax -= 129;
        //casillay = (raton.y - Tile_Monte) / Tile_Medida_Monte;
        /*
        if (IsKeyPressed(KEY_E)) {
            //            Enemigo_Crea(2, Scroll, GetRandomValue(Marcador_Y + Tile_Medida2, Pantalla_Alto - Tile_Medida2));
            Enemigo_Crea(5, GetRandomValue(1, 2000), Pantalla_MitadY);
        }*/

        // Captura de pantalla
        //---------------------------------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_S)) {
            TakeScreenshot("captura.png");
        }

        if (IsKeyPressed(KEY_E)) {
            Nave_Explota = 1;
            Nave_Velocidad = 0;
            Humanos_old = Humanoides_N;
            PlaySound(s_explosion);
        }

        //---------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------
        //      D I B U J A
        //---------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------

        BeginDrawing();



        if (Menu == 0) {


            if (Pantalla_Bomba == 0) {
                ClearBackground(BLACK);
            }
            else {
                if (Pantalla_Bomba_Modo == 0) {
                    if (Pantalla_BombaE == 0) {
                        ClearBackground(WHITE);
                    }
                    else {
                        ClearBackground(Gradiente[Color_Gradiente]);
                    }
                }
                else {
                    ClearBackground(BLACK);
                }
            }


            // Dibuja Estrellas
            Estrellas_Fondo_Dibuja();

            // Dibuja Montañas
            if (Mundo_Aniquilado == 0) Tileado_Dibuja(i_sprites);//


            // Dibuja Enemigos
            Enemigo_Dibuja(i_sprites, s_abduce, s_proyectil, s_implosion, s_explosion, s_laser, s_bombardero, s_humanoide, s_pod);


            // Dibuja Nave
            if (Nave_Hiper_Espacio == 0) {
                if (Nave_Explota > 0) {
                    if (Nave_Explota < 40) {
                        Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 272 + Frame, 16, 15, 6, 1, Nave_Direccion);
                    }
                }
                else {

                    Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y, Nave_Medida, Nave_Medida2, 64 + Frame, 0, 15, 6, 1, Nave_Direccion);

                }

                if (Humanoide_Enganchado == 1) {
                    Imagen_Dibuja(i_sprites, Nave_X + Nave_Desplazamiento, Nave_Y + Tile_Medida2, Medidas[3], Medidas[8], 209, 241, 3, 8, 1, 0);
                }

            }
            else {
                Hiper_Espacio_Dibuja(i_sprites);
            }



            // Dibuja Puntos (cuando rescatas a humanoide)
            if (Puntos_Activo == 1) Puntos_Dibuja(i_sprites);



            // Dibuja Humanoides
            Humanoide_Dibuja(i_sprites, fuente, s_implosion, s_explosion, s_laser, s_caida, s_libera, s_bombardero, s_pod, s_puntos); // Estructura de humanoides

            // Dibuja Disparos Laser Nave Jugador
            Disparo_Dibuja(i_sprites, s_implosion, s_explosion, s_laser, s_caida, s_bombardero, s_pod, fuente); // Estructura de disparos

            // Dibuja Disparos Enemigos
            Proyectil_Dibuja(i_sprites, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);

            // Dibuja Explosiones e Implosiones
            Explosiones_Dibuja(i_sprites);




            // Dibuja Explosion nave jugador
            if (Nave_Explota > 0)  Particulas_Dibuja(fuente, s_laser, s_implosion, s_explosion, s_bomba, s_caida, s_bombardero, s_inicio, s_puntos, s_pod);


            // Marcadores
            Marcador_Dibuja(i_sprites, fuente);



        }
        else {
            Menu_Dibuja(fuente, i_sprites, s_inicio, s_comenzar, s_humanoide);

            //  DrawText(TextFormat("letras %i", Letras[Iniciales_Seleccion]), Pantalla_X, Marcador_Y + (Tile_Medida), 30, BLUE);

        }

        // Mascara
        if (Pantalla_Completa == 0) {
            DrawRectangle(0, 0, Pantalla_X, Pantalla_Alto, BLACK);
            DrawRectangle(Pantalla_X2, 0, Pantalla_X, Pantalla_Alto, BLACK);
        }
        if ((Palanca_Boton[0] == 1) && Menu == 0) Palanca_Dibuja(i_sprites, PalancaX, PalancaY, BolaX, BolaY);

        //Dibuja_Regiones();

        //     DrawText(TextFormat("Juego_E %i", Juego_E), Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);
        //     DrawText(TextFormat("Juego_E_Fin %i", Juego_E_Fin), Pantalla_X, Marcador_Y + Tile_Medida, 30, BLUE);


        //  DrawText(TextFormat("Menu_Espera_Fin %i",Menu_Espera_Fin), Pantalla_X, Marcador_Y + Tile_Medida, 30, BLUE);
        // DrawText(TextFormat("Menu_Espera %i", Menu_Espera , Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);
        //    DrawText(TextFormat("InicialesX %i", InicialesX), Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);


        //     DrawText(TextFormat("Nave_Explota %i", Nave_Explota), Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);

        /*
        DrawText(TextFormat("old_lander %i", old_lander), Pantalla_X, Marcador_Y + (Tile_Medida2 ), 30, BLUE);
        DrawText(TextFormat("old_mutante %i", old_mutante), Pantalla_X, Marcador_Y + (Tile_Medida2 * 2), 30, BLUE);
        DrawText(TextFormat("old_baiter %i", old_baiter), Pantalla_X, Marcador_Y + (Tile_Medida2 * 3), 30, BLUE);
        DrawText(TextFormat("old_bombardero %i", old_bombardero), Pantalla_X, Marcador_Y + (Tile_Medida2 * 4), 30, BLUE);
        DrawText(TextFormat("old_pod %i", old_pod), Pantalla_X, Marcador_Y + (Tile_Medida2 * 5), 30, BLUE);
        DrawText(TextFormat("old_swarmer %i", old_swarmer), Pantalla_X, Marcador_Y + (Tile_Medida2 * 6), 30, BLUE);
        */
        //        Dibuja_Regiones();
        //      DrawText(TextFormat("casillax %i", casillax), Pantalla_X, Marcador_Y+Tile_Medida2, 30, BLUE);
        //      DrawText(TextFormat("casillay %i", casillay), Pantalla_X, Marcador_Y+Tile_Medida, 30, GREEN);

        //    DrawText(TextFormat("PalancaD %i", PalancaD), Pantalla_X, Marcador_Y+Tile_Medida2, 30, BLUE);
        //    DrawText(TextFormat("Angle: %.2f radians", PalancaA), Pantalla_X2, Marcador_Y+Tile_Medida2, 30, GREEN);
        //        DrawText(TextFormat("PalancaO %i", PalancaO), 30, 230, 30, BLUE);
        /*
        DrawText(TextFormat("Enemigos_N %i", Enemigos_N), Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);
        DrawText(TextFormat("Humanoides_N %i", Humanoides_N), Pantalla_X, Marcador_Y + Tile_Medida, 30, BLUE);
        DrawText(TextFormat("Humanoides_R %i", Humanoides_R), Pantalla_X, Marcador_Y + Tile_Medida * 2, 30, BLUE);

        DrawText(TextFormat("Enemigo_Abductor %i", Enemigo_Abductor), Pantalla_X + (Tile_Medida * 4), Marcador_Y + Tile_Medida2, 30, GREEN);
        DrawText(TextFormat("Humanoide_Abducido %i", Humanoide_Abducido), Pantalla_X + (Tile_Medida * 4), Marcador_Y + Tile_Medida, 30, GREEN);
        DrawText(TextFormat("Humanoide_Enganchado %i", Humanoide_Enganchado), Pantalla_X + (Tile_Medida * 4), Marcador_Y + Tile_Medida+Tile_Medida2, 30, PINK);
        */

        // Limites para nave
        /*
        DrawLine(Nave_X1, 0, Nave_X1, Pantalla_Alto, RED);
        DrawLine(Nave_X2, 0, Nave_X2, Pantalla_Alto, RED);
        DrawLine(Nave_X3, 0, Nave_X3, Pantalla_Alto, BLUE);
        DrawLine(Nave_X4, 0, Nave_X4, Pantalla_Alto, RED);
        */





        //   DrawLine(Pantalla_MitadX, 0, Pantalla_MitadX, Pantalla_Alto, GREEN);
        //   DrawText(TextFormat("Oleada %i", Oleada), Pantalla_X, Marcador_Y + Tile_Medida2, 30, BLUE);
        //   DrawText(TextFormat("Oleada_Total %i", Oleada_Total), Pantalla_X, Marcador_Y + Tile_Medida, 30, BLUE);


        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Inicialización
    //--------------------------------------------------------------------------------------
    UnloadFont(fuente);
    UnloadTexture(i_sprites);
    UnloadSound(s_laser);
    UnloadSound(s_implosion);
    UnloadSound(s_explosion);
    UnloadSound(s_caida);
    UnloadSound(s_abduce);
    UnloadSound(s_puntos);
    UnloadSound(s_libera);
    UnloadSound(s_bomba);
    UnloadSound(s_proyectil);
    UnloadSound(s_bombardero);
    UnloadSound(s_comenzar);
    UnloadSound(s_inicio);
    UnloadSound(s_oleada);
    UnloadSound(s_humanoide);
    UnloadSound(s_pod);
    CloseWindow();
    //-------------------------------------------------------------------------

    return 0;
}


#pragma region DefinicionFunciones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// D E F I N I C I O N E S
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Inicia_Variables() 
{
    int valor, num;

    Pantalla_Largo = GetScreenWidth();
    Pantalla_Alto = GetScreenHeight();
    Pantalla_MitadX = Pantalla_Largo / 2;
    Pantalla_MitadY = Pantalla_Alto / 2;

    Medida = Pantalla_Alto / 240.0f; // Medida de un pixel segun resolucion
    Medida2 = Medida * 2.0f;
    Medida3 = Medida * 0.3f;
    Medida4 = Medida * 3;

    for (int i = 1; i < 15; i++) {
        Medidas[i] = Medida * i;
    }

    Tile_Medida_Monte = Medida * 16; // tile cuadrado, Tile_Medida_Monte = Tile_Medida (sustituir); Aqui se encoge altura
    Tile_Monte = Pantalla_Alto - (Medida * 90);
    Tile_Medida_Monte16 = Tile_Medida_Monte / 16.0f;
    Tile_Monte_Fin = Tile_Monte + (Tile_Medida_Monte * 5);

    // Pantalla completa sin margenes a los lados
    if (Pantalla_Completa == 0) {
        Pantalla_Medida = (Pantalla_Alto * 320) / 240;
        Pantalla_X = (Pantalla_Largo - Pantalla_Medida) / 2;
        Pantalla_X2 = Pantalla_Largo - Pantalla_X;
        Tile_Medida = (Pantalla_Medida / 18.0f);
        IndiceX = -(Pantalla_X / Tile_Medida) + 1; // Calculo para saber nº tile x humanoide
    }
    else {
        Pantalla_Medida = Pantalla_Largo;
        Pantalla_X = 0;
        Pantalla_X2 = Pantalla_Largo;
        Tile_Medida = (Pantalla_Largo / 18.0f);
        IndiceX = 1; // Calculo para saber nº tile x humanoide

    }

    Tile_Medida2 = Tile_Medida / 2.0f;
    Tile_Medida3 = Tile_Medida2 + (Tile_Medida2 / 2.0f);
    Tile_Medida16 = Tile_Medida / 16.0f;
    Nave_Medida = Medida * 15.0f; //15x6 pixeles
    Nave_Medida2 = Medida * 6.0f;
    Nave_Medida3 = Nave_Medida / 2.0f; // Mitad de largo de la nave
    Nave_Medida4 = Nave_Medida2 / 2.0f; // Mitad de altura de la nave

    Nave_X1 = Pantalla_X + (Medida * 67.0f);//60
    Nave_X2 = Pantalla_X + (Medida * 120.0f);//100
    Nave_X3 = Pantalla_X2 - (Medida * 81);//60
    Nave_X4 = Pantalla_X2 - (Medida * 133.0f);//100


    Nave_Velocidad_Max = Medida * 1.4f; // 1.5f; //1.6f;
    Nave_Desplazamiento_Max = (Nave_X2 - Nave_X1);
    Nave_Hiper_Distancia = Tile_Medida;

    // Marcadores
    Marcador_Altura = 35 * Medida;
    Marcador_Largo = 145 * Medida;
    Marcador_X = Pantalla_MitadX - (Marcador_Largo / 2);
    Puntos_X = Medida * 18;
    Puntos_Y = Medida * 21;
    Menu_Espera_Fin = 600;

    // Radar
    Radar_X = Marcador_X + (Medida * 2);
    Radar_Largo = Marcador_Largo - (Medida * 4);
    Radar_Alto = Medida * 32.0f;
    Marcador_Y = Radar_Alto + Medida;
    Radar_CentroM = 16 * Medida;
    Radar_CentroX = Pantalla_MitadX - (Radar_CentroM / 2);
    Radar_M = (140.0f * Medida) / 2.0f;


    // Bombas
    Bombas_X = Marcador_X - (Medida * 7);
    Bombas_Y = Marcador_Altura - (Medida * 6);



    // Vidas
    Vidas_X = Marcador_X - (Medida * 62);
    Vidas_Y = Marcador_Altura - (Medida * 20);


    // Topes de Movimiento nave principal
    Nave_Y1 = Marcador_Altura + Medidas[4];
    Nave_Y2 = Pantalla_Alto - Medidas[4];
    Nave_Ascenso = Medida * 1.7; // Velocidad de elevacion nave principal

    // Scroll
    Scroll_Fin = Tile_Medida16 * 2064;
    for (int i = 0; i < 129; i++) {
        Tile[i][0] = 0;
        Tile[i][1] = 0;
        Tile[i][2] = 0;
        Tile[i][3] = 0;
        Tile[i][4] = 0;
    }

    // Atlas para tileado
    num = 1;
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 16; x++) {
            Atlas[num][0] = x * 16;
            Atlas[num][1] = (y * 16) + 96;
            num += 1;
        }
    }


    // Joystick
    valor = Pantalla_Alto / 8;
    JRegion[1][0] = Pantalla_MitadX - (valor / 2); JRegion[1][1] = 0; JRegion[1][2] = Pantalla_MitadX + (valor / 2); JRegion[1][3] = valor;
    JRegion[0][0] = 0; JRegion[0][1] = 0; JRegion[0][2] = JRegion[1][0]; JRegion[0][3] = Pantalla_Alto;
    valor = (Pantalla_Largo - JRegion[1][2]) / 2;
    JRegion[2][0] = JRegion[1][2]; JRegion[2][1] = 0; JRegion[2][2] = JRegion[1][2] + valor; JRegion[2][3] = Pantalla_MitadY;
    JRegion[3][0] = JRegion[2][2]; JRegion[3][1] = 0; JRegion[3][2] = Pantalla_Largo; JRegion[3][3] = Pantalla_MitadY;
    JRegion[4][0] = JRegion[1][2]; JRegion[4][1] = JRegion[3][3]; JRegion[4][2] = JRegion[1][2] + valor; JRegion[4][3] = Pantalla_Alto;
    JRegion[5][0] = JRegion[2][2]; JRegion[5][1] = JRegion[3][3]; JRegion[5][2] = Pantalla_Largo; JRegion[5][3] = Pantalla_Alto;

    valor = Pantalla_Alto / 5;
    JRegion[6][0] = 0; JRegion[6][1] = 0; JRegion[6][2] = JRegion[1][0]; JRegion[6][3] = valor;
    JRegion[0][1] = JRegion[6][3];
    // JRegion[0][3] -= JRegion[6][3];

    PalancaM = Pantalla_Alto / 3; // medida
    BolaM = PalancaM / 3;
    BolaR = BolaM / 1.5;
    PalancaO = BolaR / 4;

    // Posicion iniciales texto
    Iniciales_Posicion[0] = Pantalla_MitadX - (Medida * 20);
    Iniciales_Posicion[1] = Pantalla_MitadX - (Medida * 3);
    Iniciales_Posicion[2] = Pantalla_MitadX + (Medida * 15);


    Iniciales_Medida = (Pantalla_Largo - (Pantalla_X * 2)) / 13; // Medida recuadro android iniciales de texto
    InicialesX = 0;
}


// Restaura valores despues de eliminar a una ola
void Restaura_Variables() {

    // Posicion inicial nave
    Nave_X = Nave_X1;
    Nave_Y = Pantalla_MitadY;
    Nave_Direccion = 0;
    Nave_Hiper_Espacio = 0;
    Nave_Scroll = 0.0f;
    Nave_Velocidad = 0.0f;
    Nave_Desplazamiento = 0.0f;
    Nave_Explota = 0;

    // Sistema de abduccion
    Humanoide_ID = 1;
    Humanoide_Abducido = 0;
    Humanoide_Enganchado = 0;
    Enemigo_ID = 1;
    Enemigo_Abductor = 0;

    // Scroll
    Tile_N = 0;
    Tile_Scroll = 0.0f;
    Scroll = 0.0f;

    // Efecto Bomba
    Pantalla_Bomba = 0;
    Pantalla_Bomba_Modo = 0;
    Pantalla_BombaC = 0;
    Pantalla_BombaCF = 0;
    Pantalla_BombaM = 0;
    Pantalla_BombaT = 0;
    Pantalla_BombaE = 0;

    // Inicializar estrellas
    for (int i = 0; i < Estrellas_N; i++) {
        estrellas[i].x = GetRandomValue(1.0f, 2063.0f);
        estrellas[i].y = GetRandomValue(Marcador_Y + Tile_Medida2, Tile_Monte + Tile_Medida2);
        estrellas[i].indice = GetRandomValue(0, 7);
        estrellas[i].retardo = GetRandomValue(20, 60);
        estrellas[i].contador = 0;
        estrellas[i].maxCambios = GetRandomValue(1, 3);
    }

    // Juego
    Puntos_Activo = 0; // 500 puntos cuando rescatas a un humanoide
    Puntos_HX = 0, Puntos_HY = 0, Puntos_C = 0, Puntos_Q = 0, Puntos_Flash = 0;
    Juego_E = 0;
    Juego_E_Fin = 300; // Retardo para crear enemigos en nuevas oleadas
    Menu_Oleada = 0; // Bloquea pantalla un tiempo hasta crear siguiente oleada



    // Joystick
    PalancaA = 0; // angulo

    // Inicializa enemigos
    for (int i = 0; i < Enemigos_Maximos; i++) {
        enemigos[i].activo = false;
    }
    Swarmer_Crea = 0; Swarmer_CreaX = 0.0f; Swarmer_CreaY = 0.0f;
    OleadaC = 0; // contador indice
    Baiter_Tiempo = Baiter_Tiempo_Fin;

    // Inicializa disparos laser nave jugador
    for (int i = 0; i < Disparos_Maximos; i++) {
        disparo[i].activo = false;
    }

    // Inicializa disparos enemigos
    for (int i = 0; i < Proyectiles_Maximos; i++) {
        proyectil[i].activo = false;
    }

    // Inicializa explosiones
    for (int i = 0; i < Explosiones_Maximas; i++) {
        explosiones[i].activo = false;
    }

    // Inicializa humanoides
    for (int i = 0; i < Humanoides_Maximos; i++) {
        humanoides[i].activo = false;
    }


}


// Cada vez que el juego comienza:
void Reinicia_Variables() 
{
    Restaura_Variables();

    // Juego Nuevo
    Puntos = 0;
    Puntos_Extra = 0; // cada 10000 puntos, vida y bomba extra
    Vidas = 2;
    Bombas = 3;
    Mundo_Destruido = 0;
    Mundo_Aniquilado = 0;
    Oleada_Total = 1; //1-infinito
    Oleada = 1; // 1-5 5=1
    Baiter_Tiempo_Fin = 3000;
    Baiter_Tiempo = Baiter_Tiempo_Fin;

    // Animacion
    Frame = 0; //0-16-32
    Frame_c = 0;

    Iniciales_Nombre[5];//= ""; // Resultado final
    Iniciales_Seleccion = 0;             // Posición de la inicial actual (0-2)
    Iniciales_Terminado = false;

    // Lander
    Enemigo_Datos[1][0] = Tile_Medida16 * 1.2; //Velocidad de disparo  Lander
    Enemigo_Datos[1][1] = Tile_Medida16 * 0.2f;// Velocidad de enemigo
    Enemigo_Datos[1][2] = Tile_Medida16 * 0.4f;// Velocidad de ascenso/descenso
    Enemigo_Datos2[1][0] = 280; // Cadencia empieza
    Enemigo_Datos2[1][1] = 300; // Cadencia fin
    Enemigo_Datos2[1][2] = 30; // Cadencia de disparo

    // Mutante
    Enemigo_Datos[2][0] = Tile_Medida16 * 2.0f; //Velocidad de disparo  Mutante
    Enemigo_Datos[2][1] = Tile_Medida16 * 0.6f;// Velocidad de enemigo
    Enemigo_Datos[2][2] = Tile_Medida16;// Velocidad de ascenso/descenso
    Enemigo_Datos2[2][0] = 90; // Cadencia empieza
    Enemigo_Datos2[2][1] = 100; // Cadencia fin
    Enemigo_Datos2[2][2] = 8; // Cadencia de disparo

    // Baiter
    Enemigo_Datos[3][0] = Tile_Medida16 * 2.5f; //Velocidad de disparo  Baiter
    Enemigo_Datos[3][1] = Tile_Medida16 * 0.6f;// Velocidad de enemigo
    Enemigo_Datos[3][2] = Tile_Medida16 * 0.6f;// Velocidad de ascenso/descenso
    Enemigo_Datos2[3][0] = 250; // Cadencia empieza
    Enemigo_Datos2[3][1] = 300; // Cadencia fin
    Enemigo_Datos2[3][2] = 20; // Cadencia de disparo

    // Bombardero
    Enemigo_Datos[4][0] = 0.0f; //Velocidad de disparo  Bombardero
    Enemigo_Datos[4][1] = Tile_Medida16 * 0.45f;// Velocidad de enemigo
    Enemigo_Datos[4][2] = Tile_Medida16 * 0.3f;// Velocidad de ascenso/descenso
    Enemigo_Datos2[4][0] = 10; // Cadencia empieza
    Enemigo_Datos2[4][1] = 15; // Cadencia fin
    Enemigo_Datos2[4][2] = 6; // Cadencia de disparo



    // Pod
    Enemigo_Datos[5][0] = Tile_Medida16 * 1.5f; //Velocidad de disparo  Pod
    Enemigo_Datos[5][1] = Tile_Medida16 * 0.15f;// Velocidad de enemigo
    Enemigo_Datos[5][2] = Tile_Medida16 * 0.15f;// Velocidad de ascenso/descenso
    Enemigo_Datos2[5][0] = 200; // Cadencia empieza
    Enemigo_Datos2[5][1] = 250; // Cadencia fin
    Enemigo_Datos2[5][2] = 10; // Cadencia de disparo

    // Swarmer
    Enemigo_Datos[6][0] = Tile_Medida16 * 2.0f; //Velocidad de disparo  Swarm
    Enemigo_Datos[6][1] = Tile_Medida16 * 0.2f;// Velocidad de enemigo (no usado)
    Enemigo_Datos[6][2] = Tile_Medida16 * 0.4f;// Velocidad de ascenso/descenso (no usado)
    Enemigo_Datos2[6][0] = 180; // Cadencia empieza
    Enemigo_Datos2[6][1] = 260; // Cadencia fin
    Enemigo_Datos2[6][2] = 40; // Cadencia de disparo


}


void Tileado_Dibuja(Texture2D i_sprites) 
{
    int i, y, valor;
    int x = Pantalla_X + (-Tile_Medida + Tile_Scroll) - Medida;
    int py;
    int uvx, uvy;
    int num;

    for (i = 0; i < 20; i++) {
        num = Tile_N + i;
        if (num > 129) num = num - 129;
        if (num < 1) num = num + 129;
        py = Tile_Monte; // Pantalla_Alto - (Tile_Medida * 5);// 500;// Monte_Altura;
        for (y = 0; y < 5; y++) {

            //     valor = Suelo[num][y];
            //     if (valor == 1) Imagen_Dibuja(i_sprites, x, py, Tile_Medida, Tile_Medida_Monte, 144, 240, 16, 16, 0, 0);

            valor = Tile[num][y];
            if (valor != 0) {
                uvx = Atlas[valor][0];
                uvy = Atlas[valor][1];
                Imagen_Dibuja(i_sprites, x, py, Tile_Medida, Tile_Medida_Monte, uvx, uvy, 16, 16, 0, 0);
            }


            /*
            if (y == 4) {
                DrawText(TextFormat("%i", num), x + Tile_Medida2, py + Tile_Medida2, 30, GREEN);
                DrawText(TextFormat("%i", Suelo[num][5]), x + Tile_Medida2, py + Tile_Medida, 30, YELLOW);
                DrawLine(x - Tile_Medida2, Suelo[num][5], x + Tile_Medida2, Suelo[num][5], RED);
            }
            */

            py += Tile_Medida_Monte;
        }



        x += Tile_Medida;
    }

}


void Imagen_Dibuja(Texture2D i_sprites, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto, int centrado, int espejo) {

    Color pinta = WHITE;

    if (centrado == 1) {
        origin.x = largo / 2.0f;
        origin.y = alto / 2.0f;
    }
    else {
        origin.x = 0;
        origin.y = 0;
    }

    if (Pantalla_Bomba == 1) {
        if (Pantalla_Bomba_Modo == 0) {
            pinta = BLACK;
        }
        else {
            if (Mundo_Destruido == 0) {
                pinta = WHITE;
            }
            else {
                pinta = Gradiente[Color_Gradiente];
            }
        }
        uvx += 256;
    }

    if (espejo == 1) uvy += 16;
    Rectangle sourceRec = { uvx + 0.01, uvy + 0.01, uvlargo - 0.02, uvalto - 0.02 };
    Rectangle destRec = { x, y, largo, alto };
    DrawTexturePro(i_sprites, sourceRec, destRec, origin, 0, pinta);
}


void Imagen_Dibuja_Color(Texture2D i_sprites, int x, int y, int largo, int alto, int uvx, int uvy, int uvlargo, int uvalto, int centrado, int espejo, Color pinta) {

    if (centrado == 1) {
        origin.x = largo / 2.0f;
        origin.y = alto / 2.0f;
    }
    else {
        origin.x = 0;
        origin.y = 0;
    }

    if (Pantalla_Bomba == 1) {
        if (Pantalla_Bomba_Modo == 0) {
            pinta = BLACK;
        }
        else {

            if (Mundo_Destruido == 0) {
                pinta = WHITE;
            }
            else {
                pinta = Gradiente[Color_Gradiente];
            }

        }
    }
    else {
        pinta = Gradiente[Color_Gradiente];
    }
    uvx += 256;

    if (espejo == 1) uvy += 16;
    Rectangle sourceRec = { uvx + 0.01, uvy + 0.01, uvlargo - 0.02, uvalto - 0.02 };
    Rectangle destRec = { x, y, largo, alto };
    DrawTexturePro(i_sprites, sourceRec, destRec, origin, 0, pinta);
}


void Marcador_Dibuja(Texture2D i_sprites, Font fuente) {

    int x, y;
    int distante;
    int i, anim, valor;

    if (Pantalla_Bomba == 0) {
        DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, BLACK); // Marcador Completo
        DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, DARKBLUE); // Borde Radar central
        DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, DARKBLUE); // Linea horizontal
    }
    else {
        if (Pantalla_Bomba_Modo == 0) {
            if (Pantalla_BombaE == 0) {
                DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, WHITE); // Marcador Completo
                DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, BLACK); // Borde Radar central
                DrawRectangle(Marcador_X + (Medida2), Medida2, Marcador_Largo - (Medida2 * 2), Marcador_Altura, WHITE); // Borde Radar central
                DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, BLACK); // Linea horizontal
                if (Mundo_Aniquilado == 0) DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, BLACK);
            }
            else {
                DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, Gradiente[Color_Gradiente]); // Marcador Completo
                DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, BLACK); // Borde Radar central
                DrawRectangle(Marcador_X + (Medida2), Medida2, Marcador_Largo - (Medida2 * 2), Marcador_Altura, Gradiente[Color_Gradiente]); // Borde Radar central
                DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, BLACK); // Linea horizontal
                if (Mundo_Aniquilado == 0) {
                    if (Mundo_Destruido == 1) {
                        DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, BLACK);
                    }
                    else {
                        DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, Gradiente[Color_Gradiente]);
                    }
                }

            }
        }
        else {

            if (Pantalla_BombaE == 0) {
                DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, BLACK); // Marcador Completo
                DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, WHITE); // Borde Radar central
                DrawRectangle(Marcador_X + (Medida2), Medida2, Marcador_Largo - (Medida2 * 2), Marcador_Altura, BLACK); // Borde Radar central
                DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, WHITE); // Linea horizontal
                if (Mundo_Aniquilado == 0) DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, WHITE);
            }
            else {
                DrawRectangle(0, 0, Pantalla_Largo, Marcador_Altura, Gradiente[Color_Gradiente]); // Marcador Completo
                DrawRectangle(Marcador_X, 0, Marcador_Largo, Marcador_Altura, Gradiente[Color_Gradiente]); // Borde Radar central
                DrawRectangle(Marcador_X + (Medida2), Medida2, Marcador_Largo - (Medida2 * 2), Marcador_Altura, BLACK); // Borde Radar central
                DrawRectangle(0, Marcador_Y, Pantalla_Largo, Medida2, Gradiente[Color_Gradiente]); // Linea horizontal
                if (Mundo_Aniquilado == 0) {
                    if (Mundo_Destruido == 1) {
                        DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, Gradiente[Color_Gradiente]);
                    }
                    else {
                        DrawRectangle(0, Tile_Monte_Fin, Pantalla_Largo, Tile_Medida, BLACK);
                    }
                }

            }
        }
    }

    Imagen_Dibuja(i_sprites, Radar_X, Medida, Radar_Largo, Radar_Alto, Tile_N, 64, 128, 32, 0, 0); // Radar
    Imagen_Dibuja(i_sprites, Radar_CentroX, 0, Radar_CentroM, Marcador_Altura, 0, 29, 16, 35, 0, 0); // Radar blanco central

    // Iconos en Radar
    // -------------------------------------------------------------------------------------------------------------
    //
    // Enemigos:
    for (int j = 0; j < Enemigos_Maximos; j++) {
        if (enemigos[j].activo) {
            distante = ((enemigos[j].x) - Scroll);
            distante -= (Pantalla_MitadX / Tile_Medida16);
            if (distante > 1032)  distante -= 2064;
            if (distante < -1032) distante += 2064;
            x = Pantalla_MitadX + (distante / 1032.0f) * Radar_M;
            y = (((enemigos[j].y * 32) / Pantalla_Alto) * Medida) + Medida;
            anim = 0;
            if (enemigos[j].manim == 1) anim = Frame;
            Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, enemigos[j].muvx + anim, enemigos[j].muvy, 2, 2, 1, 0); // Enemigo en Radar
        }
    }



    // Humanoides:
    for (int j = 0; j < Humanoides_Maximos; j++) {
        if (humanoides[j].activo) {
            distante = ((humanoides[j].x) - Scroll);
            distante -= (Pantalla_MitadX / Tile_Medida16);
            if (distante > 1032)  distante -= 2064;
            if (distante < -1032) distante += 2064;
            x = Pantalla_MitadX + (distante / 1032.0f) * Radar_M;
            y = (((humanoides[j].y * 32) / Pantalla_Alto) * Medida) + Medida;
            Imagen_Dibuja(i_sprites, x, y, Medida2, Medida2, 123, 61, 2, 2, 1, 0); // Humanoide en Radar
        }
    }




    // Nave Principal
    x = (((Pantalla_MitadX - (Nave_X + Nave_Desplazamiento)) / 6) / Tile_Medida16) * 2;
    y = (((Nave_Y * 32) / Pantalla_Alto) * Medida) + Medida;
    Imagen_Dibuja(i_sprites, Pantalla_MitadX - x, y, Medida4, Medida4, 113, 60, 3, 3, 1, 0); // Nave Principal en Radar

    // -------------------------------------------------------------------------------------------------------------



    // Puntuacion

    if (OleadaC < 30) {
        Puntos_Flash += 1;
        if (Puntos_Flash > 15) Puntos_Flash = 0;
    }
    else { Puntos_Flash = 0; }

    if (Puntos_Flash < 10) {
        position.x = (Marcador_X - Puntos_Ancho) - Puntos_X;
        position.y = Puntos_Y;
        if (Pantalla_Bomba == 0) {
            DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]);
        }
        else {
            if (Pantalla_Bomba_Modo == 0) {
                DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, BLACK);
            }
            else {
                if (Mundo_Destruido == 0) {
                    DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, WHITE);
                }
                else {
                    DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]);
                }
            }
        }
    }


    // Bombas
    for (i = 0; i < Bombas; i++) {
        y = Bombas_Y - (Medidas[4] * i);
        Imagen_Dibuja(i_sprites, Bombas_X, y, Medidas[7], Medidas[3], 121 + Frame, 57, 6, 3, 1, 0); // Bombas
    }

    // Vidas
    valor = Vidas;
    if (valor > 4) valor = 4;
    for (i = 0; i < valor; i++) {
        x = Vidas_X + (i * Medidas[13]);
        Imagen_Dibuja(i_sprites, x, Vidas_Y, Medidas[13], Medidas[4], 161 + Frame, 58, 13, 4, 1, 0); // Naves
    }

}


void Puntua(Font fuente, int pun) { // Se llama a esta funcion para que sume puntos y actualize el tamaño del texto
    Puntos_Extra += pun;
    Puntos += pun;
    if (Puntos == 0) {
        //  sprintf_s(Puntos_Texto, sizeof(Puntos_Texto), "%02d", Puntos); // cuando los puntos = 0, se ponen 00
        snprintf(Puntos_Texto, sizeof(Puntos_Texto), "%02d", Puntos); // cuando los puntos = 0, se ponen 00
    }
    else {
        //      sprintf_s(Puntos_Texto, sizeof(Puntos_Texto), "%d", Puntos);
        snprintf(Puntos_Texto, sizeof(Puntos_Texto), "%d", Puntos);
    }
    Puntos_Ancho = MeasureTextEx(fuente, Puntos_Texto, fuente.baseSize * Medida, Medida4).x;

    // Vida y Bomba extra
    if (Puntos_Extra > 9999) {
        Puntos_Extra -= 9999;
        Bombas += 1;
        Vidas += 1;
    }
}


void Carga() { // Carga de Tileado
    int i, r, valor, pos;
    pos = 0;
    int altura = 0;

    for (r = 0; r < 5; r++) {
        for (i = 0; i < 130; i++) {
            valor = Tile[i][r];
            Tile[i][r] = LoadStorageValue(pos);
            pos += 1;
            valor = Suelo[i][r];
            Suelo[i][r] = LoadStorageValue(pos);
            pos += 1;
        }
    }

    // Posicion Y Montaña para Lander
    for (int x = 0; x < 130; x++) {
        altura = 4;
        Suelo[x][5] = Pantalla_Alto - (Tile_Medida_Monte * 2);
        for (int y = 0; y < 5; y++) {
            if (Suelo[x][y] == 1) {
                if (y < altura) {
                    altura = y;
                    Suelo[x][5] = Tile_Monte + (Tile_Medida_Monte * y);
                }
            }
        }

        Suelo[x][5] -= (Tile_Medida_Monte * 2);
    }

}


int LoadStorageValue(unsigned int position) // Carga archivo
{
    int value = 0;
    int dataSize = 0;
    unsigned char* fileData = LoadFileData(STORAGE_LEVEL_FILE, &dataSize);
    if (fileData != NULL)
    {
        if (dataSize < ((int)(position * 4))) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", STORAGE_LEVEL_FILE, position);
        else
        {
            int* dataPtr = (int*)fileData;
            value = dataPtr[position];
        }
        UnloadFileData(fileData);
        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", STORAGE_LEVEL_FILE, value);
    }
    return value;
}


void Nave_Control(int num, Sound s_laser, Sound s_puntos, Sound s_bomba, Sound s_implosion, Font fuente) 
{
    int arriba = 0;
    int abajo = 0;
    int boton1 = 0;
    bool izq = false;
    bool der = false;
    int boton2 = 0;
    int boton3 = 0;
    int boton4 = 0;
    int cambia = 0;
    float valor2 = 0.0f;
    int tCount = 0;
    int cx, cy;

    // Obtiene eventos

    switch (num) 
    {
    case 1: // Joystick Esto parece tactil
        tCount = GetTouchPointCount();
        if (tCount > MAX_TOUCH_POINTS) tCount = MAX_TOUCH_POINTS;
        for (int i = 0; i < tCount; ++i) touchPositions[i] = GetTouchPosition(i);

        if (tCount > 0) {
            for (int i = 0; i < tCount; ++i)
            {
                if ((touchPositions[i].x > 0) && (touchPositions[i].y > 0))
                {
                    Region(touchPositions[i].x, touchPositions[i].y, s_laser, s_implosion, s_bomba);
                }
            }
        }
        for (int i = 0; i < 7; i++) { // Desactiva botones
            if (Palanca_Boton[i] == 1) {
                int posible = 1;
                for (int b = 0; b < tCount; b++) {
                    if ((touchPositions[b].x > JRegion[i][0]) && (touchPositions[b].x < JRegion[i][2])) {
                        if ((touchPositions[b].y > JRegion[i][1]) && (touchPositions[b].y < JRegion[i][3])) {
                            posible = 0;
                        }
                    }
                }
                if (posible == 1) Palanca_Boton[i] = 0;
            }
            //   DrawText(TextFormat(" %i", Palanca_Boton[i]), Radar_X, Marcador_Y + (40 * i), 30, GREEN);
        }
        if (Palanca_Boton[0] == 0) PalancaD = 0;

        if (PalancaD == 1 || PalancaD == 2 || PalancaD == 8) arriba = 1;
        if (PalancaD == 4 || PalancaD == 5 || PalancaD == 6) abajo = 1;

        // if (PalancaD == 3 || PalancaD == 7) boton1 = 1;
         /*
         if (PalancaD == 3 || PalancaD == 7) boton1 = 1;

         if (Nave_Direccion == 0) {
             if (PalancaD > 5) cambia = 1;
         }
         else {
             if (PalancaD > 0 && PalancaD < 5) cambia = 1;
         }
         */

        break;

    case 2: // Joystick Windows ?? Linux ??
        if (IsGamepadAvailable(gamepad))
        {
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP)) arriba = 1;
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) abajo = 1;

            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) 
              boton1 = 1; // X  - Acelerar
            if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) 
              cambia = 1; // [] - Cambia Direccion
            if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) 
              boton2 = 1; // O - Dispara
            if ((IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) && Bombas > 0) boton3 = 1; // ^ - Bomba Inteligente

            if ((IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) ||
                (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)))
                boton4 = 1; // ^ - Hiper Espacio
        }

        break;

    default: // Teclado
        if (IsKeyDown(SUBIR))
            arriba = 1;
        if (IsKeyDown(BAJAR))
            abajo = 1;
        if (IsKeyDown(DERECHA))
        {
            boton1 = 1;
            der = true;
        }
        if (IsKeyDown(IZQUIERDA))
        {
            boton1 = 1;
            izq = true;
        }

        if (IsKeyPressed(ORIENTACION)) cambia = 1;

        if (IsKeyPressed(DISPARAR))
            boton2 = 1;

        if (IsKeyPressed(BOMBA) && Bombas > 0)// Bomba
            boton3 = 1;

        if (IsKeyPressed(HIPERESPACIO))
            boton4 = 1;

        break;
    }

    // Actualiza estado

    if (arriba == 1) {
        Nave_Y -= Nave_Ascenso;
        if (Nave_Y < Nave_Y1) Nave_Y = Nave_Y1;
    }

    if (abajo == 1) {
        Nave_Y += Nave_Ascenso;
        if (Nave_Y > Nave_Y2) Nave_Y = Nave_Y2;
    }

    if ((der && Nave_Direccion == 0) || (izq && Nave_Direccion == 1) )
    {
        if (der && Nave_Direccion == 0)
        {
            if (Nave_Velocidad < Medida3)  Nave_Velocidad += Medida3;
            Nave_Desplazamiento += Medida3;
        }
        else if (izq && Nave_Direccion == 1)
        {
            if (Nave_Velocidad > -Medida3)  Nave_Velocidad -= Medida3;
            Nave_Desplazamiento -= Medida3;
        }
        Nave_Velocidad *= Nave_Propulsion;
        Nave_Desplazamiento *= 1.03;
        if (Nave_Desplazamiento < -Nave_Desplazamiento_Max) Nave_Desplazamiento = -Nave_Desplazamiento_Max;
        if (Nave_Desplazamiento > Nave_Desplazamiento_Max) Nave_Desplazamiento = Nave_Desplazamiento_Max;
    }

    if (boton2 == 1) {
        Disparo_Crea(Nave_X + Nave_Desplazamiento, Nave_Y);
        if (IsSoundPlaying(s_laser)) StopSound(s_laser); PlaySound(s_laser);
    }

    if (boton3 == 1) {
        if (Pantalla_Bomba == 0) {
            PlaySound(s_bomba);
            Pantalla_Bomba = 1; Pantalla_BombaC = 0; Pantalla_BombaCF = 0; Pantalla_BombaM = 0; Pantalla_BombaT = 12; Pantalla_BombaE = 0; Bombas -= 1;
        }
    }

    if (boton4 == 1) {
        Nave_Hiper_Espacio = 1;
        Nave_Hiper_Distancia = 0;
        Nave_HiperD = 0;
        if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
        PlaySound(s_implosion);
    }

    if (cambia == 1) {
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
    Nave_Velocidad *= 0.99f;
    if (Nave_Velocidad > Nave_Velocidad_Max) Nave_Velocidad = Nave_Velocidad_Max;
    if (Nave_Velocidad < -Nave_Velocidad_Max) Nave_Velocidad = -Nave_Velocidad_Max;

    valor2 = ((Nave_X + Nave_Desplazamiento) / Tile_Medida) * 16;
    //    valor2 = (((Nave_X + Nave_Desplazamiento) - Pantalla_X) / Tile_Medida) * 16;
    Nave_Scroll = Scroll + valor2;
    if (Nave_Scroll > 2064.0f) Nave_Scroll -= 2064.0f;


    // Libera Humanoide
    if (Humanoide_Enganchado == 1)
        if (Nave_Y > Tile_Monte) {
            cx = (Nave_Scroll / 16.0f) + IndiceX;
            cy = ((Nave_Y + Tile_Medida2) - Tile_Monte) / Tile_Medida_Monte;
            if (cx < 1) cx += 129;
            if (cx > 129) cx -= 129;
            //DrawText(TextFormat(" %i", cx), Nave_X, Nave_Y + Tile_Medida, 30, WHITE);


            if (cy > -1 && cy < 5) {
                if (Suelo[cx][cy] == 1) {
                    Puntua(fuente, 500);
                    Humanoide_Crea(cx - IndiceX, cy);
                    Puntos_Q = 0; // 500
                    Puntos_HX = Nave_Scroll;
                    Puntos_HY = Nave_Y;
                    Puntos_Activo = 1;
                    Puntos_C = 0;
                    Humanoide_Enganchado = 0;
                    Humanoide_Abducido = 0;
                    Enemigo_Abductor = 0;
                    PlaySound(s_puntos);
                }


            }
        }
}


void Dibuja_Regiones() {

    Color cola;

    for (int i = 0; i < 7; i++) {
        cola = RED;
        if (Palanca_Boton[i] == 1) cola = GREEN;
        DrawLine(JRegion[i][0], JRegion[i][1], JRegion[i][2], JRegion[i][1], cola);
        DrawLine(JRegion[i][0], JRegion[i][3], JRegion[i][2], JRegion[i][3], cola);
        DrawLine(JRegion[i][0], JRegion[i][1], JRegion[i][0], JRegion[i][3], cola);
        DrawLine(JRegion[i][2], JRegion[i][1], JRegion[i][2], JRegion[i][3], cola);
    }
}


void Region(int x, int y, Sound s_laser, Sound s_implosion, Sound s_bomba) {

    for (int i = 0; i < 7; i++) {
        if (x > JRegion[i][0] && x < JRegion[i][2]) {
            if (y > JRegion[i][1] && y < JRegion[i][3]) {
                if (i == 0) { // palanca
                    if (Palanca_Boton[0] == 0) {
                        PalancaX = x;
                        PalancaY = y;
                    }
                    BolaX = x;
                    BolaY = y;
                    Palanca_Boton[0] = 1;
                }
                else { // botones
                    if (i == 3) Palanca_Boton[i] = 0;



                    if (Palanca_Boton[i] == 0) {
                        Palanca_Boton[i] = 1;
                        switch (i) {
                        case 1: // 
                            // sale a menu

                            break;
                        case 2:
                            // bomba
                            if (Pantalla_Bomba == 0) {
                                PlaySound(s_bomba);
                                Pantalla_Bomba = 1; Pantalla_BombaC = 0; Pantalla_BombaCF = 0; Pantalla_BombaM = 0; Pantalla_BombaT = 12; Pantalla_BombaE = 0; Bombas -= 1;
                               // if (Joystick == 1) VibrateEx(0.2f, 1.0f);
                            }

                            break;
                        case 3: // cambia direccion

                            if (Nave_Direccion == 0) {
                                if (Nave_Velocidad < Medida3)  Nave_Velocidad += Medida3;
                                Nave_Desplazamiento += Medida3;
                            }
                            else {
                                if (Nave_Velocidad > -Medida3)  Nave_Velocidad -= Medida3;
                                Nave_Desplazamiento -= Medida3;
                            }
                            Nave_Velocidad *= Nave_Propulsion;
                            Nave_Desplazamiento *= 1.03;
                            if (Nave_Desplazamiento < -Nave_Desplazamiento_Max) Nave_Desplazamiento = -Nave_Desplazamiento_Max;
                            if (Nave_Desplazamiento > Nave_Desplazamiento_Max) Nave_Desplazamiento = Nave_Desplazamiento_Max;

                            break;
                        case 4:
                            if (Nave_Direccion == 0) {
                                Nave_Direccion = 1; // izquierda
                            }
                            else {
                                Nave_Direccion = 0; // derecha
                            }
                            Nave_X += Nave_Desplazamiento * 2;
                            Nave_Desplazamiento *= -1;

                            break;
                        case 5:
                            Disparo_Crea(Nave_X + Nave_Desplazamiento, Nave_Y);
                            if (IsSoundPlaying(s_laser)) StopSound(s_laser); PlaySound(s_laser);
                            //if (Joystick == 1) VibrateEx(0.01f, 0.8f);
                            break;
                        case 6:
                            Nave_Hiper_Espacio = 1;
                            Nave_Hiper_Distancia = 0;
                            Nave_HiperD = 0;
                            if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
                            PlaySound(s_implosion);
                            //if (Joystick == 1) VibrateEx(0.5f, 0.3f);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}


void Region_Rellena(int num) {

    int x, y, largo, alto;

    x = JRegion[num][0];
    y = JRegion[num][1];
    largo = (JRegion[num][2] - JRegion[num][0]);
    alto = (JRegion[num][3] - JRegion[num][1]);
    DrawRectangle(x, y, largo, alto, GREEN);
}


void Palanca_Dibuja(Texture2D i_sprites, int x, int y, int x2, int y2) {

    int PalancaR;

    Imagen_Dibuja(i_sprites, x, y, PalancaM, PalancaM, 16, 0, 48, 48, 1, 0);
    // DrawLine(x - (PalancaM), y, x + (PalancaM), y, YELLOW);
    // DrawLine(x, y - (PalancaM), x, y + (PalancaM), YELLOW);

    if (Palanca_Boton[0] == 1) {
        //   DrawLine(x, y, x2, y2, RED);

        float dx = x2 - x;
        float dy = y2 - y;
        PalancaR = sqrtf(dx * dx + dy * dy);

        if (PalancaR > PalancaO) {

            float anguloRad = atan2(dy, dx);
            PalancaA = anguloRad * (180.0f / PI);
            if (PalancaA < 0) PalancaA += 360.0f;

            if ((PalancaA >= 337.5f && PalancaA < 360.0f) || (PalancaA >= 0 && PalancaA < 22.5f))
                PalancaD = 3;
            else if (PalancaA >= 22.5f && PalancaA < 67.5f)
                PalancaD = 4;
            else if (PalancaA >= 67.5f && PalancaA < 112.5f)
                PalancaD = 5;
            else if (PalancaA >= 112.5f && PalancaA < 157.5f)
                PalancaD = 6;
            else if (PalancaA >= 157.5f && PalancaA < 202.5f)
                PalancaD = 7;
            else if (PalancaA >= 202.5f && PalancaA < 247.5f)
                PalancaD = 8;
            else if (PalancaA >= 247.5f && PalancaA < 292.5f)
                PalancaD = 1;
            else if (PalancaA >= 292.5f && PalancaA < 337.5f)
                PalancaD = 2;

            if (PalancaR < BolaR) {
                BolaX = x2;
                BolaY = y2;
            }
            else {
                BolaX = x + cosf(anguloRad) * BolaR;
                BolaY = y + sinf(anguloRad) * BolaR;
            }
            Imagen_Dibuja(i_sprites, BolaX, BolaY, BolaM, BolaM, 272, 0, 16, 16, 1, 0);
        }
        else {
            Imagen_Dibuja(i_sprites, x2, y2, BolaM, BolaM, 272, 0, 16, 16, 1, 0);
        }
    }

}


//----------------------------------------------------------
// Crea humanoide
//----------------------------------------------------------
void Humanoide_Crea(int cx, int cy) {

    float x, y, dif;

    dif = (cx - IndiceX);
    if (dif < 1) cx += 129;
    if (dif > 129) cx -= 129;
    //    x = ((cx - IndiceX) * 16) + 8;
    x = ((cx) * 16) + 8;
    y = Tile_Monte + (cy * Tile_Medida_Monte) + Tile_Medida_Monte / 2;

    for (int i = 0; i < Humanoides_Maximos; i++) {
        if (!humanoides[i].activo) {
            humanoides[i].largo = Medidas[3];
            humanoides[i].ID = Humanoide_ID;
            Humanoide_ID += 1;
            humanoides[i].alto = Medidas[8];
            humanoides[i].x = x;
            humanoides[i].y = y;
            humanoides[i].estado = 0;
            humanoides[i].direccion = 0;
            humanoides[i].pasos = 0;
            humanoides[i].caida = 0;
            humanoides[i].contador = 0;
            humanoides[i].contador_max = GetRandomValue(100, 300);
            humanoides[i].activo = true;
            break;
        }
    }
}


//----------------------------------------------------------
// Borra humanoide con disparo nave jugador
//----------------------------------------------------------
int Humanoide_Borra(int d1, int dy, int dd, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_pod) {

    float d2 = d1 + Tile_Medida3;
    float c1, c2, a1, a2;
    float x;
    int azar;
    if (dd = 0) {
        d2 = d1 - Tile_Medida3;
    }
    else {
        d2 = d1;
        d1 += Tile_Medida3;
    }

    for (int i = 0; i < Humanoides_Maximos; i++) {
        if (humanoides[i].activo) {

            // Posicion Circular
            x = (humanoides[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            if (x > Pantalla_X && x < Pantalla_X2) {

                a1 = humanoides[i].y - humanoides[i].alto / 2;
                a2 = humanoides[i].y + humanoides[i].alto / 2;

                if (dy > a1 && dy < a2) {

                    c1 = x - humanoides[i].largo / 2;
                    c2 = x + humanoides[i].largo / 2;

                    //                    bool colision = (d1 < c2) && (d2 > c1);
                    bool colision = (d1 > c1) && (d2 < c2);

                    if (colision) {
                        if (IsSoundPlaying(s_laser)) StopSound(s_laser);
                        if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
                        PlaySound(s_explosion);

                        if (humanoides[i].ID == Humanoide_Abducido) {
                            Humanoide_Abducido = 0;
                            // establece lander a estado normal
                            for (int e = 0; e < Enemigos_Maximos; e++) {
                                if (enemigos[e].ID == Enemigo_Abductor) {
                                    enemigos[e].estado = 0;
                                    azar = GetRandomValue(0, 1);
                                    if (azar == 0) {
                                        enemigos[e].velocidadX = Tile_Medida16 * 0.2f;
                                    }
                                    else {
                                        enemigos[e].velocidadX = -Tile_Medida16 * 0.2f;
                                    }
                                    Enemigo_Abductor = 0;
                                }
                            }
                        }
                        humanoides[i].activo = false;
                        Explosion_Crea(humanoides[i].x, humanoides[i].y, 0, 0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);

                        Humanoides_N -= 1;
                        if (Humanoides_N == 0) { // Explota Mundo
                            Pantalla_Bomba = 1; Pantalla_BombaC = 0;  Pantalla_BombaCF = 0; Pantalla_BombaM = 0;
                            Mundo_Destruido = 1; Pantalla_BombaT = 70; Pantalla_BombaE = 1;
                        }

                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


//----------------------------------------------------------
// Dibuja humanoides
//----------------------------------------------------------
void Humanoide_Dibuja(Texture2D i_sprites, Font fuente, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_caida, Sound s_libera, Sound s_bombardero, Sound s_pod, Sound s_puntos) {

    float x;
    int valor, cx, cy, mas;
    int celdax, celday; // posicion humanoide dentro del tile
    int hy1, ny1, ny2, nx1, nx2; // colision con nave principal
    Humanoides_N = 0; // Numero de humanoides
    Humanoides_R = 0; // Humanoides en reposo ( Cuando son abducidos)

    for (int i = 0; i < Humanoides_Maximos; i++) {
        if (humanoides[i].activo == true) {
            Humanoides_N += 1;
            // Actualiza
            //------------------------------------------------------------------------

            if (humanoides[i].x > 2064.0f) humanoides[i].x -= 2064.0f;
            if (humanoides[i].x < 0.0f) humanoides[i].x += 2064.0f;

            // Posicion Circular
            x = (humanoides[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            // casilla
            cx = (humanoides[i].x / 16.0f) + IndiceX;
            cy = (humanoides[i].y - Tile_Monte) / Tile_Medida_Monte;
            if (cx < 1) cx += 129;
            if (cx > 129) cx -= 129;


            // IA
            if (Nave_Explota == 0) {

                switch (humanoides[i].estado) {

                case 1: // cayendo
                    humanoides[i].y += Tile_Medida_Monte16;
                    humanoides[i].caida += Tile_Medida16;
                    if (humanoides[i].y > Tile_Monte) {

                        if (cy > 4) cy = 4;
                        if (cy > -1 && cy < 5) {
                            if (Suelo[cx][cy] == 1 || cy == 4) {

                                if (humanoides[i].caida > (Tile_Medida * 3)) { // Muere en la caida
                                    humanoides[i].activo = false;
                                    Explosion_Crea(humanoides[i].x, humanoides[i].y, 0, 0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                                    if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
                                    PlaySound(s_explosion);

                                }
                                else { // Sobreeevivire, eee
                                    Puntua(fuente, 250);
                                    humanoides[i].estado = 0;
                                    PlaySound(s_puntos);
                                    Puntos_Q = 1; // 500
                                    Puntos_HX = humanoides[i].x + 10;
                                    Puntos_HY = humanoides[i].y;
                                    Puntos_Activo = 1;
                                    Puntos_C = 0;
                                }

                                if (IsSoundPlaying(s_caida)) StopSound(s_caida);

                                Humanoide_Abducido = 0;
                                Enemigo_Abductor = 0;

                            }

                        }
                    }

                    // Se engancha a la nave principal
                    hy1 = (humanoides[i].y - Medidas[4]);
                    ny1 = Nave_Y - Nave_Medida4;
                    ny2 = Nave_Y + Nave_Medida4;
                    nx1 = (Nave_X + Nave_Desplazamiento) - Nave_Medida3;
                    nx2 = (Nave_X + Nave_Desplazamiento) + Nave_Medida3;

                    bool colision = (hy1 > ny1 && hy1 < ny2 && x>nx1 && x < nx2);
                    if (colision) {
                        humanoides[i].activo = false;
                        Humanoide_Abducido = 0;
                        Enemigo_Abductor = 0;
                        Humanoide_Enganchado = 1;
                        if (IsSoundPlaying(s_caida)) StopSound(s_caida);
                        PlaySound(s_libera);
                        Puntos_Q = 0;
                        Puntos_HX = humanoides[i].x;
                        Puntos_HY = humanoides[i].y;
                        Puntua(fuente, 500);
                        Puntos_Activo = 1;
                        Puntos_C = 0;
                    }
                    break;


                case 2: // caminando
                    humanoides[i].contador += 1;
                    if (humanoides[i].contador > 10) {
                        humanoides[i].contador = 0;
                        switch (humanoides[i].direccion) {

                        case 2:
                            humanoides[i].x += 1;
                            humanoides[i].y -= Tile_Medida_Monte16;
                            break;
                        case 3:
                            humanoides[i].x += 1;
                            break;
                        case 4:
                            humanoides[i].x += 1;
                            humanoides[i].y += Tile_Medida_Monte16;
                            break;
                        case 6:
                            humanoides[i].x -= 1;
                            humanoides[i].y += Tile_Medida_Monte16;
                            break;
                        case 7:
                            humanoides[i].x -= 1;
                            break;
                        case 8:
                            humanoides[i].x -= 1;
                            humanoides[i].y -= Tile_Medida_Monte16;
                            break;
                        default:
                            break;
                        }
                        humanoides[i].pasos += 1;
                        if (humanoides[i].pasos > 15) {
                            humanoides[i].pasos = 0;
                            valor = Ruta(cx, cy, humanoides[i].direccion);
                            humanoides[i].direccion = valor;
                            mas = GetRandomValue(0, 4);
                            //if (mas == 1) valor = 0;
                            //if (valor == 0) {
                            if (mas > 0) {
                                humanoides[i].estado = 0;
                                humanoides[i].contador_max = GetRandomValue(100, 400);
                            }
                        }
                    }
                    break;

                case 3: // abducido, ascendiendo con enemigo
//                    humanoides[i].y -= Tile_Medida16 * 0.4f;
                    humanoides[i].y -= Enemigo_Datos[1][2]; // Lander

                    
                    break;

                case 10: // Preparado para ser abducido
                    break;

                default: // reposo
                    humanoides[i].contador += 1;
                    if (humanoides[i].contador > humanoides[i].contador_max) {
                        humanoides[i].contador = 0;
                        valor = Ruta(cx, cy, humanoides[i].direccion);
                        humanoides[i].direccion = valor;
                        humanoides[i].estado = 2; // camina
                    }
                    break;

                }
            }

            mas = 0;
            celdax = 0;
            celday = 0;
            // celdax = Humanoide_Posicion[humanoides[i].celda][0];
            // celday = Humanoide_Posicion[humanoides[i].celda][1];

            if (humanoides[i].estado == 0) Humanoides_R += 1; // humanoides en reposo, se pueden abducir
            if (humanoides[i].estado == 2) mas = Frame; // animacion caminar

            if (humanoides[i].direccion > 5) {
                Imagen_Dibuja(i_sprites, x + celdax, humanoides[i].y + celday, Medidas[3], Medidas[8], 209 + mas, 241, 3, 8, 1, 0); // Humanoide
            }
            else {
                Imagen_Dibuja(i_sprites, x + celdax, humanoides[i].y + celday, Medidas[3], Medidas[8], 161 + mas, 241, 3, 8, 1, 0); // Humanoide
            }

            //      DrawText(TextFormat("e %i", humanoides[i].estado), x, humanoides[i].y - Tile_Medida, 30, YELLOW);
            //      DrawText(TextFormat("id %i", humanoides[i].ID), x, humanoides[i].y - Tile_Medida2, 30, GREEN);
            //                DrawText(TextFormat("%i", cx), x, humanoides[i].y - Tile_Medida2, 30, GREEN);
        }
    }
}


int Ruta(int x, int y, int dir) {

    int nlis = 0, salid = 0, azar = 0;

    for (int i = 0; i < 6; i++) {
        Salidas[i] = 0;
    }

    if (x == 1) {
        Salidas[nlis] = 3; nlis += 1;
        //if (dir== 7) Salidas[nlis] = 3; nlis += 1;
    }
    else {
        if (x == 129) {
            Salidas[nlis] = 8; nlis += 1;
            //Salidas[nlis] = 3; nlis += 1;
        }
        else {
            if (y > 0) {
                if (Suelo[x + 1][y - 1] == 1) { Salidas[nlis] = 2; nlis += 1; };
                if (Suelo[x - 1][y - 1] == 1) { Salidas[nlis] = 8; nlis += 1; };
            }
            if (y < 4) {
                if (Suelo[x + 1][y + 1] == 1) { Salidas[nlis] = 4; nlis += 1; };
                if (Suelo[x - 1][y + 1] == 1) { Salidas[nlis] = 6; nlis += 1; };
            }
            if (Suelo[x - 1][y] == 1) { Salidas[nlis] = 7; nlis += 1; };
            if (Suelo[x + 1][y] == 1) { Salidas[nlis] = 3; nlis += 1; };

        }
    }

    nlis -= 1;

    azar = GetRandomValue(0, nlis);
    salid = Salidas[azar];
    return salid;

}


//----------------------------------------------------------
// Crea Disparo Laser nave principal
//----------------------------------------------------------
void Disparo_Crea(int x, int y) {


    for (int i = 0; i < Disparos_Maximos; i++) {
        if (!disparo[i].activo) {
            disparo[i].y = y + Tile_Medida16;
            disparo[i].frame = 3; // uvy
            disparo[i].direccion = Nave_Direccion;
            if (Nave_Direccion == 0) {
                disparo[i].x = x + Medidas[2];
            }
            else {
                disparo[i].x = x - Medidas[8];
            }
            disparo[i].x0 = disparo[i].x;
            disparo[i].activo = true;
            break;
        }
    }
}


//----------------------------------------------------------
// Dibuja Disparos
//----------------------------------------------------------
void Disparo_Dibuja(Texture2D i_sprites, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_caida, Sound s_bombardero, Sound s_pod, Font fuente) {

    int posx = 0;
    int cuantos = 0;
    int frame = 3;
    int azar = 0;
    int borra;

    for (int i = 0; i < Disparos_Maximos; i++) {
        if (disparo[i].activo) {

            // Actualiza
            //------------------------------------------------------------------------

            if (disparo[i].direccion == 0) { // derecha

                disparo[i].x += Tile_Medida3;// *1.3;
                if (disparo[i].x > (Pantalla_Largo + Tile_Medida)) disparo[i].activo = false;

                cuantos = ((disparo[i].x - disparo[i].x0) / Medidas[8]) + 1;
                posx = disparo[i].x;
                for (int d = 1; d < cuantos; d++) {
                    Imagen_Dibuja_Color(i_sprites, posx, disparo[i].y, Medidas[8], Medida, 241, frame, 8, 1, 0, 0, Gradiente[Color_Gradiente]); // estela
                    posx -= Medidas[8];
                    azar = GetRandomValue(0, 2);
                    frame += (azar * 2);
                    if (frame > 31) {
                        frame = 33;
                        if ((d + 10) < cuantos) {
                            azar = GetRandomValue(0, 4);
                            if (azar == 100) {
                                azar = GetRandomValue(0, 5);
                                frame = 23 + (azar * 2);
                            }
                        }
                    }
                    if (d < 3) frame = 3;

                }
                if ((disparo[i].x) > disparo[i].x0) Imagen_Dibuja(i_sprites, disparo[i].x, disparo[i].y, Medidas[8], Medida, 241, 1, 8, 1, 0, 0); // disparo principal


            }
            else {

                disparo[i].x -= Tile_Medida3;// *1.3;
                if (disparo[i].x < -Tile_Medida) disparo[i].activo = false;

                cuantos = ((disparo[i].x0 - disparo[i].x) / Medidas[8]) + 1;
                posx = disparo[i].x;
                for (int d = 1; d < cuantos; d++) {
                    Imagen_Dibuja_Color(i_sprites, posx, disparo[i].y, Medidas[8], Medida, 241, frame, 8, 1, 0, 0, Gradiente[Color_Gradiente]); // estela
                    posx += Medidas[8];
                    azar = GetRandomValue(0, 2);
                    frame += (azar * 2);
                    if (frame > 31) {
                        frame = 33;
                        if ((d + 10) < cuantos) {
                            azar = GetRandomValue(0, 4);
                            if (azar == 100) {
                                azar = GetRandomValue(0, 5);
                                frame = 23 + (azar * 2);
                            }
                        }
                    }
                    if (d < 3) frame = 3;

                }
                Imagen_Dibuja(i_sprites, disparo[i].x, disparo[i].y, Medidas[8], Medida, 241, 31, 8, 1, 0, 0); // disparo principal

            }

            // Comprueba colisiones
            if (Nave_Explota == 0) {
                if (disparo[i].x > Pantalla_X && disparo[i].x < Pantalla_X2) {
                    borra = Humanoide_Borra(disparo[i].x, disparo[i].y, disparo[i].direccion, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                    if (borra == 1) {
                        disparo[i].activo = false;
                    }
                    else {
                        borra = Enemigo_Borra(disparo[i].x, disparo[i].y, disparo[i].direccion, s_implosion, s_explosion, s_laser, s_caida, s_bombardero, s_pod, fuente);
                        if (borra == 1) disparo[i].activo = false;
                    }
                }
            }
        }
    }
}


void Humanoide_Posiciona_Nivel(int cantidad) {
    int azar, cuantos = 0;

    while (cuantos < cantidad) {

        for (int y = 3; y < 5; y++) {
            for (int x = 4; x < 125; x++) {
                if (cuantos < cantidad)
                    if (Suelo[x][y] == 1) {
                        azar = GetRandomValue(1, 10);
                        if (azar == 1) {
                            Humanoide_Crea(x, y);
                            cuantos += 1;
                        }
                    }
            }
        }
    }
}


//----------------------------------------------------------
// Crea Enemigo
//----------------------------------------------------------
void Enemigo_Crea(int num, float x, float y) {

    int azar;

    for (int i = 0; i < Enemigos_Maximos; i++) {
        if (!enemigos[i].activo) {
            enemigos[i].x = x;
            enemigos[i].y = y;
            enemigos[i].num = num;
            enemigos[i].alterna = 0;
            enemigos[i].alterna_c = 0;
            enemigos[i].alterna_fin = 0;
            enemigos[i].estado = 0;
            enemigos[i].ID = Enemigo_ID;
            Enemigo_ID += 1;
            enemigos[i].altura = 0.0f; // Diferentes posiciones Y
            enemigos[i].altura2 = 0.0f; // Swarmers

            enemigos[i].activo = true;
            enemigos[i].cadencia = 0;
            enemigos[i].cadencia_recarga = 0;


            enemigos[i].cadencia_fin = Enemigo_Datos2[num][1]; // 300;
            enemigos[i].cadencia_empieza = Enemigo_Datos2[num][0]; //280;
            enemigos[i].cadencia_de_disparo = Enemigo_Datos2[num][2]; // 30;

            //   int cadencia, cadencia_fin, cadencia_empieza, cadencia_de_disparo;


            switch (num) {
            case 1: // Lander

# pragma region Lander

                enemigos[i].uvx = 128;
                enemigos[i].uvy = 0;
                enemigos[i].animado = 1;
                enemigos[i].largo = Medidas[9];
                enemigos[i].alto = Medidas[8];
                enemigos[i].mx = 9;
                enemigos[i].my = 8;
                enemigos[i].muvx = 117; // radar
                enemigos[i].muvy = 61;
                enemigos[i].manim = 0;
                enemigos[i].puntos = 150;
                enemigos[i].altura = GetRandomValue(0, Tile_Medida * 2); // Diferentes posiciones Y
                enemigos[i].cadencia_fin = Enemigo_Datos2[num][1]; // 300;
                enemigos[i].cadencia_empieza = Enemigo_Datos2[num][0]; //280;
                enemigos[i].cadencia_de_disparo = Enemigo_Datos2[num][2]; // 30;




                /*static float Enemigo_Datos[7][4]; // num - 0-Velocidad de disparo  1-Velocidad enemigo
static int Enemigo_Datos2[7][4]; // num - 0-cadencia empieza  1-cadencia fin  2-cadencia disparo */

                azar = GetRandomValue(0, 1);
                if (azar == 0) {
                    enemigos[i].velocidadX = Enemigo_Datos[num][1];
                }
                else {
                    enemigos[i].velocidadX = -Enemigo_Datos[num][1];
                }
                enemigos[i].velocidadY = Enemigo_Datos[num][2];
                break;

#pragma endregion

            case 2: // Mutante

# pragma region Mutante

                enemigos[i].uvx = 176;
                enemigos[i].uvy = 16;
                enemigos[i].animado = 1;
                enemigos[i].largo = Medidas[9];
                enemigos[i].alto = Medidas[8];
                enemigos[i].mx = 9;
                enemigos[i].my = 8;
                enemigos[i].muvx = 112; // radar
                enemigos[i].muvy = 57;
                enemigos[i].manim = 1;
                enemigos[i].velocidadX = Enemigo_Datos[num][1];
                enemigos[i].velocidadY = Enemigo_Datos[num][2];
                enemigos[i].alterna = GetRandomValue(0, 1);
                enemigos[i].alterna_fin = 20;
                enemigos[i].puntos = 150;

                break;

#pragma endregion

            case 3: // Baiter

#pragma region Baiter

                enemigos[i].uvx = 160;
                enemigos[i].uvy = 32;
                enemigos[i].animado = 1;
                enemigos[i].largo = Medidas[11];
                enemigos[i].alto = Medidas[4];
                enemigos[i].mx = 11;
                enemigos[i].my = 4;
                enemigos[i].muvx = 126; // radar
                enemigos[i].muvy = 61;
                enemigos[i].manim = 0;
                enemigos[i].velocidadX = Enemigo_Datos[num][1];
                enemigos[i].velocidadY = Enemigo_Datos[num][2];
                enemigos[i].alterna = 0;// GetRandomValue(0, 1);
                enemigos[i].alterna_fin = 0;// 20;
                enemigos[i].puntos = 200;




                break;

#pragma endregion

            case 4: // Bombardero

#pragma region Bombardero

                enemigos[i].uvx = 128;
                enemigos[i].uvy = 16;
                enemigos[i].animado = 1;
                enemigos[i].largo = Medidas[6];
                enemigos[i].alto = Medidas[7];
                enemigos[i].mx = 6;
                enemigos[i].my = 7;
                enemigos[i].muvx = 129; // radar
                enemigos[i].muvy = 61;
                enemigos[i].manim = 0;
                enemigos[i].velocidadX = Enemigo_Datos[num][1]; // Tile_Medida16 * 0.45;// 35;
                enemigos[i].velocidadY = Enemigo_Datos[num][2]; // Tile_Medida16 * 0.3;// 2;
                enemigos[i].alterna = GetRandomValue(0, 1);
                enemigos[i].alterna_fin = 20;
                enemigos[i].puntos = 250;

                break;

#pragma endregion

            case 5: // Pod

#pragma region Pod

                enemigos[i].uvx = 176;
                enemigos[i].uvy = 0;
                enemigos[i].animado = 1;
                enemigos[i].largo = Medidas[7];
                enemigos[i].alto = Medidas[7];
                enemigos[i].mx = 7;
                enemigos[i].my = 7;
                enemigos[i].muvx = 17; // radar
                enemigos[i].muvy = 59;
                enemigos[i].manim = 1;
                enemigos[i].velocidadX = Enemigo_Datos[num][1];
                enemigos[i].velocidadY = Enemigo_Datos[num][2];
                azar = GetRandomValue(0, 1);
                if (azar == 0) enemigos[i].velocidadX *= -1;
                azar = GetRandomValue(0, 1);
                if (azar == 0) enemigos[i].velocidadY *= -1;
                enemigos[i].alterna = 0;
                enemigos[i].alterna_fin = 20;
                enemigos[i].puntos = 1000;




                break;

#pragma endregion


            case 6: // Swarmer

#pragma region Swarmer

                enemigos[i].uvx = 224;
                enemigos[i].uvy = 16;
                enemigos[i].animado = 0;
                enemigos[i].largo = Medidas[5];
                enemigos[i].alto = Medidas[4];
                enemigos[i].mx = 5;
                enemigos[i].my = 4;
                enemigos[i].muvx = 132; // radar
                enemigos[i].muvy = 61;
                enemigos[i].manim = 0;

                enemigos[i].altura = Tile_Medida16 * 0.1f; // Velocidad minima
                enemigos[i].altura2 = Tile_Medida16 * 0.6f; // Velocidad maxima

                enemigos[i].velocidadX = enemigos[i].altura;
                enemigos[i].velocidadY = enemigos[i].altura;

                azar = GetRandomValue(1, 3);
                enemigos[i].alterna_c = azar * Tile_Medida2; // X
                azar = GetRandomValue(0, 1);
                if (azar == 1)  enemigos[i].alterna_c *= -1;

                azar = GetRandomValue(1, 3);
                enemigos[i].alterna_fin = azar * Tile_Medida2; // Y
                azar = GetRandomValue(0, 1);
                if (azar == 1)  enemigos[i].alterna_fin *= -1;
                enemigos[i].puntos = 150;

                break;

#pragma endregion



            default:
                break;
            }
            enemigos[i].cadencia = GetRandomValue(1, enemigos[i].cadencia_empieza);

            break;
        }
    }
}


//----------------------------------------------------------
// Dibuja Enemigos
//----------------------------------------------------------
void Enemigo_Dibuja(Texture2D i_sprites, Sound s_abduce, Sound s_proyectil, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_humanoide, Sound s_pod) {

    float x;
    int anim = 0;
    float d1, d2;
    int casillax;
    int valor;
    float valor2;
    float destinoX, destinoY;

    // colision
    int nx = (Nave_X + Nave_Desplazamiento) - Nave_Medida3;
    int ny = (Nave_Y)-Nave_Medida4;
    int nx2 = nx + Nave_Medida;
    int ny2 = ny + Nave_Medida2;
    bool colision;
    int ex, ex2, ey, ey2;

    Enemigos_N = 0;

    for (int i = 0; i < Enemigos_Maximos; i++) {
        if (enemigos[i].activo) {
            Enemigos_N += 1;
            // Actualiza
            //------------------------------------------------------------------------
            // Posicion Circular
            x = (enemigos[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            // IA
            //------------------------------------------------------------------------
            if (Nave_Explota == 0) {

                switch (enemigos[i].num) {
                case 1: // Lander (Abduce)

#pragma region Lander

                    switch (enemigos[i].estado) {
                    case 0:

                        // Altura de Montañas
                        //-------------------------------------------------------------------------------
                        casillax = ((enemigos[i].x / 16) - (Pantalla_X / Tile_Medida)) + 1;
                        if (casillax < 1) casillax += 129;
                        //  DrawText(TextFormat("%i", casillax), x, enemigos[i].+ Tile_Medida, 30, YELLOW);
                        enemigos[i].x += enemigos[i].velocidadX;

                        valor = Suelo[casillax][5] - enemigos[i].altura; // altura de montaña
                        if (valor < (enemigos[i].y - enemigos[i].velocidadY)) {
                            enemigos[i].y -= enemigos[i].velocidadY;
                        }
                        else if (valor > (enemigos[i].y + enemigos[i].velocidadY)) {
                            enemigos[i].y += enemigos[i].velocidadY;
                        }

                        if (Enemigo_Abductor > 0) {
                            if (Enemigo_Abductor == enemigos[i].ID) {
                                d1 = enemigos[i].x - (Humanoides_Abduccion_X);
                                if (d1 < 0) d1 += 2064.0f;
                                d2 = (2064.0f - enemigos[i].x) + Humanoides_Abduccion_X;
                                if (d2 > 2064.0f) d2 -= 2064.0f;

                                if (d1 < Tile_Medida || d2 < Tile_Medida) {
                                    enemigos[i].velocidadX *= 0.98;

                                    if (enemigos[i].velocidadX > 0) {
                                        if (enemigos[i].velocidadX < (Tile_Medida16 * 0.05)) enemigos[i].velocidadX = (Tile_Medida16 * 0.05);
                                    }
                                    else {
                                        if (enemigos[i].velocidadX > -(Tile_Medida16 * 0.05)) enemigos[i].velocidadX = -(Tile_Medida16 * 0.05);
                                    }
                                }

                                if (d1 < 1 || d2 < 1) { // Baja para abducir
                                    enemigos[i].estado = 1;
                                    enemigos[i].x = Humanoides_Abduccion_X;
                                    enemigos[i].velocidadX = 0;
                                }
                            }
                        }
                        break;

                    case 1: // Baja para abducir
                        if (enemigos[i].y < Humanoides_Abduccion_Y) {
                            enemigos[i].y += enemigos[i].velocidadY;
                        }
                        else {
                            enemigos[i].estado = 2;
                            PlaySound(s_abduce);
                            for (int h = 0; h < Humanoides_Maximos; h++) {
                                if (humanoides[h].ID == Humanoide_Abducido) humanoides[h].estado = 3; // sube humanoide
                            }
                        }
                        break;

                    case 2: // sube a humanoide abducido
                        if (enemigos[i].y > (Marcador_Y - Tile_Medida)) {
                            enemigos[i].y -= enemigos[i].velocidadY;
                        }
                        else { // se convierte en mutante
                            Lander_Mutante(i);
                            for (int h = 0; h < Humanoides_Maximos; h++) {
                                if (humanoides[h].ID == Humanoide_Abducido) {
                                    humanoides[h].activo = false; //
                                    Humanoides_N -= 1;
                                    if (Humanoides_N == 0) { // Explota Mundo
                                        Pantalla_Bomba = 1; Pantalla_BombaC = 0;  Pantalla_BombaCF = 0; Pantalla_BombaM = 0;
                                        Mundo_Destruido = 1; Pantalla_BombaT = 70; Pantalla_BombaE = 1;
                                    }
                                }
                            }
                            Enemigo_Abductor = 0;
                            Humanoide_Abducido = 0;
                        }
                        break;
                    }


                    //    DrawText(TextFormat("%i", enemigos[i].estado), x, humanoides[i].y - Tile_Medida, 30, YELLOW);

                    break;

#pragma endregion

                case 2: // Mutante (Persigue a Nave Zig-Zag)

#pragma region Mutante

                    d1 = (enemigos[i].x) - (Nave_Scroll);
                    d2 = (2064.0f - enemigos[i].x) + Nave_Scroll;

                    if (d1 < 0) d1 += 2064.0f;
                    if (d2 > 2064.0f) d2 -= 2064.0f;
                    if (d1 < d2) {
                        if (d1 > enemigos[i].velocidadX) {
                            enemigos[i].x -= enemigos[i].velocidadX;
                        }
                    }
                    else if (d2 < d1) {
                        if (d2 > enemigos[i].velocidadX) {
                            enemigos[i].x += enemigos[i].velocidadX;
                        }
                    }

                    // Solo modifica su altura cuando esta cerca de la nave principal
                    if (d1 < Tile_Medida || d2 < Tile_Medida) {
                        if ((enemigos[i].y + enemigos[i].velocidadY) < Nave_Y) {
                            enemigos[i].y += enemigos[i].velocidadY;
                        }
                        else if ((enemigos[i].y - enemigos[i].velocidadY) > Nave_Y) {
                            enemigos[i].y -= enemigos[i].velocidadY;
                        }

                    }
                    else { // Cambia de altura

                        enemigos[i].alterna_c += 1;
                        if (enemigos[i].alterna_c > enemigos[i].alterna_fin) {
                            enemigos[i].alterna = !enemigos[i].alterna;
                            enemigos[i].alterna_c = 0;
                            enemigos[i].alterna_fin = GetRandomValue(3, 20);
                        }

                        if (enemigos[i].alterna == 0) {
                            if (enemigos[i].y < Marcador_Y) {
                                enemigos[i].alterna = 1;
                            }
                            else {
                                enemigos[i].y -= enemigos[i].velocidadY;
                            }
                        }
                        else {
                            if (enemigos[i].y > Tile_Monte) {
                                enemigos[i].alterna = 0;
                            }
                            else {
                                enemigos[i].y += enemigos[i].velocidadY;
                            }
                        }
                    }
                    break;

#pragma endregion

                case 3: // Baiter (Persigue a Nave)

#pragma region Baiter

                    d1 = (enemigos[i].x) - (Nave_Scroll);
                    d2 = (2064.0f - enemigos[i].x) + Nave_Scroll;

                    // Calcula que direccion es mas corta para perseguir
                    if (d1 < 0) d1 += 2064.0f;
                    if (d2 > 2064.0f) d2 -= 2064.0f;
                    if (d1 < d2) {
                        if (d1 > enemigos[i].velocidadX) {
                            enemigos[i].x -= enemigos[i].velocidadX;
                        }
                    }
                    else if (d2 < d1) {
                        if (d2 > enemigos[i].velocidadX) {
                            enemigos[i].x += enemigos[i].velocidadX;
                        }
                    }

                    // Ajusta altura
                    if (x > Pantalla_X && x < Pantalla_X2) {
                        if ((enemigos[i].y + enemigos[i].velocidadY) < Nave_Y) {
                            enemigos[i].y += enemigos[i].velocidadY;
                        }
                        else if ((enemigos[i].y - enemigos[i].velocidadY) > Nave_Y) {
                            enemigos[i].y -= enemigos[i].velocidadY;
                        }
                    }


                    break;
#pragma endregion

                case 4: // Bomber (Crea Minas)

#pragma region Bomber

                    enemigos[i].x += enemigos[i].velocidadX;
                    enemigos[i].y += enemigos[i].velocidadY;

                    if (enemigos[i].velocidadY < 0) { // subiendo
                        if (enemigos[i].y < (Marcador_Y + Tile_Medida)) enemigos[i].velocidadY *= -1;
                    }
                    else { // bajando
                        if (enemigos[i].y > (Pantalla_Alto - Tile_Medida)) enemigos[i].velocidadY *= -1;
                    }

                    break;

#pragma endregion

                case 5: // Pod (Movimiento Erratico * Genera Swarmers)

#pragma region Pod

                    enemigos[i].x += enemigos[i].velocidadX;
                    enemigos[i].y += enemigos[i].velocidadY;

                    if (enemigos[i].y < 0) enemigos[i].y += Pantalla_Alto;
                    if (enemigos[i].y > Pantalla_Alto) enemigos[i].y -= Pantalla_Alto;

                    enemigos[i].alterna += 1;
                    if (enemigos[i].alterna > enemigos[i].alterna_fin) {
                        enemigos[i].alterna = 0;

                        enemigos[i].velocidadX = GetRandomValueFloat(-Tile_Medida16 * 0.3f, Tile_Medida16 * 0.3f);
                        enemigos[i].velocidadY = GetRandomValueFloat(-Tile_Medida16 * 0.3f, Tile_Medida16 * 0.3f);
                        enemigos[i].alterna_fin = GetRandomValue(30, 100);
                        valor = GetRandomValue(0, 1);
                        if (valor == 0) enemigos[i].velocidadX *= -1;
                        valor = GetRandomValue(0, 1);
                        if (valor == 0) enemigos[i].velocidadY *= -1;
                    }

                    break;

#pragma endregion


                case 6: // Swarmer (Escuadron)

#pragma region Swarmer

                    destinoX = Nave_Scroll + (enemigos[i].alterna_c);
                    destinoY = Nave_Y + (enemigos[i].alterna_fin);
                    if (destinoX < 0.0f) destinoX += 2064.0f;
                    if (destinoX > 2064.0f) destinoX -= 2064.0f;
                    if (destinoY < 0.0f) destinoY += 2064.0f;
                    if (destinoY > 2064.0f) destinoY -= 2064.0f;



                    d1 = (enemigos[i].x) - (destinoX);
                    d2 = (2064.0f - enemigos[i].x) + destinoX;

                    if (d1 < 0) d1 += 2064.0f;
                    if (d2 > 2064.0f) d2 -= 2064.0f;

                    if ((d1 < (Tile_Medida * 2)) || (d2 < (Tile_Medida * 2))) {
                        enemigos[i].velocidadX *= 0.98;
                        enemigos[i].velocidadY *= 0.98;
                        if (enemigos[i].velocidadX < enemigos[i].altura) enemigos[i].velocidadX = enemigos[i].altura;
                        if (enemigos[i].velocidadY < enemigos[i].altura) enemigos[i].velocidadY = enemigos[i].altura;
                    }
                    else {
                        enemigos[i].velocidadX *= 1.02;
                        enemigos[i].velocidadY *= 1.02;
                        if (enemigos[i].velocidadX > enemigos[i].altura2) enemigos[i].velocidadX = enemigos[i].altura2;
                        if (enemigos[i].velocidadY > enemigos[i].altura2) enemigos[i].velocidadY = enemigos[i].altura2;
                    }

                    // Calcula que direccion es mas corta para posicionar
                    if (d1 < d2) {
                        if (d1 > enemigos[i].velocidadX) {
                            enemigos[i].x -= enemigos[i].velocidadX;
                        }
                    }
                    else if (d2 < d1) {
                        if (d2 > enemigos[i].velocidadX) {
                            enemigos[i].x += enemigos[i].velocidadX;
                        }
                    }

                    // Ajusta altura
                    if (x > Pantalla_X && x < Pantalla_X2) {
                        if ((enemigos[i].y + enemigos[i].velocidadY) < destinoY) {
                            enemigos[i].y += enemigos[i].velocidadY;
                        }
                        else if ((enemigos[i].y - enemigos[i].velocidadY) > destinoY) {
                            enemigos[i].y -= enemigos[i].velocidadY;
                        }
                    }

                    // Nueva posicion
                    if (d1 < Tile_Medida || d2 < Tile_Medida) {
                        valor = GetRandomValue(1, 6);
                        enemigos[i].alterna_c = valor * Tile_Medida2; // X
                        valor = GetRandomValue(0, 1);
                        if (valor == 1)  enemigos[i].alterna_c *= -1;

                        valor = GetRandomValue(1, 4);
                        enemigos[i].alterna_fin = valor * Tile_Medida2; // Y
                        valor = GetRandomValue(0, 1);
                        if (valor == 1)  enemigos[i].alterna_fin *= -1;
                    }


                    if (enemigos[i].y < 0) enemigos[i].y += Pantalla_Alto;
                    if (enemigos[i].y > Pantalla_Alto) enemigos[i].y -= Pantalla_Alto;

                    break;

#pragma endregion

                default:
                    break;
                }


                if (enemigos[i].x > 2064.0f) enemigos[i].x -= 2064.0f;
                if (enemigos[i].x < 0.0f) enemigos[i].x += 2064.0f;

                ex = x - (enemigos[i].largo / 2);
                ey = enemigos[i].y - (enemigos[i].alto / 2);
                ex2 = ex + enemigos[i].largo;
                ey2 = ey + enemigos[i].alto;

                // Colision
                if (Nave_Invulnerable == 0) {
                    colision = ((ex2 > nx) && (ex < nx2) && (ey2 > ny) && (ey < ny2));
                    if (colision) {
                        Nave_Explota = 1;
                        Nave_Velocidad = 0;
                        Humanos_old = Humanoides_N;
                        Enemigos_Contabiliza(0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                        //if (Joystick == 1) VibrateEx(0.3f, 1.0f);
                    }
                }

                // Dispara
                //------------------------------------------------------------------------
                //  if (x > 0 && x < Pantalla_Largo) {
                if (Pantalla_X > 0 && x < Pantalla_X2) {

                    if (enemigos[i].y > Marcador_Y) {

                        enemigos[i].cadencia += 1;
                        if (enemigos[i].cadencia > enemigos[i].cadencia_empieza) {
                            enemigos[i].cadencia_recarga -= 1;
                            if (enemigos[i].cadencia_recarga < 1) {
                                enemigos[i].cadencia_recarga = enemigos[i].cadencia_de_disparo;
                                if (enemigos[i].num == 4) {
                                    Proyectil_Crea(enemigos[i].x, x, enemigos[i].y, 1, enemigos[i].num, s_proyectil, s_humanoide); // Mina
                                }
                                else {
                                    Proyectil_Crea(enemigos[i].x, x, enemigos[i].y, 0, enemigos[i].num, s_proyectil, s_humanoide); // Proyectil
                                }
                            }
                        }
                        if (enemigos[i].cadencia > enemigos[i].cadencia_fin) enemigos[i].cadencia = 0;
                    }
                }

            }
            //----------------------- IA

            // Dibuja
            //------------------------------------------------------------------------
            if (enemigos[i].animado == 1) {
                anim = Frame;
            }
            else {
                anim = 0;
            }

            Imagen_Dibuja(i_sprites, x, enemigos[i].y, enemigos[i].largo, enemigos[i].alto, enemigos[i].uvx + anim, enemigos[i].uvy, enemigos[i].mx, enemigos[i].my, 1, 0); // Lander

            //   DrawText(TextFormat("%i", enemigos[i].ID), x, enemigos[i].y + Tile_Medida2, 30, YELLOW);
            //   DrawText(TextFormat("%i", enemigos[i].estado), x, enemigos[i].y + Tile_Medida, 30, GREEN);

            //     DrawText(TextFormat("%f", d1), x, enemigos[i].y + Tile_Medida2, 30, YELLOW);
            //     DrawText(TextFormat("%f", d2), x, enemigos[i].y + Tile_Medida, 30, YELLOW);
        }
    }
}


//----------------------------------------------------------
// Borra Enemigo con disparo
//----------------------------------------------------------
int Enemigo_Borra(int d1, int dy, int dd, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_caida, Sound s_bombardero, Sound s_pod, Font fuente) {

    float x;
    float c1, c2, a1, a2, d2;

    if (dd = 0) {
        d2 = d1 - Tile_Medida3;
    }
    else {
        d2 = d1;
        d1 += Tile_Medida3;
    }


    for (int i = 0; i < Enemigos_Maximos; i++) {
        if (enemigos[i].activo) {

            // Posicion Circular
            x = (enemigos[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            if (x > Pantalla_X && x < Pantalla_X2) {

                a1 = enemigos[i].y - (enemigos[i].alto / 2);
                a2 = enemigos[i].y + (enemigos[i].alto / 2);

                if (dy > a1 && dy < a2) {
                    c1 = x - (enemigos[i].largo / 2);
                    c2 = x + (enemigos[i].largo / 2);

                    bool colision = (d1 > c1) && (d2 < c2);

                    if (colision) {
                        enemigos[i].activo = false;
                        Explosion_Crea(enemigos[i].x, enemigos[i].y, enemigos[i].num, 0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);

                        if (enemigos[i].ID == Enemigo_Abductor) {
                            if (enemigos[i].estado == 2) { // Esta abduciendo a un humanoide
                                // Cae Humanoide
                                for (int h = 0; h < Humanoides_Maximos; h++) {
                                    if (humanoides[h].ID == Humanoide_Abducido) {
                                        humanoides[h].estado = 1;
                                        humanoides[h].caida = 0;
                                        PlaySound(s_caida);
                                    }
                                }
                            }
                            else {
                                for (int h = 0; h < Humanoides_Maximos; h++) {
                                    if (humanoides[h].ID == Humanoide_Abducido) {
                                        humanoides[h].estado = 0;
                                        Humanoide_Abducido = 0;
                                    }
                                }
                            }
                            Enemigo_Abductor = 0;

                        }
                        Puntua(fuente, enemigos[i].puntos);

                        if (enemigos[i].num == 5) { // Pod (Genera Swarmers)

                            Swarmer_Crea = 1;
                            Swarmer_CreaX = enemigos[i].x;
                            Swarmer_CreaY = enemigos[i].y;
                        }
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


//----------------------------------------------------------
// Crea Explosion / Implosion
//----------------------------------------------------------
void Explosion_Crea(float x, float y, int num, int modo, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_pod) {

    int cx, azar;


    // modo: 0-explosion  1-implosion

    for (int i = 0; i < Explosiones_Maximas; i++) {
        if (!explosiones[i].activo) {
            explosiones[i].x = x;
            explosiones[i].y = y;
            explosiones[i].num = num;
            explosiones[i].modo = modo;
            explosiones[i].activo = true;
            explosiones[i].uvx = 128;
            explosiones[i].uvy = 0;
            explosiones[i].largo = 8;
            explosiones[i].alto = 8;

            if (explosiones[i].modo == 0) { // explosion
                explosiones[i].escala = Tile_Medida16;
                if (IsSoundPlaying(s_laser)) StopSound(s_laser);
                if (explosiones[i].num == 4) {
                    PlaySound(s_bombardero);
                }
                else if (explosiones->num == 5) { // pod
                    PlaySound(s_pod);
                }
                else {
                    PlaySound(s_explosion);
                }
            }
            else { // implosion

                Enemigos_N += 1;

                explosiones[i].escala = Tile_Medida * 4;

                // Posicion Circular
                cx = (explosiones[i].x - (Scroll)) * Tile_Medida16;
                if (cx < 0) cx += Scroll_Fin;

                if (cx > 0 && cx < Pantalla_Largo) { // Solo reproduce sonido si esta dentro de la pantalla
                    if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
                    PlaySound(s_implosion);
                }

            }

            switch (explosiones[i].num) {

            case -1: // Mundo explota
                azar = GetRandomValue(0, 1);
                if (azar == 0) {
                    explosiones[i].uvx = 224;
                    explosiones[i].uvy = 0;
                    explosiones[i].largo = 14;
                    explosiones[i].alto = 6;
                }
                else {
                    explosiones[i].uvx = 224;
                    explosiones[i].uvy = 8;
                    explosiones[i].largo = 16;
                    explosiones[i].alto = 6;
                }
                break;


            case 1: // Lander
                explosiones[i].uvx = 128;
                explosiones[i].uvy = 0;
                explosiones[i].largo = 8;
                explosiones[i].alto = 8;
                break;


            case 2: // Mutante
                explosiones[i].uvx = 176;
                explosiones[i].uvy = 16;
                explosiones[i].largo = 8;
                explosiones[i].alto = 8;
                break;

            case 3: // Baiter
                explosiones[i].uvx = 160;
                explosiones[i].uvy = 32;
                explosiones[i].largo = 11;
                explosiones[i].alto = 4;
                break;

            case 4: // Bombardero
                explosiones[i].uvx = 128;
                explosiones[i].uvy = 16;
                explosiones[i].largo = 7;
                explosiones[i].alto = 7;
                break;

            case 5: // Pod
                explosiones[i].uvx = 176;
                explosiones[i].uvy = 0;
                explosiones[i].largo = 7;
                explosiones[i].alto = 7;
                break;

            case 6: // Swarmer
                explosiones[i].uvx = 224;
                explosiones[i].uvy = 16;
                explosiones[i].largo = 5;
                explosiones[i].alto = 4;
                break;


            default: // Humanoide
                explosiones[i].uvx = 0;
                explosiones[i].uvy = 0;
                explosiones[i].largo = 5;
                explosiones[i].alto = 8;
                break;
            }
            break;
        }
    }
}


//----------------------------------------------------------
// Dibuja Explosiones
//----------------------------------------------------------
void Explosiones_Dibuja(Texture2D i_sprites) {

    float x;
    float nx, ny;
    float cx, cy;
    float dx, dy;
    float angulo;
    float dist;
    int crea = 0;


    for (int i = 0; i < Explosiones_Maximas; i++) {
        if (explosiones[i].activo) {

            // Posicion Circular
            x = (explosiones[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            for (int n = 0; n < (explosiones[i].alto + 1); n += 2) {
                for (int m = 0; m < (explosiones[i].largo + 1); m += 2) {

                    cx = (explosiones[i].largo / 2.0f);
                    cy = (explosiones[i].alto / 2.0f) - 1;

                    dx = cx - m;
                    dy = cy - n;
                    angulo = atan2(dy, dx);
                    dist = sqrtf(dx * dx + dy * dy);
                    nx = x + cosf(angulo) * explosiones[i].escala * -dist;
                    ny = explosiones[i].y + sinf(angulo) * explosiones[i].escala * -dist;


                    Imagen_Dibuja(i_sprites, nx, ny, Medidas[2], Medidas[2], explosiones[i].uvx + m, explosiones[i].uvy + n, 2, 2, 1, 0); // retales
                }
            }

            if (explosiones[i].modo == 0) { // explosion
                if (Nave_Explota == 0) explosiones[i].escala += Tile_Medida16;
                if (explosiones[i].escala > Tile_Medida) explosiones[i].activo = false;
            }
            else { // implosion
                Enemigos_N += 1;
                if (Nave_Explota == 0) explosiones[i].escala -= Tile_Medida16;
                if (explosiones[i].escala < Tile_Medida16) {
                    explosiones[i].activo = false;
                    if (explosiones[i].num == -2) {
                        Nave_Hiper_Espacio = 0;
                    }
                    else {
                        Enemigo_Crea(explosiones[i].num, explosiones[i].x, explosiones[i].y);
                    }
                }
            }

        }
    }
}


void Abduce(Sound s_implosion, Sound s_explosion, Sound s_laser) {

    int lander = 0;
    float dista = 0.0f;
    float distaMax = 30000.0f;
    int elige = -1;
    int elige_h = -1;
    float elige_hx = 0;

    Enemigo_Abductor = 0;
    Humanoide_Abducido = 0;

    for (int i = 0; i < Humanoides_Maximos; i++) {
        if (humanoides[i].activo == true) {
            if (elige_h == -1) {
                if (humanoides[i].estado == 0) {
                    elige_h = i;
                }
                break;
            }
        }
    }

    if (elige_h > -1) {

        elige_hx = humanoides[elige_h].x;
        // Elige el Lander mas cercano
        for (int i = 0; i < Enemigos_Maximos; i++) {

            if (enemigos[i].activo == true) {
                if (enemigos[i].num == 1) { // Lander

                    if (enemigos[i].velocidadX < 0.0f) {
                        dista = (enemigos[i].x - elige_hx);
                        if (dista < 0) dista += 2064.0f;
                        if (dista < distaMax) {
                            distaMax = dista;
                            elige = i;
                        }
                    }
                    else {
                        dista = (2064.0f - enemigos[i].x) + elige_hx;// (enemigos[i].x - Humanoides_Abduccion_X);
                        if (dista > 2064.0f) dista -= 2064.0f;
                        if (dista < distaMax) {
                            distaMax = dista;
                            elige = i;
                        }
                    }

                }
            }
        }

        if (elige > -1 && elige_h > -1) {
            Enemigo_Abductor = enemigos[elige].ID;

            humanoides[elige_h].estado = 10; // Preparado para ser abducido
            Humanoide_Abducido = humanoides[elige_h].ID;
            Humanoides_Abduccion_X = humanoides[elige_h].x;
            Humanoides_Abduccion_Y = humanoides[elige_h].y - (Tile_Medida16 * 10);

        }

    }


}


void Puntos_Dibuja(Texture2D i_sprites) {

    float x;
    int mas = 0;;
    // Posicion Circular
    x = (Puntos_HX - (Scroll)) * Tile_Medida16;
    if (x < 0) x += Scroll_Fin;

    if (Puntos_Q == 1) mas = -9; //250

    Imagen_Dibuja(i_sprites, x, Puntos_HY, Medidas[13], Medidas[7], 64 + Frame, 57 + mas, 13, 7, 1, 0);
    Puntos_C += 1;
    if (Puntos_C > 200) Puntos_Activo = 0;
}


void Estrellas_Fondo_Dibuja() {

    float x;

    for (int i = 0; i < Estrellas_N; i++) {
        estrellas[i].retardo--;

        if (estrellas[i].retardo <= 0) {
            // Cambiar al siguiente color en el gradiente
            estrellas[i].indice = (estrellas[i].indice + 1) % 8;
            estrellas[i].contador++;

            // Reset del retardo
            estrellas[i].retardo = GetRandomValue(20, 60);

            // Si ya alcanzó su número máximo de cambios, reaparece
            if (estrellas[i].contador >= estrellas[i].maxCambios) {
                estrellas[i].x = GetRandomValue(1.0f, 2063.0f);
                if (Mundo_Aniquilado == 0) {
                    estrellas[i].y = GetRandomValue(Marcador_Y + Tile_Medida2, Tile_Monte + Tile_Medida2);
                }
                else {
                    estrellas[i].y = GetRandomValue(Marcador_Y, Pantalla_Alto - Tile_Medida2);
                }
                estrellas[i].indice = GetRandomValue(0, 7);
                estrellas[i].contador = 0;
                estrellas[i].maxCambios = GetRandomValue(1, 3);
            }
        }
    }

    for (int i = 0; i < Estrellas_N; i++) {

        // Posicion Circular
        x = (estrellas[i].x - (Scroll / 2)) * Tile_Medida16;
        if (x < 0) x += Scroll_Fin;

        //        DrawPixel(estrellas[i].x, estrellas[i].y, Gradiente[estrellas[i].indice]);
        if (Pantalla_Bomba == 0) {
            DrawRectangle(x, estrellas[i].y, Tile_Medida16, Tile_Medida16, Gradiente[estrellas[i].indice]);
        }
        else {
            if (Pantalla_Bomba_Modo == 0) {
                DrawRectangle(x, estrellas[i].y, Tile_Medida16, Tile_Medida16, BLACK);
            }
            else {
                DrawRectangle(x, estrellas[i].y, Tile_Medida16, Tile_Medida16, WHITE);
            }
        }
        //        DrawRectangle(estrellas[i].x, estrellas[i].y, Tile_Medida16, Tile_Medida16, Gradiente[estrellas[i].indice]);

    }

}


void Mundo_Explota(Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_pod) {

    int bumx = Scroll + GetRandomValue(0, Tile_Medida2);
    int bumy = 0;

    for (int i = 0; i < 7; i++) {
        bumx += Tile_Medida2;
        bumy = Tile_Monte + (Tile_Medida_Monte * 3) + GetRandomValue(-Tile_Medida * 2, Tile_Medida + Tile_Medida2);
        Explosion_Crea(bumx, bumy, -1, 0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
    }
}


void Lander_Mutante(int i) 
{
    enemigos[i].estado = 0;
    enemigos[i].num = 2;
    enemigos[i].uvx = 176;
    enemigos[i].uvy = 16;
    enemigos[i].animado = 1;
    enemigos[i].largo = Medidas[9];
    enemigos[i].alto = Medidas[8];
    enemigos[i].mx = 9;
    enemigos[i].my = 8;
    enemigos[i].muvx = 112; // radar
    enemigos[i].muvy = 57;
    enemigos[i].manim = 1;
    enemigos[i].velocidadX = Tile_Medida16 * 0.7f;
    enemigos[i].velocidadY = Tile_Medida16;
}


void Bomba_Inteligente(Font fuente, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_caida, Sound s_bombardero, Sound s_pod) {

    float x;

    for (int i = 0; i < Enemigos_Maximos; i++) {
        if (enemigos[i].activo) {

            // Posicion Circular
            x = (enemigos[i].x - (Scroll)) * Tile_Medida16;
            if (x < 0) x += Scroll_Fin;

            if (x > Pantalla_X && x < Pantalla_X2) {

                enemigos[i].activo = false;
                Explosion_Crea(enemigos[i].x, enemigos[i].y, enemigos[i].num, 0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);

                if (enemigos[i].ID == Enemigo_Abductor) {
                    if (enemigos[i].estado == 2) { // Esta abduciendo a un humanoide
                        // Cae Humanoide
                        for (int h = 0; h < Humanoides_Maximos; h++) {
                            if (humanoides[h].ID == Humanoide_Abducido) {
                                humanoides[h].estado = 1;
                                humanoides[h].caida = 0;
                                PlaySound(s_caida);
                            }
                        }
                    }
                    else {
                        for (int h = 0; h < Humanoides_Maximos; h++) {
                            if (humanoides[h].ID == Humanoide_Abducido) {
                                humanoides[h].estado = 0;
                                Humanoide_Abducido = 0;
                            }
                        }
                    }
                    Enemigo_Abductor = 0;

                }
                Puntua(fuente, enemigos[i].puntos);
            }
        }
    }
}


void Hiper_Espacio_Dibuja(Texture2D i_sprites) { // Nave Jugador Retales

    float cx, cy, dx, dy, angulo, dist, nx, ny, x;
    int uvx, uvy;

    if (Nave_HiperD == 0) {
        Nave_Hiper_Distancia += Tile_Medida16;
        if (Nave_Hiper_Distancia > (Tile_Medida)) {
            Nave_HiperD = 1;
            // Hiper Espacio
            Tile_N = GetRandomValue(2, 127);
            Scroll = Tile_N * 16;
            Nave_Y = GetRandomValue(Nave_Y1, Nave_Y2);
            if (Nave_Direccion == 0) {
                Nave_Direccion = 1;
            }
            else {
                Nave_Direccion = 0;
            }
            Nave_Hiper_Distancia = Tile_Medida * 3;
            Nave_Velocidad = 0.0f;
        }
    }
    else {
        Nave_Hiper_Distancia -= Tile_Medida16;
        if (Nave_Hiper_Distancia < Tile_Medida16) Nave_Hiper_Espacio = 0;
    }

    x = Nave_X + Nave_Desplazamiento;

    if (Nave_Direccion == 0) {
        uvx = 64;
        uvy = 0;
    }
    else {
        uvx = 65;
        uvy = 16;
    }

    for (int n = 0; n < 7; n += 2) {
        for (int m = 0; m < 16; m += 2) {

            cx = 7.0f;
            cy = 3.0f;
            dx = cx - m;
            dy = cy - n;
            angulo = atan2(dy, dx);
            dist = sqrtf(dx * dx + dy * dy);
            nx = x + cosf(angulo) * Nave_Hiper_Distancia * -dist;
            ny = Nave_Y + sinf(angulo) * Nave_Hiper_Distancia * -dist;
            Imagen_Dibuja(i_sprites, nx, ny, Medidas[2], Medidas[2], uvx + m, uvy + n, 2, 2, 1, 0); // retales
        }
    }
}


//----------------------------------------------------------
// Crea Explosion Nave Jugador
//----------------------------------------------------------
void Particulas_Explosion(float x, float y) {

    for (int i = 0; i < Particulas_Maximas; i++) {
        particulas[i].x = x;
        particulas[i].y = y;
        particulas[i].angulo = GetRandomValue(0, 359);
        particulas[i].velocidad = GetRandomValueFloat(Tile_Medida16 / 2, Tile_Medida16 * 4);
        particulas[i].activo = true;
    }
}


//----------------------------------------------------------
// Dibuja Explosion Nave Jugador
//----------------------------------------------------------
void Particulas_Dibuja(Font fuente, Sound s_laser, Sound s_implosion, Sound s_explosion, Sound s_bomba, Sound s_caida, Sound s_bombardero, Sound s_inicio, Sound s_puntos, Sound s_pod) {
    int Vgameover = 0;

    for (int i = 0; i < Particulas_Maximas; i++) {


        particulas[i].x += cosf(particulas[i].angulo) * particulas[i].velocidad;
        particulas[i].y += +sinf(particulas[i].angulo) * particulas[i].velocidad;

        particulas[i].velocidad *= 1.025;

        if ((particulas[i].x > Pantalla_Largo) || (particulas[i].y > Pantalla_Alto) || (particulas[i].x < 0) || (particulas[i].y < 0)) {
            //   particulas[i].activo = false;
        }
        if (Nave_Explota < 70) {
            DrawRectangle(particulas[i].x, particulas[i].y, Tile_Medida16 * 2, Tile_Medida16 * 2, WHITE);
        }
        else {
            DrawRectangle(particulas[i].x, particulas[i].y, Tile_Medida16 * 2, Tile_Medida16 * 2, Gradiente[Color_Gradiente]);
        }
        //            DrawRectangle(particulas[i].x, particulas[i].y, Tile_Medida16 * 2, Tile_Medida16 * 2, Gradiente[Color_Gradiente]);

    }

    if (Vidas == 0 && Nave_Explota > 60) {
        Pantalla_Bomba = 0;
        //ClearBackground(BLACK);
        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[16], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Pantalla_MitadY;
        DrawTextEx(fuente, textos[16], position, fuente.baseSize * Medida, Medida4, DARKBLUE); // Electronic arts
        Vgameover = 150;
    }

    Nave_Explota += 1;
    if (Nave_Explota == 49) {
        if (IsSoundPlaying(s_laser)) StopSound(s_laser);
        if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
        if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
        if (IsSoundPlaying(s_caida)) StopSound(s_caida);
        PlaySound(s_bomba);
        Particulas_Explosion(Nave_X + Nave_Desplazamiento, Nave_Y);
    }
    if (Nave_Explota > 200 + Vgameover) {

        Restaura_Variables();
        Menu = 0;
        Humanoide_Posiciona_Nivel(Humanos_old); // Crea humanoides
        Vidas -= 1;
        Nave_Explota = 0;


        Enemigos_Contabiliza(1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);


        if (Vidas < 0) {
            // Aqui Mueres, Se comprueba si accedes a la tabla de records

            if (Puntos > TR_Puntuacion[7]) {
                Menu = 3; // 3  4-tabla records 3-iniciales
                PlaySound(s_puntos);
            }
            else {
                Menu = 4; // 3  4-tabla records 3-iniciales
                PlaySound(s_inicio);
            }

            if (IsSoundPlaying(s_laser)) StopSound(s_laser);
            if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
            if (IsSoundPlaying(s_implosion)) StopSound(s_implosion);
            if (IsSoundPlaying(s_caida)) StopSound(s_caida);
        }
    }
}


//----------------------------------------------------------
// Crea Disparo Enemigo
//----------------------------------------------------------
void Proyectil_Crea(float x, float x2, float y, int clase, int num, Sound s_proyectil, Sound s_humanoide) {
    float dx, dy, angulo;
    float d1, d2;


    for (int i = 0; i < Proyectiles_Maximos; i++) {

        if (!proyectil[i].activo) {
            proyectil[i].activo = true;
            proyectil[i].x = x2;
            proyectil[i].y = y;
            proyectil[i].tiempo = 0;
            proyectil[i].clase = 0;

            if (clase == 0) // Proyectil
            {
                dx = (Nave_X + Nave_Desplazamiento) - x2;
                dy = Nave_Y - y;
                angulo = atan2(dy, dx);
                proyectil[i].angulo = angulo;
                //                proyectil[i].velocidad = (Tile_Medida16);
                proyectil[i].velocidad = Enemigo_Datos[num][0];
                proyectil[i].tiempo_fin = 200;

                if (IsSoundPlaying(s_proyectil)) StopSound(s_proyectil);

                if (num == 6) {
                    PlaySound(s_humanoide);
                }
                else {
                    PlaySound(s_proyectil);

                }



            }
            else { // Mina

                proyectil[i].x = x;
                proyectil[i].angulo = 0.0f;
                proyectil[i].velocidad = 0.0f;
                proyectil[i].clase = 1;
                proyectil[i].tiempo_fin = 1000;

            }




            break;
        }
    }

}


//----------------------------------------------------------
// Dibuja Disparos Enemigos
//----------------------------------------------------------
void Proyectil_Dibuja(Texture2D i_sprites, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_pod) {

    float x; // , x2;
    int x3;// , colx, colx2, coly, coly2;
    bool colision;

    // Colision con nave principal
    int colx = (Nave_X + Nave_Desplazamiento) - Nave_Medida3;
    int colx2 = (Nave_X + Nave_Desplazamiento) + Nave_Medida3;
    int coly = (Nave_Y)-Nave_Medida4;
    int coly2 = (Nave_Y)+Nave_Medida4;

    for (int i = 0; i < Proyectiles_Maximos; i++) {
        if (proyectil[i].activo) {

            if (proyectil[i].clase == 0) {

                //proyectil[i].velocidad=Tile_Medida16;

                if (Nave_Explota == 0) {
                    proyectil[i].x += cosf(proyectil[i].angulo) * proyectil[i].velocidad;
                    proyectil[i].y += +sinf(proyectil[i].angulo) * proyectil[i].velocidad;

                    proyectil[i].x -= Nave_Velocidad * Tile_Medida16;
                }
                Imagen_Dibuja(i_sprites, proyectil[i].x, proyectil[i].y, Medidas[3], Medidas[3], 128 + Frame, 12, 3, 3, 1, 0);
                if ((proyectil[i].x > (Pantalla_X2 + Tile_Medida2)) || (proyectil[i].y > (Pantalla_Alto + Tile_Medida2)) || (proyectil[i].x < (Pantalla_X - Tile_Medida2)) || (proyectil[i].y < (Marcador_Y - Tile_Medida2))) proyectil[i].activo = false;
                x3 = proyectil[i].x;
            }
            else {
                // Posicion Circular
                x = (proyectil[i].x - (Scroll)) * Tile_Medida16;
                if (x < 0) x += Scroll_Fin;
                Imagen_Dibuja(i_sprites, x, proyectil[i].y, Medidas[3], Medidas[3], 128 + Frame, 12, 3, 3, 1, 0);
                if ((x > (Pantalla_X2 + Tile_Medida2)) || (proyectil[i].y > (Pantalla_Alto + Tile_Medida2)) || (x < (Pantalla_X - Tile_Medida2)) || (proyectil[i].y < (Marcador_Y - Tile_Medida2))) proyectil[i].activo = false;
                x3 = x;
            }

            proyectil[i].tiempo += 1;
            if (proyectil[i].tiempo > proyectil[i].tiempo_fin) proyectil[i].activo = false;


            if (Nave_Explota == 0) {

                if (Nave_Invulnerable == 0) {
                    colision = ((x3 > colx) && (x3 < colx2) && (proyectil[i].y > coly) && (proyectil[i].y < coly2));
                    if (colision) {
                        Nave_Explota = 1;
                        Nave_Velocidad = 0;
                        Humanos_old = Humanoides_N;
                        Enemigos_Contabiliza(0, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
                        //if (Joystick == 1) VibrateEx(0.3f, 1.0f);

                        //    if (IsSoundPlaying(s_laser)) StopSound(s_laser);
                        //   if (IsSoundPlaying(s_explosion)) StopSound(s_explosion);
                        //PlaySound(s_bomba);
                    }
                }
            }
            //            if ((x > (Pantalla_X2 + Tile_Medida2)) || (proyectil[i].y > (Pantalla_Alto + Tile_Medida2)) || (x < (Pantalla_X - Tile_Medida2)) || (proyectil[i].y < (Marcador_Y - Tile_Medida2))) proyectil[i].activo = false;
            //            if ((x2 > (Pantalla_X2 + Tile_Medida2)) || (proyectil[i].y > (Pantalla_Alto + Tile_Medida2)) || (x2 < (Pantalla_X - Tile_Medida2)) || (proyectil[i].y < (Marcador_Y - Tile_Medida2))) proyectil[i].activo = false;

        }
    }
}


float GetRandomValueFloat(float min, float max) {
    int precision = 10000; // cuanto más grande, más decimales
    return (float)GetRandomValue(min * precision, max * precision) / precision;
}


void Swarmer_Crea_Enjambre(float x, float y) {
    int azar;

    azar = GetRandomValue(4, 6);
    for (int i = 0; i < azar; i++) {
        Enemigo_Crea(6, x, y);
    }
    Swarmer_Crea = 0;

}


void Menu_Dibuja(Font fuente, Texture2D i_sprites, Sound s_inicio, Sound s_comenzar, Sound s_humanoide) {

    int humX, humanos;
    int tCount, termina = 0;
    int valor;
    char textoPos[8];
    int kpos = 8;
    char kletra[2];
    int sepaJ = 0; // separacion joystick android

    switch (Menu) {

    case 1: // Oleada completada

#pragma region Attack Wave Completed

        Pantalla_Bomba = 0;
        ClearBackground(BLACK);

        // Marcadores
        Marcador_Dibuja(i_sprites, fuente);

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[0], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 77;
        DrawTextEx(fuente, textos[0], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Attack wave

        position.x = Pantalla_MitadX + Medida * 44;
        snprintf(Oleada_Texto, sizeof(Oleada_Texto), "%d", Oleada_Total);
        DrawTextEx(fuente, Oleada_Texto, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Nº de Oleada

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[1], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 93;
        DrawTextEx(fuente, textos[1], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Completed



        if (Humanoides_N > 0) {

            Humanoide_RellenaC += 1;
            if (Humanoide_RellenaC > 17) {
                Humanoide_RellenaC = 0;
                Humanoide_Rellena += 1;
                if (Humanoide_Rellena > Humanoides_N) {
                    Humanoide_Rellena = Humanoides_N;
                }
                else {
                    Puntua(fuente, 100);
                    PlaySound(s_humanoide);
                }

            }
            position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[2], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 141;
            DrawTextEx(fuente, textos[2], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Bonus x 100

            humX = position.x + Medidas[7];
            for (int i = 0; i < Humanoide_Rellena; i++) {
                Imagen_Dibuja(i_sprites, humX, (Medida * 160), Medidas[3], Medidas[8], 161, 241, 3, 8, 1, 0); // humanoides
                humX += Medidas[8];
            }
        }

        Menu_Oleada += 1;
        if (Menu_Oleada > 250) {
            Menu_Oleada = 0;
            Menu = 0;
            Oleada_Total += 1;
            Oleada += 1;
            //            OleadaC = 0;
            //            Juego_E = 0;
            humanos = Humanoides_N;
            Restaura_Variables();

            Juego_Mas_Dificil();

            if (Oleada == 5) {
                if (Mundo_Aniquilado == 1) {
                    Mundo_Destruido = 0;
                    Mundo_Aniquilado = 0;
                    humanos = 10;
                    Oleada = 1;
                }
            }
            //Puntua(fuente, (Humanoides_N * 100));
            Humanoide_Posiciona_Nivel(humanos); // Crea humanoides


        }
        break;

#pragma endregion

    case 2: // Menu Principal

        Pantalla_Bomba = 0;
        ClearBackground(BLACK);

#pragma region Menu Principal

        Pantalla_Bomba = 0;
        ClearBackground(BLACK);

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[14], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 77;
        DrawTextEx(fuente, textos[14], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts


        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[15], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 97;
        DrawTextEx(fuente, textos[15], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // PRESENTS

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[17], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 200;
        DrawTextEx(fuente, textos[17], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Press Play

        Imagen_Dibuja(i_sprites, Pantalla_MitadX, (Medida * 58), (102 * Medida), (21 * Medida), 0, 256, 102, 21, 1, 0); // Williams

        Imagen_Dibuja(i_sprites, Pantalla_MitadX, (Medida * 145), (138 * Medida), (26 * Medida), 384, 257, 128, 26, 1, 0); // Defender


        tCount = GetTouchPointCount();
        if (tCount > 0) { termina = 1; Joystick = 1; }
        //       break;

        //    case 2: // Joystick
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { termina = 1; Joystick = 2; } // X  - Acelerar
        if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) { termina = 1; Joystick = 2; } // [] - Cambia Direccion
        if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) { termina = 1; Joystick = 2; } // O - Dispara
        if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) { termina = 1; Joystick = 2; } // ^ - Bomba Inteligente
        //      break;

        //   default:
        if ((IsKeyPressed(KEY_SPACE)) || (IsKeyPressed(KEY_ENTER)) || (IsKeyPressed(KEY_LEFT_CONTROL))) { termina = 1; Joystick = 0; }
        //        break;


        if (termina) {
            Reinicia_Variables();
            Menu = 0;
            Humanoide_Posiciona_Nivel(10); // Crea humanoides
            if (IsSoundPlaying(s_inicio)) StopSound(s_inicio);
            PlaySound(s_comenzar);
            Puntos = 0;
            Puntua(fuente, 0);
        }

        Menu_Espera += 1;
        if (Menu_Espera > Menu_Espera_Fin) {
            Menu_Espera = 0;
            Menu = 4; // Tabla de Records
        }

        break;

#pragma endregion

    case 3: // Graba iniciales tabla de records

#pragma region Iniciales Records

        Pantalla_Bomba = 0;
        ClearBackground(BLACK);

        if (!Iniciales_Terminado)
        {
            if (IsKeyPressed(KEY_UP))
            {
                Letras[Iniciales_Seleccion] = (Letras[Iniciales_Seleccion] + 1) % 26;
            }
            if (IsKeyPressed(KEY_DOWN))
            {
                Letras[Iniciales_Seleccion] = (Letras[Iniciales_Seleccion] - 1 + 26) % 26;
            }
            if (IsKeyPressed(KEY_SPACE))
            {
                // Confirmar la letra actual y pasar a la siguiente
                Iniciales_Nombre[Iniciales_Seleccion] = 'A' + Letras[Iniciales_Seleccion];
                Iniciales_Seleccion++;
                if (Iniciales_Seleccion >= NUM_INICIALES)
                {
                    Iniciales_Nombre[NUM_INICIALES] = '\0'; // -> Texto de las 3 iniciales elegidas

                    Iniciales_Terminado = true;
                }
            }


            if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP)) Letras[Iniciales_Seleccion] = (Letras[Iniciales_Seleccion] + 1) % 26;;
            if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) Letras[Iniciales_Seleccion] = (Letras[Iniciales_Seleccion] - 1 + 26) % 26;;

            if ((IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) ||//; // X  - Acelerar
                (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) || //; // [] - Cambia Direccion
                (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) || // // O - Dispara
                (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP))) {
                // Confirmar la letra actual y pasar a la siguiente
                Iniciales_Nombre[Iniciales_Seleccion] = 'A' + Letras[Iniciales_Seleccion];
                Iniciales_Seleccion++;
                if (Iniciales_Seleccion >= NUM_INICIALES)
                {
                    Iniciales_Nombre[NUM_INICIALES] = '\0'; // -> Texto de las 3 iniciales elegidas

                    Iniciales_Terminado = true;
                }
            }
            // ^ - Bomba Inteligente


            // Android
            if (Joystick == 1) {

                sepaJ = -Tile_Medida2;
                position.y = Pantalla_Alto - Tile_Medida;
                position.x = Tile_Medida;
                Imagen_Dibuja(i_sprites, Pantalla_X, Pantalla_Alto - (Iniciales_Medida * 2), Pantalla_Medida, (2 * Iniciales_Medida), -0, 288, 208, 32, 0, 0); // a-z

                // InicialesX =
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Confirmar la letra actual y pasar a la siguiente
                    Iniciales_Nombre[Iniciales_Seleccion] = 'A' + InicialesX;//Letras[InicialesX];
                    Iniciales_Seleccion++;
                    if (Iniciales_Seleccion >= NUM_INICIALES)
                    {
                        Iniciales_Nombre[NUM_INICIALES] = '\0'; // -> Texto de las 3 iniciales elegidas

                        Iniciales_Terminado = true;
                    }
                    PlaySound(s_humanoide);
                }
            }



            if (Iniciales_Terminado) {
                Menu = 4; // Finaliza Menu
                PlaySound(s_inicio);
                // Comprueba posicion tabla de records
                for (int t = 0; t < 8; t++) {
                    if (Puntos > TR_Puntuacion[t] && kpos > t) {
                        kpos = t;
                    }
                }
                if (kpos == 7) { // ultima posicion
                    TR_Puntuacion[kpos] = Puntos;
                    TR_Nombres[kpos] = Iniciales_Nombre;
                }
                else { // desplaza
                    for (int d = 7; d > kpos; d--) {
                        TR_Puntuacion[d] = TR_Puntuacion[d - 1];
                        TR_Nombres[d] = TR_Nombres[d - 1];
                    }
                    TR_Puntuacion[kpos] = Puntos;
                    TR_Nombres[kpos] = Iniciales_Nombre;
                }
                // Aqui deberia grabar la tabla de records
                //Tabla_Records_Graba("record.dat");
            }
        }

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[18], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 53) + sepaJ;
        DrawTextEx(fuente, textos[18], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[19], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 85) + sepaJ;
        DrawTextEx(fuente, textos[19], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[20], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 95) + sepaJ;
        DrawTextEx(fuente, textos[20], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[21], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 115) + sepaJ;
        DrawTextEx(fuente, textos[21], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[22], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 135) + sepaJ;
        DrawTextEx(fuente, textos[22], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        //  position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[23], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = (Medida * 95) + sepaJ;
        //  DrawTextEx(fuente, textos[23], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts

        //---
        Imagen_Dibuja_Color(i_sprites, Pantalla_MitadX, (Medida * 180) + sepaJ, (44 * Medida), (2 * Medida), 209 - 256, 45, 44, 2, 1, 0, RED); // Subrayado

        position.y = (Medida * 169) + sepaJ;
        for (int i = 0; i < NUM_INICIALES; i++)
        {
            position.x = Iniciales_Posicion[i];
            if (i < Iniciales_Seleccion)
            {
                // Ya confirmada
                char letra[2] = { Iniciales_Nombre[i], '\0' };
                DrawTextEx(fuente, letra, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts
            }
            else if (i == Iniciales_Seleccion && !Iniciales_Terminado)
            {
                // La letra que se está eligiendo
                char letra[2] = { 'A' + Letras[i], '\0' };
                if (Joystick != 1) { // Todo menos android
                    DrawTextEx(fuente, letra, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // Electronic arts
                }

            }
        }




        break;

#pragma endregion

    case 4: // Tabla de Records

#pragma region Tabla de Records


        Pantalla_Bomba = 0;
        ClearBackground(BLACK);

        Imagen_Dibuja(i_sprites, Pantalla_MitadX, (Medida * 60), (138 * Medida), (26 * Medida), 384, 257, 128, 26, 1, 0); // Defender


        position.x = Pantalla_MitadX - MeasureTextEx(fuente, textos[3], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 76;
        DrawTextEx(fuente, textos[3], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // hall of fame

        position.x = Pantalla_MitadX - (Tile_Medida16 * 73) - MeasureTextEx(fuente, textos[4], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 101;
        DrawTextEx(fuente, textos[4], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // todays

        position.x = Pantalla_MitadX + (Tile_Medida16 * 73) - MeasureTextEx(fuente, textos[6], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 101;
        DrawTextEx(fuente, textos[6], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // all time

        position.x = Pantalla_MitadX - (Tile_Medida16 * 73) - MeasureTextEx(fuente, textos[5], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 111;
        DrawTextEx(fuente, textos[5], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // greated

        position.x = Pantalla_MitadX + (Tile_Medida16 * 73) - MeasureTextEx(fuente, textos[5], fuente.baseSize * Medida, Medida4).x / 2.0f; position.y = Medida * 111;
        DrawTextEx(fuente, textos[5], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // greated

        Imagen_Dibuja_Color(i_sprites, Pantalla_MitadX - (Tile_Medida16 * 73), (Medida * 122), (68 * Medida), (2 * Medida), -95, 29, 68, 2, 1, 0, RED); // Subrayado
        Imagen_Dibuja_Color(i_sprites, Pantalla_MitadX + (Tile_Medida16 * 73), (Medida * 122), (68 * Medida), (2 * Medida), -95, 29, 68, 2, 1, 0, RED); // Subrayado

        if (Puntos > 0) {
            position.x = (Marcador_X - Puntos_Ancho) - Puntos_X;
            position.y = Puntos_Y;
            DrawTextEx(fuente, Puntos_Texto, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]);
        }

        valor = (Medida * 131);
        position.y = valor;

        for (int i = 0; i < 8; i++) {

            position.x = Pantalla_MitadX - (Tile_Medida16 * 119);
            snprintf(textoPos, sizeof(textoPos), "%d", i + 1);
            DrawTextEx(fuente, textoPos, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // posicion

            position.x = Pantalla_MitadX + (Tile_Medida16 * 24);
            DrawTextEx(fuente, textoPos, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // posicion

            position.x = Pantalla_MitadX - (Tile_Medida16 * 108);
            DrawTextEx(fuente, TR_Nombres[i], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // iniciales

            position.x = Pantalla_MitadX + (Tile_Medida16 * 35);
            DrawTextEx(fuente, TR_Nombres[i], position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // iniciales

            snprintf(textoPos, sizeof(textoPos), "%d", TR_Puntuacion[i]);
            position.x = Pantalla_MitadX - (Tile_Medida16 * 29) - MeasureTextEx(fuente, textoPos, fuente.baseSize * Medida, Medida4).x;
            DrawTextEx(fuente, textoPos, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // puntuacion

            position.x = Pantalla_MitadX + (Tile_Medida16 * 115) - MeasureTextEx(fuente, textoPos, fuente.baseSize * Medida, Medida4).x;
            DrawTextEx(fuente, textoPos, position, fuente.baseSize * Medida, Medida4, Gradiente[Color_Gradiente]); // puntuacion

            position.y += (Medida * 10);
        }

        Menu_Espera += 1;
        if (Menu_Espera > Menu_Espera_Fin) {
            Menu_Espera = 0;
            Menu = 2; // Menu Principal
            Reinicia_Variables();
        }

        break;

#pragma endregion

    }

}


void Enemigos_Contabiliza(int modo, Sound s_implosion, Sound s_explosion, Sound s_laser, Sound s_bombardero, Sound s_pod) {

    // modo: 0-Guarda old  1-Genera old
    if (modo == 0) {
        old_lander = 0, old_mutante = 0, old_baiter = 0, old_pod = 0, old_bombardero = 0, old_swarmer = 0, old_oleadaC = 0;
        old_oleadaC = OleadaC;
        for (int e = 0; e < Enemigos_Maximos; e++) {
            if (enemigos[e].activo == true) {
                switch (enemigos[e].num) {
                case 1:
                    old_lander += 1;
                    break;
                case 2:
                    old_mutante += 1;
                    break;
                case 3:
                    old_baiter += 1;
                    break;
                case 4:
                    old_bombardero += 1;
                    break;
                case 5:
                    old_pod += 1;
                    break;
                case 6:
                    old_swarmer += 1;
                    break;
                }
            }
        }

    }
    else {

        // Crea Enemigos restantes cuando continua la partida
        OleadaC = old_oleadaC; // continua por donde estaba
        for (int e = 0; e < old_lander; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 1, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //  Enemigo_Crea( 1, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        for (int e = 0; e < old_mutante; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 2, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //   Enemigo_Crea(2, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        for (int e = 0; e < old_baiter; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 3, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //   Enemigo_Crea(3, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        for (int e = 0; e < old_bombardero; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 4, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //   Enemigo_Crea(4, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        for (int e = 0; e < old_pod; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 5, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //    Enemigo_Crea(5, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        for (int e = 0; e < old_swarmer; e++) {
            Explosion_Crea(GetRandomValue(1, 2060), GetRandomValue(Marcador_Y, Tile_Monte), 6, 1, s_implosion, s_explosion, s_laser, s_bombardero, s_pod);
            //    Enemigo_Crea(6, GetRandomValue(300, 1700), GetRandomValue(Marcador_Y, Tile_Monte));
        }
        // Enemigos_N = old_lander+ old_mutante+ old_baiter+ old_bombardero+ old_pod+ old_swarmer;
    }
}


void Juego_Mas_Dificil() {

    for (int i = 1; i < 7; i++) {

        Enemigo_Datos[i][0] *= 1.1; // Velocidad del disparo
        Enemigo_Datos[i][1] *= 1.1;// Velocidad de enemigo
        Enemigo_Datos[i][2] *= 1.1;// Velocidad de ascenso/descenso
        if (i != 4) { // distinto de bombardero
            Enemigo_Datos2[i][0] -= 10;// = 180; // Cadencia empieza
            if (Enemigo_Datos2[i][0] < 5) Enemigo_Datos2[i][0] = 5;
            Enemigo_Datos2[i][1] += 10; // Cadencia fin
        }
    }
    Baiter_Tiempo_Fin * 0.9;
    if (Baiter_Tiempo_Fin < 200) Baiter_Tiempo_Fin = 200;
}






#pragma endregion


