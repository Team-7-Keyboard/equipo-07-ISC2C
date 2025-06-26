// Equipo 7
// ISC
// 2°C
// Adriana Chiquito Díaz de León 341166
// Ejercicio 6: Scrabble
// 24-04-2025

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>


#define CASILLA 60 // Dimensiones para una casilla
#define TAM 9 // Tamaño del tablero TAM*TAM
#define TAM_DICCIONARIO 200
#define _CRT_SECURE_NO_WARNINGS

const char *diccionario[TAM_DICCIONARIO] = {
    "CASA", "PERRO", "GATO", "RATON", "JUEGO",
    "TECLADO", "PANTALLA", "PROGRAMA", "COMPUTAR", "ALUMNO",
    "CLASE", "BOTELLA", "CAMINAR", "SALTAR", "COMER",
    "BEBER", "CORRER", "FELIZ", "MUSICA", "NARANJA",
    "CORRER", "DORMIR", "HERMANO", "HERMANA", "AMIGO",
    "AMIGA", "LIBRO", "ESCUELA", "CIENCIA", "PROYECTO",
    "CELULAR", "TECLADO", "PANTALLA", "RATONES", "AUDIFONO",
    "TELEFONO", "COCINA", "CUIDADO", "PINTURA", "PALABRA",
    "ESCRIBIR", "DIBUJAR", "ESPEJO", "PAREDES", "VENTANA",
    "PUERTA", "ALMOHADA", "TORMENTA", "AVION", "CAMION",
    "BARCO", "PLANETA", "GALAXIA", "UNIVERSO", "ESTRELLA",
    "COMETA", "NUBE", "LLUVIA", "RELAMPAGO", "TRUENO",
    "SOL", "LUNA", "MARTE", "VENUS", "TIERRA",
    "JARDIN", "BOSQUE", "MONTAÑA", "DESIERTO", "PLAYA",
    "CARACOL", "TORTUGA", "LEON", "TIGRE", "ELEFANTE",
    "JIRAFA", "ABEJA", "MARIPOSA", "PEZ", "PULPO",
    "MANZANA", "PLATANO", "PERA", "LIMON", "MELON",
    "SANDIA", "UVA", "CEREZA", "TOMATE", "LECHUGA",
    "ZANAHORIA", "PEPINO", "PESCADO", "POLLO", "CARNE",
    "QUESO", "PAN", "TORTILLA", "SOPA", "POSTRE",
    "ESFERA", "SOMBRA", "LUZ", "CAMARA", "ACCION",
    "POEMA", "MESA", "SILLA", "LAPIZ", "PLUMA",
    "HOJA", "CUADERNO", "BOLSA", "MOCHILA", "ZAPATO",
    "CALCETIN", "CAMISA", "PANTALON", "CHAMARRA", "SUDADERA",
    "RELOJ", "CARTERA", "LLAVE", "CARGADOR", "PLANTA",
    "BROCHA", "PEINE", "ESPEJO", "LINTERNA", "LAMPARA",
    "CABLE", "BATERIA", "RADIO", "LIBRE", "ANTENA",
    "PIZARRA", "MARCADOR", "BORRADOR", "CINTA", "TIJERA",
    "GOMA", "CUADRO", "RETRATO", "PUENTE", "CALLE",
    "SEMAFORO", "TREN", "METRO", "COCHE", "LANCHA",
    "PILOTO", "ABAJO", "COHETE", "SATELITE", "RAYO",
    "CRATER", "VOLCAN", "GLACIAR", "ISLA", "OCÉANO",
    "RÍO", "LAGO", "CASCADA", "ZANJA", "CUEVA",
    "PRADERA", "GRANJA", "CIUDAD", "PUEBLO", "VILLA",
    "CASTILLO", "IGLESIA", "TEMPLO", "MERCADO", "TIENDA",
    "BANCO", "HOSPITAL", "ESQUINA", "PLAZA", "PARQUE",
    "ESTADIO", "TEATRO", "CINE", "MUSEO", "ZOO",
    "OFICINA", "TRABAJO", "TALLER", "FABRICA", "ALMACEN",
    "COMIDA", "CAFE", "HELADO", "PLATO", "FLORERIA",
    "PAPEL", "FARMACIA", "TIANGUIS", "GIMNASIO", "ESTACION"
};

enum Jugador {PRIMERO, SEGUNDO}; // Turno del jugador
enum Orientacion {ARRIBA, DERECHA, ABAJO, IZQUIERDA}; // Determina hacia que direccion en el tablero va a escribirse la palabra
struct Juego{ // Puntaje de cada jugador
    struct Puntaje *palabras_acumuladas; // Palabras de cada jugador 
    int puntaje_total; // Puntaje total de cada jugador
};

struct Puntaje{
    char *word; // Palabra
    int puntos; // Puntos por dicha palabra
    struct Puntaje *next;
};

void agregarPuntos(struct Puntaje **inicio, char *palabra, int puntos); // Agregamos la palabra y su puntaje a la lista enlazada del jugador correspondiente
bool estaVacio (char tablero[TAM][TAM]); // Para conocer si el tablero esta vacio o no
bool colocarPalabra (char tablero[][TAM], int x, int y, enum Orientacion o, char *palabra, int *puntos); // Decide y agrega la palabra a la variable tablero
void mostrarTablero(char tablero[TAM][TAM], ALLEGRO_FONT *texto, int columna, int fila, char *palabra, enum Orientacion o, enum Jugador player); // Muestra los cambios hechos en el tablero y acciones del usuario
void quitarPalabra(char tablero[TAM][TAM], int fila_inicial, int columna_inicial, enum Orientacion o, int fila_actual, int columna_actual, int posiciones[TAM][2], char *letras_repetidas, int num_repeticion);
bool esPalabra(char *palabra); // Si la palabra esta en el diccionario del juego
void guardarPalabra(char *palabra); // Guarda el tablero y el puntaje en unn archivo de texto
void guardarPartida(struct Juego partida[2]); // Juardamos las palabras y puntos de cada jugador en un .txt
void liberar(struct Puntaje **inicio); // Liberamos la memoria reservada de las estructuras
void convertirPalabra(char *point, int *puntos, int divisor, int *k);
bool doblePalabra(char *palabra);
void mostrarMarcador(struct Juego partida[2], ALLEGRO_FONT *texto);
void mostrarInstrucciones(ALLEGRO_EVENT_QUEUE *evento);

// Funciones para saber si es una casilla especial
bool esTriplePalabra(int y, int x);
bool esDoblePalabra(int y, int x);
bool esTripleLetra(int y, int x);

int main(){
    if (!al_init() || !al_install_mouse() || !al_install_keyboard() || !al_init_primitives_addon() || !al_init_ttf_addon()) { // Iniciamos allegro, mouse, teclado, figuras, archivos tff
        return -1;
    }
    FILE *archivo = fopen("Scrabble_Score.txt", "w"); // Limpia el archivo
    if(!archivo){
        return -1;
    }
    fclose(archivo);
// DECLARACION  E INICIALIZACION DE LAS VARIABLES PRINCIPALES
    char tablero[TAM][TAM]; // Tablero
    for(int i=0; i<TAM; i++){ // Inicializamos todas las posiciones en '\0'
        for(int j=0; j<TAM; j++){
            tablero[i][j] = '\0'; 
        }
    }

    int columna = 5, fila = 5; // Posiciones en el tablero(x,y)(fila,columna)
    int puntos = 0; // Puntos por palabra
    enum Orientacion o = DERECHA; // Declaramos una variable enumeracion para guardar la orientacion
    enum Jugador player = PRIMERO; // Turno del jugador
    struct Juego partida[2];
    char *point = (char*)calloc(30,sizeof(char)); // Para la funcion convertirPalabra
    int k = 0, score = 0; // La usamos para la funcion convertiPalabra
    // Inicializamos el marcador en cero
    partida[0].palabras_acumuladas = NULL;
    partida[1].palabras_acumuladas = NULL;
    partida[0].puntaje_total = 0;
    partida[1].puntaje_total = 0;

    bool run = true, escribir = false; // run para que el programa cntinue y escribir para activar el modo escritura
    char *palabra = (char *) calloc (TAM*TAM , sizeof(char)); // Palabra que recibmos del usuario

    ALLEGRO_DISPLAY *display = al_create_display(CASILLA * TAM, CASILLA*TAM); // Crea una ventana del tamaño del tablero
    ALLEGRO_EVENT_QUEUE *evento = al_create_event_queue(); // Crea una cola donde se almacenan los eventos
    ALLEGRO_FONT *texto = al_load_ttf_font("frisk.ttf", 30, 0); // Crea una fuente de texto
    ALLEGRO_EVENT ev; // Recibe los eventos
    if(!display || !evento || !texto){ // Iniciamos las variables ALLEGRO
        printf("Error al cargar un archivo.\nEs posible que requiera el archivo frisk.ttf para ejecutar el programa.\n");
        return -1;
    }

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA); // Para la transparencia de la ventana "no es palabra"
    al_register_event_source(evento, al_get_keyboard_event_source());
    al_register_event_source(evento, al_get_mouse_event_source());
    al_register_event_source(evento, al_get_display_event_source(display));
    mostrarTablero(tablero, texto, columna, fila, palabra, o, player);
    mostrarInstrucciones(evento);
    al_clear_to_color(al_map_rgb(0,0,0));
    while(run){ // Avanza hasta que un evento cambie la variable run
        mostrarTablero(tablero, texto, columna, fila, palabra, o, player);
        while(al_get_next_event(evento, &ev)){ // Saca los eventos guardados en la cola de eventos
            switch(ev.type){
                case ALLEGRO_EVENT_DISPLAY_CLOSE: 
                        run = false; // Nos vamos
                        al_flush_event_queue(evento);
                    break;

                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: 
                        if(!escribir){ // No puede acceder a otras casillas si el modo escribir esta activo
                            if(!estaVacio(tablero) || (estaVacio(tablero) && ev.mouse.x/CASILLA == TAM/2 && ev.mouse.y/CASILLA == TAM/2)){
                                columna = ev.mouse.x/CASILLA; fila = ev.mouse.y/CASILLA; // Posicion donde el usuario dio click
                                escribir = true; // Modo escribir activado
                            }
                        }
                    break;
                
                case ALLEGRO_EVENT_KEY_DOWN: // teclas de control
                            if(escribir){ 
                                if(*palabra == '\0' || *palabra != '\0'){ // Solo puedes cambiar de direccion cuando la palabra no tiene o tiene una sola letra
                                    switch(ev.keyboard.keycode){
                                        case ALLEGRO_KEY_UP: o = ARRIBA; break;

                                        case ALLEGRO_KEY_DOWN: o = ABAJO; break;

                                        case ALLEGRO_KEY_LEFT: o = IZQUIERDA; break;

                                        case ALLEGRO_KEY_RIGHT: o = DERECHA; break;
                                    }
                                }
                            }
                    break;

                // LLENAMOS "palabra"
                case ALLEGRO_EVENT_KEY_CHAR: // letras
                        if(escribir){
                            if(ev.keyboard.unichar == '\b'){
                                palabra[strlen(palabra)-1] = '\0'; // Borramos
                                al_flush_event_queue(evento);
                                break;
                            }

                            if(ev.keyboard.unichar == '\n' || ev.keyboard.unichar == '\r'){ // Cuando el usuario presione enter, significa que termino de escribir
                                if(colocarPalabra(tablero, columna, fila, o, palabra, &puntos)){ // Colocamos la palabra
                                    guardarPalabra(palabra);
                                    *point = '\0';
                                    k = 0;
                                    score = puntos;
                                    point[k++] = '+';
                                    convertirPalabra(point, &score, 1, &k);
                                    for(int i=0; i<10; i++){
                                        mostrarTablero(tablero, texto, columna, fila, palabra, o, player);
                                        al_draw_text(texto, al_map_rgb(255,255,255), 5*CASILLA, (5*CASILLA) - (i*10), 0, point);
                                        al_flip_display();
                                        al_rest(0.1);
                                    }
                                    
                                    switch(player){ // cambiamos de turno y agregamos la palabra y sus puntos al jugador correspondiente
                                        case PRIMERO: 
                                                player = SEGUNDO;
                                                if(partida[0].palabras_acumuladas == NULL){ // Si es su primer turno creamos la lista enlazada
                                                    partida[0].palabras_acumuladas = (struct Puntaje*)malloc(sizeof(struct Puntaje));
                                                    partida[0].palabras_acumuladas->word = (char*)malloc(sizeof(strlen(palabra)+1));
                                                    strcpy(partida[0].palabras_acumuladas->word, palabra);
                                                    partida[0].palabras_acumuladas->puntos = puntos;
                                                    partida[0].palabras_acumuladas->next = NULL;
                                                    partida[0].puntaje_total = puntos;
                                                }else{ // Agregamos a la lista enlazada
                                                    agregarPuntos(&partida[0].palabras_acumuladas, palabra, puntos);
                                                    partida[0].puntaje_total += puntos;
                                                }
                                            break;
                                        case SEGUNDO: 
                                                player = PRIMERO;
                                                if(partida[1].palabras_acumuladas == NULL){ // Si es su primer turno creamos la lista enlazada
                                                    partida[1].palabras_acumuladas = (struct Puntaje*)malloc(sizeof(struct Puntaje));
                                                    partida[1].palabras_acumuladas->word = (char*)malloc(sizeof(strlen(palabra)+1));
                                                    strcpy(partida[1].palabras_acumuladas->word, palabra);
                                                    partida[1].palabras_acumuladas->puntos = puntos;
                                                    partida[1].palabras_acumuladas->next = NULL;
                                                    partida[1].puntaje_total = puntos;
                                                }else{
                                                    agregarPuntos(&partida[1].palabras_acumuladas, palabra, puntos); // Agregamos a la lista enlazada
                                                    partida[1].puntaje_total += puntos;
                                                }
                                            break;
                                    }
                                }
            
                                for(int i=0; i<TAM; i++){ // Vaciamos la palabra
                                    palabra[i] = '\0';
                                }
                                puntos = 0; // Reiniciamos el conteo
                                escribir = false; // Salimos del modo escritura
                            }else{ // Agregamos una letra dada por el usuario 

                                if(ev.keyboard.unichar >= 97 && ev.keyboard.unichar <= 122){
                                    ev.keyboard.unichar -= 32;
                                }
                                palabra[strlen(palabra)] = ev.keyboard.unichar;
                                mostrarTablero(tablero, texto, columna, fila, palabra, o, player); // Mostramos "palabra" actualizada en el tablero
                            }
                        }
                    break;
            }
        }
    }

    // Mostramos el marcador
    mostrarMarcador(partida, texto);

    //Guardamos la partida
    guardarPartida(partida); // Guardamos los reusltados en un archivo .txt
    
    // Liberamos los punteros
    free(palabra);
    free(point);
    liberar(&partida[0].palabras_acumuladas); // Liberamos la memoria reservada
    liberar(&partida[1].palabras_acumuladas); // Liberamos la memoria reservada
    al_uninstall_keyboard();
    al_destroy_display(display);
    al_destroy_event_queue(evento);
    al_destroy_font(texto);
    return 0;
}

bool estaVacio (char tablero[TAM][TAM]){ 
    for(int i=0; i<TAM; i++){
        for(int j=0; j<TAM; j++){
            if(tablero[i][j] != '\0'){
                return false; // Devuelve false si encontro una letra en una casilla
            }
        }
    }
    return true; // Devuelve true si no encontro ninguna letra
}

bool colocarPalabra (char tablero[TAM][TAM], int x, int y, enum Orientacion o, char *palabra, int *puntos){
    int columna = x, fila = y, k = 0, num_repeticion = -1; // Para recorrer posiciones (columna,fila), para sacar las letras de "palabra"(k), para guardar las letras repetidas(num_repeticion)
    bool letra_encontrada=false; // Saber si al menos una letra coincide
    int triple = 1, doble = 1; // Multiplicador de casillas especiales
    struct Misma_Letra{ // Para guardar las posiciones y la letra repetida
        int posiciones[TAM][2];
        char *letras_repetidas;
    }misma_letra;
    misma_letra.letras_repetidas = (char*)calloc(TAM, sizeof(char));

    // Palabra Repetida
    if(doblePalabra(palabra)){
        return false; // Palabra ya usada
    }

    // Palabra Existe en el Diccionario
    if(!esPalabra(palabra)){
        ALLEGRO_FONT *no_es_Palabra = al_create_builtin_font(); // Llamamos al tipo de texto de ALLEGRO
        int x_texto = (TAM-5)/2*CASILLA, y_texto = (TAM-3)/2*CASILLA; // Calculamos las posiciones (x,y) iniciales para dibujar un rectangulo centrado
        al_draw_filled_rectangle(x_texto, y_texto, x_texto+(5*CASILLA), y_texto+(3*CASILLA), al_map_rgba(39,30,22, 160)); // Rectangulo centrado
        al_draw_text(no_es_Palabra, al_map_rgb(255,255,255), x_texto+(1.5*CASILLA), y_texto+(1.5*CASILLA), 0, "Palabra no Valida."); // Mensaje centrado
        al_flip_display();
        al_rest(1.0);
        al_destroy_font(no_es_Palabra);
        return false; // La palabra no esta en el diccionario del juego
    }

    // Fuera del tablero
    int len = strlen(palabra);
    switch(o){ // Devuelve false si la palabra se sale del tablero o se confunde con otra palabra (que empiece al lado de una palabra ya colocada)
        case ABAJO: if(TAM-fila < len || tablero[fila-1][columna] != '\0'){return false;}; break;
        case ARRIBA: if(fila+1 < len || tablero[fila+1][columna] != '\0'){return false;}; break;
        case DERECHA: if(TAM-columna < len || tablero[fila][columna-1] != '\0'){return false;}; break;
        case IZQUIERDA: if(columna+1 < len || tablero[fila][columna+1] != '\0'){return false;}; break;
    }
    
    if(estaVacio(tablero)){
        letra_encontrada = true; // Primera palabra del tablero
    }

    // AGREGAMOS LETRA POR LETRA AL TABLERO
    while(palabra[k] != '\0'){

    // Palabras Superpuestas
        if(tablero[fila][columna] != '\0' && palabra[k] != tablero[fila][columna]){
            if(fila != y || columna != x){
                quitarPalabra(tablero, y, x, o, fila, columna, misma_letra.posiciones, misma_letra.letras_repetidas, num_repeticion); // Removemos las letras ya agregadas
            }
            free(misma_letra.letras_repetidas);
            return false; // Letra superpuesta
        }

    // Letras que coinciden
        if(palabra[k] == tablero[fila][columna]){
            num_repeticion++;
            letra_encontrada = true; // Existe al menos una letra de la palabra que coincide en el tablero
            misma_letra.posiciones[num_repeticion][0] = fila; 
            misma_letra.posiciones[num_repeticion][1] = columna;
            misma_letra.letras_repetidas[num_repeticion] = tablero[fila][columna];
        }

    // Agregamos la letra en "palabra" a la casilla actual(fila, columna) en el tablero
        tablero[fila][columna] = palabra[k++];

    //AGREGAMOS LOS PUNTOS A LAS CASILLAS ESPECIALES
        // triple_palabra = {0,0}, {8,0}, {0,8}, {8,8}
        // doble_palabra = {2,2}, {6,2}, {2,6}, {6,6}
        // triple_letra = {1,4}, {4,1}, {7,4}, {4,7}
        if(esTriplePalabra(fila, columna)){
            triple *= 3;
        }else {
            if(esDoblePalabra(fila, columna)){
                doble *= 2;
            }else{
                if(esTripleLetra(fila, columna)){
                    (*puntos) += 2;
                }
            }
        }
        (*puntos)++;

        switch(o){ // Nos movemos a la siguiente casilla, dependiendo de la orintacion
            case ARRIBA: fila--; break;
            case ABAJO: fila++; break;
            case IZQUIERDA: columna--; break;
            case DERECHA: columna++; break;
        }
    }

    if(!letra_encontrada){ //No hay letras que coincidan
        quitarPalabra(tablero, y, x, o, fila, columna, misma_letra.posiciones, misma_letra.letras_repetidas, num_repeticion); // Removemos las letras ya agregadas
        free(misma_letra.letras_repetidas);
        return false; // No coincide con ninguna palabra del tablero
    }

    (*puntos) *= (triple * doble);

    free(misma_letra.letras_repetidas);
    return true; // La palabra cumplio los reuisitos, fue agregada correctamente
}

void quitarPalabra(char tablero[TAM][TAM], int fila_inicial, int columna_inicial, enum Orientacion o, int fila_actual, int columna_actual, int posiciones[TAM][2], char *letras_repetidas, int num_repeticion){
    while(fila_actual != fila_inicial || columna_actual != columna_inicial){ // Hasta que hayamos borrado todas las palabras agregadas
        switch(o){ // Retrocedemos posiciones en el tablero
            case ARRIBA: fila_actual++; break;
            case ABAJO: fila_actual--; break;
            case IZQUIERDA: columna_actual++; break;
            case DERECHA: columna_actual--; break;
        }
        tablero[fila_actual][columna_actual] = '\0'; // Vaciamos

        if(fila_actual == posiciones[num_repeticion][0] && columna_actual == posiciones[num_repeticion][1]){ // Respetamos las letras que ya estaban antes de colocar la palabra
            tablero[fila_actual][columna_actual] = letras_repetidas[num_repeticion];
            num_repeticion--;
        }
    }
}

void mostrarTablero(char tablero[TAM][TAM], ALLEGRO_FONT *texto, int columna, int fila, char *palabra, enum Orientacion o, enum Jugador player){
    ALLEGRO_COLOR color, color2;
    // Tablero vacio
    for(int i=0; i<TAM; i++){
        for(int j=0; j<TAM; j++){
            if((j+i)%2 == 0){
                switch(player){
                    case PRIMERO: color = al_map_rgb(100, 90, 81); color2 = al_map_rgb(60, 60, 50); break;
                    case SEGUNDO: color = al_map_rgb(132, 144, 153); color2 = al_map_rgb(70, 80, 87); break;
                }
            }else{
                switch(player){
                    case PRIMERO: color = al_map_rgb(180, 160, 140); color2 = al_map_rgb(150, 130, 110); break;
                    case SEGUNDO: color = al_map_rgb(185, 201, 207); color2 = al_map_rgb(101, 124, 142); break;
                }
            }
            al_draw_filled_rounded_rectangle(j*CASILLA, i*CASILLA, (j+1)*CASILLA, (i+1)*CASILLA, 10, 10, color);

            if(tablero[i][j] != '\0'){
                al_draw_filled_rounded_rectangle(j*CASILLA, i*CASILLA, (j+1)*CASILLA, (i+1)*CASILLA, 10, 10, color2);
            }

        //AGREGAMOS LAS CASILLAS ESPECIALES
            // triple_palabra = {0,0}, {8,0}, {0,8}, {8,8}
            // doble_palabra = {2,2}, {6,2}, {2,6}, {6,6}
            // triple_letra = {1,4}, {4,1}, {7,4}, {4,7}
            if(esTriplePalabra(i, j)){
                al_draw_filled_rounded_rectangle(j*CASILLA, i*CASILLA, (j+1)*CASILLA, (i+1)*CASILLA, 10, 10, al_map_rgb(40,40,40));
            }else {
                if(esDoblePalabra(i, j)){
                    al_draw_filled_rounded_rectangle(j*CASILLA, i*CASILLA, (j+1)*CASILLA, (i+1)*CASILLA, 10, 10, al_map_rgb(80,50,130));
                }else{
                    if(esTripleLetra(i, j)){
                        al_draw_filled_rounded_rectangle(j*CASILLA, i*CASILLA, (j+1)*CASILLA, (i+1)*CASILLA, 10, 10, al_map_rgb(50,130,80));
                    }
                }
            }

        }
    }
    
    // Contenido de la variable tablero
    for(int i=0; i<TAM; i++){
        for(int j=0; j<TAM; j++){
            if(tablero[i][j] == '\0'){
            //AGREGAMOS LAS CASILLAS ESPECIALES
                // triple_palabra = {0,0}, {8,0}, {0,8}, {8,8} x3
                // doble_palabra = {2,2}, {6,2}, {2,6}, {6,6} x2
                // triple_letra = {1,4}, {4,1}, {7,4}, {4,7} +2
                if(esTriplePalabra(i, j)){
                    al_draw_text(texto, al_map_rgb(80, 80, 80), (j*CASILLA)+15, (i*CASILLA)+10, 0, "x3");
                }else {
                    if(esDoblePalabra(i, j)){
                        al_draw_text(texto, al_map_rgb(120, 90, 170), (j*CASILLA)+15, (i*CASILLA)+10, 0, "x2");
                    }else{
                        if(esTripleLetra(i, j)){
                            al_draw_text(texto, al_map_rgb(90, 170, 120), (j*CASILLA)+15, (i*CASILLA)+10, 0, "+2");
                        }
                    }
                }
            }
            char letra[2];
            letra[0] = tablero[i][j];
            letra[1] = '\0';
            if(letra[0] >= 97 && letra[0] <= 122){
                letra[0] -= 32;
            }
            al_draw_text(texto, al_map_rgb(255, 255, 255), (j*CASILLA)+20, (i*CASILLA)+10, 0, letra);
        }
    }

    int k = 0;
    // Supermonemos el contenido de la variable palabra, usando la orientacion y posiciones que el usuario nos indico
    while(palabra[k] != '\0'){
        char letra[2];
        letra[0] = palabra[k++];
        letra[1] = '\0';
        al_draw_text(texto, al_map_rgb(255, 255, 255), (columna*CASILLA)+20, (fila*CASILLA)+10, 0, letra);
        switch(o){
            case DERECHA: columna++; break;
            case IZQUIERDA: columna--; break;
            case ABAJO: fila++; break;
            case ARRIBA: fila--; break;
        }
    }

    al_flip_display();
}

bool esPalabra(char *palabra){
    for(int i=0; i<TAM_DICCIONARIO; i++){ // Recorre todo el diccionario
        if(strcmp(palabra, diccionario[i]) == 0){ // Las palabras son iguales
            return true; // Palabra encontrada
        }
    }
    return false;
}

void guardarPalabra(char *palabra){ // Guardamos palabras para verificar que no se repitan
    FILE *archivo = fopen("Scrabble_Score.txt", "a");
    if(!archivo){
        return;
    }
    fprintf(archivo, "%s\t%d\n", palabra, strlen(palabra));
    fclose(archivo);
}

void agregarPuntos(struct Puntaje **inicio, char *palabra, int puntos){ // Agregamos los datos a la lista enlazada
    struct Puntaje *nuevo = (struct Puntaje *)malloc(sizeof(struct Puntaje));
    nuevo->word = (char*)malloc(sizeof(strlen(palabra)+1)); 
    strcpy(nuevo->word, palabra); // Agregamos la palabra dada por el jugador su respectiva lista enlazada
    nuevo->puntos = puntos; // Agregamos los puntos de la palabra indibidual
    nuevo->next = *inicio; // Enlazamos a la lista 
    *inicio = nuevo; // Recorremos el primer elemento
}

void guardarPartida(struct Juego partida[2]){ // Guardamos los resultados de la partida en un archivo .txt
    FILE *archivo = fopen("Scrabble_Score.txt", "w"); // Sobreescribimos el archivo utilizado para guardar las palabras
    if(!archivo){
        return;
    }
    for(int i=0; i<2; i++){ // Primero grabamos la partida del jugador 1 y despues la del jugador 2
        fprintf(archivo, "JUGADOR %d:\n", i+1);
        if(partida[i].palabras_acumuladas == NULL){
            fprintf(archivo, "Total: 0 puntos\n\n");
        }else{
            struct Puntaje *aux = partida[i].palabras_acumuladas;
            while(aux != NULL){ // Recorremos la lista enlazada
                fprintf(archivo, " %s %d\n", aux->word, aux->puntos); // Grabamos los datos de cada elemento
                aux = aux->next; // Pasamos al siguiente elemento
            }
            fprintf(archivo, "Total: %d puntos\n\n", partida[i].puntaje_total);
        }
        
    }
    fclose(archivo); // Cerramos el archivo
}

void liberar(struct Puntaje **inicio){ // Liberamos memoria reservada
    struct Puntaje *aux;
    while(*inicio != NULL){
        aux = *inicio;
        *inicio = (*inicio)->next;
        free(aux->word);
        free(aux);
    }
    *inicio = NULL;
}

bool esTriplePalabra(int y, int x){
    // Si las coordenadas son (0,8), (0,0), (8,0) o (8,8)
    if(x == 0 || x == 8){
        if(y == 0 || y == 8){
            return true;
        }
    }
    return false;
}

bool esDoblePalabra(int y, int x){
    // Si las coordenadas son (2,2), (2,6), (6,2) o (6,6)
    if(x == 2 || x == 6){
        if(y == 2 || y == 6){
            return true;
        }
    }
    return false;
}

bool esTripleLetra(int y, int x){
    // Si las coordenadas son (4,1), (4,7), (1,4) o (7,4)
    if(x == 4 && (y == 1 || y == 7)){
        return true;
    }
    if(y == 4 && (x == 1 || x == 7)){
        return true;
    }
    return false;
}

void convertirPalabra(char *point, int *puntos, int divisor, int *k){ // Convierte el numero en una variable char*, sin importar la extension
    if((*puntos)/divisor >= 10){ // Para convertir unidad por unidad
        convertirPalabra(point, puntos, divisor*10, k); // Quiere decir que el numero es diez veces mas grande
    }
    int num = (*puntos)/divisor;
    point[(*k)++] = num + '0'; // Agregamos unidad por unidad
    (*puntos) -= (num*divisor);
    if(divisor == 1) {
        point[*k] = '\0';
    }
}

bool doblePalabra(char *palabra){
        FILE *archivo = fopen("Scrabble_Score.txt", "r");
        if(!archivo){
            return true;
        }
        char *arch_palabra = (char*)calloc(TAM, sizeof(char));
        while(fscanf(archivo, "%s", arch_palabra) == 1){
            if(strcmp(arch_palabra, palabra) == 0){
                fclose(archivo);
                free(arch_palabra);
                return true; // Palabra ya usada
            }
        }
        free(arch_palabra);
        fclose(archivo);
        return false;
    }

void mostrarMarcador(struct Juego partida[2], ALLEGRO_FONT *texto){
    int x_texto = (TAM-5)/2*CASILLA, y_texto = (TAM-3)/2*CASILLA; // Calculamos las posiciones (x,y) iniciales para dibujar un rectangulo centrado
    al_draw_filled_rectangle(x_texto, y_texto, x_texto+(5*CASILLA), y_texto+(3*CASILLA), al_map_rgba(39,30,22, 200)); // Rectangulo centrado
    if(partida[0].puntaje_total > partida[1].puntaje_total){ // Anunciamos al ganador
        al_draw_text(texto, al_map_rgb(255,255,255), x_texto, y_texto+CASILLA, 0, "GANA JUGADOR 1");
    }else{
        if(partida[0].puntaje_total < partida[1].puntaje_total){
            al_draw_text(texto, al_map_rgb(255,255,255), x_texto, y_texto+CASILLA, 0, "GANA JUGADOR 2");
        }else{
            al_draw_text(texto, al_map_rgb(255,255,255), x_texto+20, y_texto+CASILLA, 0, "SIN GANADOR");
        }
    }

    ALLEGRO_FONT *archivo = al_create_builtin_font();
    int i=0, j =0, puntaje1 = partida[0].puntaje_total, puntaje2 = partida[1].puntaje_total;
    char *jugador1 = (char*)calloc(30, sizeof(char));
    char *jugador2 = (char*)calloc(30, sizeof(char));
    convertirPalabra(jugador1, &puntaje1, 1, &i);
    convertirPalabra(jugador2, &puntaje2, 1, &j);
    al_draw_text(archivo, al_map_rgb(255,255,255), x_texto+20, y_texto+100+CASILLA, 0, "Guardado en Scrabble_Score.txt");
    al_draw_text(archivo, al_map_rgb(255,255,255), x_texto+20, y_texto+60+CASILLA, 0, "JUGADOR 1:");
    al_draw_text(archivo, al_map_rgb(255,255,255), x_texto+20, y_texto+80+CASILLA, 0, "JUGADOR 2:");
    al_draw_text(archivo, al_map_rgb(255,255,255), x_texto+120, y_texto+60+CASILLA, 0, jugador1);
    al_draw_text(archivo, al_map_rgb(255,255,255), x_texto+120, y_texto+80+CASILLA, 0, jugador2);
    al_flip_display();
    al_rest(2.0); // Dejamos el resultado en pantalla por 3 sec

    al_destroy_font(archivo);
    free(jugador1);
    free(jugador2);
}

void mostrarInstrucciones(ALLEGRO_EVENT_QUEUE *evento){
    bool run = true;
    ALLEGRO_EVENT ev;
    while(run){
        if(al_get_next_event(evento, &ev)){
            if(ev.type == ALLEGRO_EVENT_KEY_CHAR){
                run = false;
            }
        }
        al_draw_filled_rectangle(CASILLA*1, CASILLA*3, CASILLA*8, CASILLA*6, al_map_rgb(222,206,189));
        ALLEGRO_FONT *instrucciones = al_create_builtin_font();
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +10, 0, "1. Comience desde casilla (5,5)");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +30, 0, "2. Solo Palabras de const Diccionario.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +50, 0, "3. No se salga del tablero.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +70, 0, "4. Si no acepta una palabra.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 20, CASILLA*3 +90, 0, "de click nuevamente.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +110, 0, "5. Use las flechas para ajustar la direccion.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +130, 0, "6. Presione enter para mandar la palabra.");
        al_draw_text(instrucciones, al_map_rgb(39,36,30), CASILLA*1.5 + 10, CASILLA*3 +150, 0, "7. No se aceptan palabras repetidas.");
        al_flip_display();
        al_destroy_font(instrucciones);
    }
}
