// Equipo 7
// ISC
// 2°C
// Adriana Chiquito Díaz de León 341166
// Ejercicio 2: Balanza
// 24-05-2025

#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define EJE_X 1080
#define EJE_Y 400

bool calcularEquilibrio(FILE *archivo_entrada, FILE *archivo_salida, int *peso, ALLEGRO_FONT *texto); // Funcion recursiva que recorre el archivo, guarda y calcula los datos
void recorrerModulo1(int distancia1, ALLEGRO_FONT *texto);
void recorrerModulo2(int distancia2, ALLEGRO_FONT *texto);
void balanzaDerecha(int peso2, int distancia2, ALLEGRO_FONT *texto);
void balanzaIzquierda(int peso1, int distancia1, ALLEGRO_FONT *texto);
void convertirPalabra(char *signo, int *num, int divisor, int *k);
void recorrerArriba();

int main(){
    if (!al_init() || !al_init_primitives_addon() || !al_init_ttf_addon()) { // Iniciamos allegro, figuras
        return -1;
    }

    int peso, i = 1; // Peso (que va a tomar el valor de peso1 y peso2), y el i es el numero del modulo a analizar
    bool run = true; 
    int j = 0, k = 0;
    FILE *archivo_entrada = fopen("Balanza.txt", "r");
    FILE *archivo_salida = fopen("Moviles_Equilibrados.txt", "w");
    if(!archivo_entrada || !archivo_salida){
        if(archivo_entrada){fclose(archivo_entrada);}
        if(archivo_salida){fclose(archivo_salida);}
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(EJE_X,EJE_Y); // Crea una ventana de 1080*800
    ALLEGRO_EVENT_QUEUE *evento = al_create_event_queue(); // Crea una cola donde se almacenan los eventos
    ALLEGRO_EVENT ev; // Recibe los eventos
    ALLEGRO_FONT *texto = al_load_ttf_font("frisk.ttf", 30, 0);
    if(!display || !evento || !texto){ // Iniciamos las variables ALLEGRO
        return -1;
    }

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA); // Para la transparencia de la ventana "mensaje"
    al_register_event_source(evento, al_get_display_event_source(display));

    while(run){
        al_clear_to_color(al_map_rgb(227, 215, 206));
        al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));
        al_flip_display();
        al_rest(2.0);

        if(calcularEquilibrio(archivo_entrada, archivo_salida, &peso, texto)){ // Calculamos balanza por balanza
            fprintf(archivo_salida, " Movil %d: EQUILIBRADO\n\n", i); // Si esta equilibrada (recibimos true)
        }else{
            if(peso != -1){ // Cuando es -1 es porque hemos llegado a la linea 0000 (fin del archivo)
                fprintf(archivo_salida, " Movil %d: NO EQUILIBRADO\n\n", i); // Si no esta equilibrada (recibimos false)
            }else{
                run = false; // Dejamos de ejecutar cuando lleguemos a la linea 0000 (peso == -1)
            }
        }
        i++; // Numero del movil a calcular
    }
    // Liberamos
    fclose(archivo_entrada);
    fclose(archivo_salida);
    al_destroy_font(texto);
    al_destroy_display(display);
    al_destroy_event_queue(evento);
    return 0;
}

bool calcularEquilibrio(FILE *archivo_entrada, FILE *archivo_salida, int *peso, ALLEGRO_FONT *texto){
    int peso1 = 0, peso2 = 0, distancia1 = 0, distancia2 = 0; // Crea nuevas variables de datos en cada recursion
    al_clear_to_color(al_map_rgb(227, 215, 206));
    al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));

    fscanf(archivo_entrada, "%d %d %d %d", &peso1, &distancia1, &peso2, &distancia2); // Guarda los datos de la linea en las variables respectivas
    if(peso1 == 0 && peso2 == 0 && distancia1 == 0 && distancia2 == 0){ // Verifica que no hayamos llegado al final del archivo
        *peso = -1; // De ser el caso, el peso toma el valor de -1 (asi el main sabra que hemos llegado al final)
        return false; // Fin del archivo
    }

    fprintf(archivo_salida, "%d %d %d %d\n", peso1, distancia1, peso2, distancia2); // Si el archivo aun tiene contenido, lo guardamos en otro archivo

    if(peso1 == 0){ // Si el peso1 es igual a 0, quiere decir que existe un submodulo del lado izquierdo de la balanza
        balanzaDerecha(peso2, distancia2, texto);
        recorrerModulo1(distancia1, texto);
        if(!calcularEquilibrio(archivo_entrada, archivo_salida, &peso1, texto)){ // Llamamos a la funcion, para calcular el submodulo
            al_clear_to_color(al_map_rgb(227, 215, 206));
            al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));
            balanzaIzquierda(peso1, distancia1, texto);
            balanzaDerecha(peso2, distancia2, texto);
            al_draw_text(texto, al_map_rgb(255,255,255), 500, EJE_Y-60, 0, "NO EQUILIBRADO");
            al_flip_display();
            al_rest(2.0);
            recorrerArriba();
            return false; // Si el submodulo no esta equilibrado, quiere decir que el modulo desde el que fue llamado, tampoco lo esta
        }
    }
    al_clear_to_color(al_map_rgb(227, 215, 206));
    al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));

    if(peso2 == 0){ // Si el peso2 es igual a 0, quiere decir que existe un submodulo del lado derecho de la balanza
        balanzaIzquierda(peso1, distancia1, texto);
        recorrerModulo2(distancia2, texto);
        if(!calcularEquilibrio(archivo_entrada, archivo_salida, &peso2, texto)){ // Llamamos a la funcion, para calcular el submodulo
            al_clear_to_color(al_map_rgb(227, 215, 206));
            al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));
            balanzaIzquierda(peso1, distancia1, texto);
            balanzaDerecha(peso2, distancia2, texto);
            al_draw_text(texto, al_map_rgb(255,255,255), 500, EJE_Y-60, 0, "NO EQUILIBRADO");
            al_flip_display();
            al_rest(2.0);
            recorrerArriba();
            return false; // Si el submodulo no esta equilibrado, quiere decir que el modulo desde el que fue llamado, tampoco lo esta
        }
    }

    al_clear_to_color(al_map_rgb(227, 215, 206));
    al_draw_filled_rectangle(0, EJE_Y-80, EJE_X, EJE_Y, al_map_rgb(115, 108, 102));
    balanzaIzquierda(peso1, distancia1, texto);
    balanzaDerecha(peso2, distancia2, texto);
    al_flip_display();
    al_rest(2.0);

    // Si los pesos de la balanza tienen valores o las llamadas a la funcion de los submodulos regresaron true
    if(peso1 * distancia1 == peso2 * distancia2){ // Calculamos si la operacion pi*di = pd*dp (en quilibrio)
        al_draw_text(texto, al_map_rgb(255,255,255), 40, EJE_Y-60, 0, "EQUILIBRADO");
        al_flip_display();
        al_rest(2.0);
        recorrerArriba();
        *peso = peso1 + peso2; // Si es el caso sumamos ambos pesos (por si estamos en una recursion donde peso = (peso1 o peso2))
        return true; // Balanza o submodulo en equilibrio
    }

    al_draw_text(texto, al_map_rgb(255,255,255), 400, EJE_Y-60, 0, "NO EQUILIBRADO");
    al_flip_display();
    al_rest(2.0);
    recorrerArriba();

    return false; // Balanza o submodulo en desequilibrio
}

void balanzaDerecha(int peso2, int distancia2, ALLEGRO_FONT *texto){
    int x1 = 80, y1 = 80, x2 = EJE_X-80, k = 0;
    int centro_x = 540;
    char *p2 = (char*)calloc(30,sizeof(char));
    char *d2 = (char*)calloc(30,sizeof(char));
    convertirPalabra(p2, &peso2, 1, &k); k = 0;
    convertirPalabra(d2, &distancia2, 1, &k); k = 0;

    // Linea Vertical Superior
    al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80, al_map_rgb(108, 229, 167));
    // Linea Horizontal
    al_draw_filled_rectangle(centro_x-5, y1+80, x2-80, y1+90, al_map_rgb(108, 229, 167));
    // Distancias
    al_draw_text(texto, al_map_rgb(255,255,255), x1+600, y1+60, 0, d2); // Distancia 2
    
    // Extremo Derecha
    al_draw_filled_rectangle(x2-90, y1+80, x2-80, y1+180, al_map_rgb(108, 229, 167)); // Linea Vertical
    al_draw_filled_rectangle(x2-170, y1+180, x2, y1+250, al_map_rgb(108, 229, 167)); // Cuadrado
    al_draw_text(texto, al_map_rgb(255,255,255), x2-110, y1+190, 0, p2); // Peso 2

    free(p2);
    free(d2);
}

void balanzaIzquierda(int peso1, int distancia1, ALLEGRO_FONT *texto){
    int x1 = 80, y1 = 80, x2 = EJE_X-80, k = 0;
    int centro_x = 540;
    char *p1 = (char*)calloc(30,sizeof(char));
    char *d1 = (char*)calloc(30,sizeof(char));
    convertirPalabra(p1, &peso1, 1, &k); k = 0;
    convertirPalabra(d1, &distancia1, 1, &k); k = 0;

    // Linea Vertical Superior
    al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80, al_map_rgb(108, 229, 167));
    // Linea Horizontal
    al_draw_filled_rectangle(x1+80, y1+80, centro_x+5, y1+90, al_map_rgb(108, 229, 167));
    // Distancias
    al_draw_text(texto, al_map_rgb(255,255,255), x1+250, y1+60, 0, d1); // Distancia 1

    // Extremo Izquierda
    al_draw_filled_rectangle(x1+80, y1+80, x1+90, y1+180, al_map_rgb(108, 229, 167)); // Linea Vertical
    al_draw_filled_rectangle(x1, y1+180, x1+170, y1+250, al_map_rgb(108, 229, 167)); // Cuadrado
    al_draw_text(texto, al_map_rgb(255,255,255), x1+60, y1+190, 0, p1); // Peso 1

    free(p1);
    free(d1);
}

void recorrerModulo2(int distancia2, ALLEGRO_FONT *texto){
    int x1 = 80, y1 = 80, x2 = EJE_X-80, y2 = EJE_Y-80, k = 0;
    int centro_x = 540;
    char *d2 = (char*)calloc(30,sizeof(char));
    convertirPalabra(d2, &distancia2, 1, &k);

    // Linea Vertical Superior
    al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80, al_map_rgb(108, 229, 167));
    // Linea Horizontal
    al_draw_filled_rectangle(centro_x-5, y1+80, x2-80, y1+90, al_map_rgb(108, 229, 167));
    // Distancias
    al_draw_text(texto, al_map_rgb(255,255,255), x1+600, y1+60, 0, d2); // Distancia 2

    // Extremo Derecha
    al_draw_filled_rectangle(x2-90, y1+80, x2-80, y2, al_map_rgb(108, 229, 167)); // Linea Vertical

    int i = 0;
    while(y1+80+(i*5) <= y2){
        if(y1+80+(i*10) <= y2){
            al_draw_filled_rectangle(x2-90, y1+80, x2-80, y1+80+(i*10), al_map_rgb(229, 108, 108)); // Linea Vertical
        }
        al_draw_filled_rectangle(x2-90, y1+80, x2-80, y1+80+(i*5), al_map_rgb(108, 229, 167)); // Linea Vertical
        i++;
        al_flip_display();
        al_rest(0.1);
    }

    free(d2);
}

void recorrerModulo1(int distancia1, ALLEGRO_FONT *texto){
    int x1 = 80, y1 = 80, x2 = EJE_X-80, y2 = EJE_Y-80, k = 0;
    int centro_x = 540;
    char *d1 = (char*)calloc(30,sizeof(char));
    convertirPalabra(d1, &distancia1, 1, &k);

    // Linea Vertical Superior
    al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80, al_map_rgb(108, 229, 167));
    // Linea Horizontal
    al_draw_filled_rectangle(x1+80, y1+80, centro_x+5, y1+90, al_map_rgb(108, 229, 167));
    // Distancias
    al_draw_text(texto, al_map_rgb(255,255,255), x1+250, y1+60, 0, d1); // Distancia 1

    // Extremo Izquierda
    al_draw_filled_rectangle(x1+80, y1+80, x1+90, y2, al_map_rgb(108, 229, 167)); // Linea Vertical

    int i = 0;
    while(y1+80+(i*5) <= y2){
        if(y1+80+(i*10) <= y2){
            al_draw_filled_rectangle(x1+80, y1+80, x1+90, y1+80+(i*10), al_map_rgb(229, 108, 108)); // Linea Vertical
        }
        al_draw_filled_rectangle(x1+80, y1+80, x1+90, y1+80+(i*5), al_map_rgb(108, 229, 167)); // Linea Vertical
        i++;
        al_flip_display();
        al_rest(0.1);
    }

    i++;
    free(d1);
}

void convertirPalabra(char *signo, int *num, int divisor, int *k){ // Convierte el numero en una variable char*, sin importar la extension
    if((*num)/divisor >= 10){ // Para convertir unidad por unidad
        convertirPalabra(signo, num, divisor*10, k); // Quiere decir que el numero es diez veces mas grande
    }
    int n = (*num)/divisor;
    signo[(*k)++] = n + '0'; // Agregamos unidad por unidad
    (*num) -= (n*divisor);
    if(divisor == 1) {
        signo[*k] = '\0';
    }
}

void recorrerArriba(){
    int x1 = 80, y1 = 80, x2 = EJE_X-80, y2 = EJE_Y-80, k = 0;
    int centro_x = 540;
    int i = 0;

    // Linea Vertical Superior
    al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80, al_map_rgb(108, 229, 167));

    while(y1+80-(i*5) >= 0){
        if(y1+80-(i*10) >= 0){
            al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80-(i*5), al_map_rgb(229, 108, 108));
        }
        al_draw_filled_rectangle(centro_x-5, 0, centro_x+5, y1+80-(i*10), al_map_rgb(108, 229, 167));
        i++;
        al_flip_display();
        al_rest(0.1);
    }
}
