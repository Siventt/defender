// capturas mame: 292 x 240 pixeles mame , mostrado en pantalla a 320x240 ¡Importante

// escenario montañas  2064 x 89 pixeles (No usado)

// marcador radar 128 x 32 pixeles

// Tileado:  129 tiles de largo, 16x16 pixeles cada uno. total 129*16 = 2064;
// Scroll: 0 - 2064;

// Enemigos: el enemigo se mueve en un rectangulo: x: 0 - 2064    y:ejm 0 - Pantalla_Alto
// los enemigos si suben hacia arriba y llegan a la posicion y=0 aparecen por debajo y=Pantalla_Alto

// Observaciones:

// Tareas:

// [   ] - Menu principal
// [   ] - Tabla de Records
// [   ] - Info enemigos
// [ x ] - Estructura para Enemigos
// [ x ] - Estructura para Humanoides
// [ x ] - Estructura para Explosiones e Implosiones
// [   ] - Estructura para disparos enemigos
// [ x ] - Estructura para lasers nave principal
// [ x ] - Rutina para crear humanoides estrategicamente
// [ x ] - La nave lleva enganchado a un humanoide, aterriza con humanoide
// [   ] - Bombas
// [   ] - Hiper Espacio
// [   ] - Explosion nave principal
// [   ] - Graficos propulsion
// [   ] -


// Cuando toca abducir: (Si Enemigo_Abductor==0 y Humanoide_Abducido==0)
// Si hay humanoides en reposo:
//     - Comprueba si hay landers
//     - Selecciona el lander que va a abducir. Se estable en Enemigo_Abductor su id unico
//     - Si el lander muere y su id es el mismo que Enemigo_Abductor, entonces libera Enemigo_Abductor=0
//
//  Selecciona un humanoide en reposo y se establece Humanoide_Abducido en su id unico.
// - Si el humanoide se convierte en mutante entonces libera Humanoide_Abducido=0
// - estado 10 del Humanoide_abducido queda inmovil en pantalla y no camina.
// Si el humanoide_abducido muere libera Humanoide_Abducido=0 y libera Enemigo_Abductor=0


/*

Inicio oleada → Spawn Landers/Bombers/Pods
↓
Jugador protege humanos
↓
Si humano secuestrado → Mutante
↓
Si todos humanos mueren → Planeta destruido → Landers vivos → Mutantes
↓
Cada 5 oleadas → humanos restaurados, dificultad incrementa
↓
Baiters aparecen según tiempo
↓
Fin de oleada → wave++

*/


/*

Oleadas (Waves):

Oleada	Landers	 Bombers	Pods	Observaciones
1	      15	    0	     0	    Solo Landers, velocidad baja
2	      20	    3	     1	    Introducción de Pods y Bombers
3	      20	    4	     3	    Más enemigos y Swarmers al destruir Pods
≥4	      20	    5	     4	    Plantilla estándar; dificultad creciente



Baiter:
//-----------
     Muy rápido y directo. A<
     parecen si el jugador tarda en eliminar la oleada.

Pod:
//-----------
     Movimiento lento y recto horizontal o diagonal.
     No persigue al jugador activamente.
     Se mueve en zigzag leve o línea recta dependiendo de la aleatoriedad del spawn.
     Comportamiento especial:
     Al destruirse → genera 4–6 Swarmers en distintas direcciones.

Bomber:
//-----------
     Se desplazan horizontalmente a velocidad moderada.
     Mantienen altura constante o descienden lentamente hacia el suelo.
     Su función principal es soltar minas flotantes que permanecen estáticas en pantalla.
     Comportamiento especial:
     No atacan directamente al jugador, solo crean minas que obstaculizan.

Swarmers
//-----------
     Liberados al destruir Pods.
     Patrón de movimiento:
     Muy rápido y errático.
     Movimiento diagonal y oscilante, cambia dirección aleatoriamente.
     Persiguen al jugador solo de manera indirecta, el objetivo es llenar pantalla



Progresión de dificultad:

    Cada 5 oleadas:

        * Velocidad de enemigos aumenta (+0.1 por ciclo de 5).

        * Agresividad de Mutantes aumenta (+0.1 por ciclo).

        * Baiters aparecen antes (mínimo 1 s).

        * Swarmers y Pods se vuelven más veloces.

        * El juego no tiene final, solo termina al perder todas las vidas.

*/