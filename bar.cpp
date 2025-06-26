#define _CRT_SECURE_NO_WARNINGS  // evita advertencias  por el fopen al usar allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM 5 // numero de categorias existentes

using namespace std; 

// prototipos
const char* categoriaMax(float[]);
const char* categoriaMin(float[]);
void procesarDatos(char resultados[][100], int* num_resultados);
void abrirArchivoSalida();
void dibujarBoton(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int x, int y, int w, int h, const char* texto, bool hover);
bool puntoEnRect(int px, int py, int rx, int ry, int rw, int rh);

int main() {
    // inicializacion de allegro y sus modulos
    if (!al_init()) {
        printf("Error al inicializar Allegro.\n");
        return -1;
    }
    if (!al_init_font_addon() || !al_init_ttf_addon()) {
        printf("Error al inicializar addons de fuentes.\n");
        return -1;
    }
    if (!al_init_primitives_addon()) {
        printf("Error al inicializar primitivas.\n");
        return -1;
    }
    if (!al_install_mouse()) {
        printf("Error al inicializar mouse.\n");
        return -1;
    }

    // crear ventana principal
    ALLEGRO_DISPLAY* display = al_create_display(640, 480);
    if (!display) {
        printf("Error al crear ventana.\n");
        return -1;
    }

    // cargar fuente ttf
    ALLEGRO_FONT* font = al_load_ttf_font("arial.ttf", 18, 0);
    if (!font) {
        printf("Error al cargar fuente 'arial.ttf'.\n");
        al_destroy_display(display);
        return -1;
    }

    // crear y registrar la cola de eventos
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());

    // botones y area de texto
    struct Boton {
        int x, y, w, h;
        const char* texto;
    };

    Boton btnProcesar = { 50, 400, 150, 40, "Procesar datos" };
    Boton btnAbrir = { 250, 400, 150, 40, "Abrir archivo salida" };
    Boton btnSalir = { 450, 400, 100, 40, "Salir" };

    char resultados[100][100]; // para mostrar resultados en pantalla
    int num_resultados = 0;
    bool redraw = true;
    bool running = true;

    // bucle principal de la interfaz grafica
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            // detectar en que boton se hizo clic
            if (puntoEnRect(mx, my, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h)) {
                num_resultados = 0;
                procesarDatos(resultados, &num_resultados); // procesar datos del archivo
                redraw = true;
            }
            else if (puntoEnRect(mx, my, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h)) {
                abrirArchivoSalida(); // abrir archivo de salida
            }
            else if (puntoEnRect(mx, my, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h)) {
                running = false; // salir del programa
            }
        }

        // redibujar la pantalla si es necesario
        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(118, 22, 196)); // cambia el color de fondo aqui

            // titulo
            al_draw_text(font, al_map_rgb(209, 104, 97), 320, 20, ALLEGRO_ALIGN_CENTRE, "BAR - EL PEPE");

            // botones
            ALLEGRO_MOUSE_STATE mouse_state;
            al_get_mouse_state(&mouse_state);

            dibujarBoton(display, font, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h, btnProcesar.texto,
                puntoEnRect(mouse_state.x, mouse_state.y, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h));
            dibujarBoton(display, font, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h, btnAbrir.texto,
                puntoEnRect(mouse_state.x, mouse_state.y, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h));
            dibujarBoton(display, font, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h, btnSalir.texto,
                puntoEnRect(mouse_state.x, mouse_state.y, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h));

            // mostrar resultados
            int y_texto = 80;
            if (num_resultados == 0) {
                al_draw_text(font, al_map_rgb(255, 255, 255), 30, y_texto, ALLEGRO_ALIGN_LEFT, "Presiona 'Procesar datos' para comenzar.");
            } else {
                for (int i = 0; i < num_resultados; ++i) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 30, y_texto, ALLEGRO_ALIGN_LEFT, resultados[i]);
                    y_texto += 24;
                }
            }

            al_flip_display();
            redraw = false;
        }
    }

    // liberar recursos
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    return 0;
}

// funcion para dibujar boton con efecto hover
void dibujarBoton(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int x, int y, int w, int h, const char* texto, bool hover) {
    ALLEGRO_COLOR color_fondo = hover ? al_map_rgb(100, 100, 255) : al_map_rgb(50, 50, 150);
    ALLEGRO_COLOR color_borde = al_map_rgb(200, 200, 255);
    ALLEGRO_COLOR color_texto = al_map_rgb(255, 255, 255);

    al_draw_filled_rectangle(x, y, x + w, y + h, color_fondo);
    al_draw_rectangle(x, y, x + w, y + h, color_borde, 2);
    al_draw_text(font, color_texto, x + w / 2, y + h / 2 - 10, ALLEGRO_ALIGN_CENTRE, texto);
}

// verifica si un punto esta dentro de un rectangulo
bool puntoEnRect(int px, int py, int rx, int ry, int rw, int rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

// procesa los datos del archivo y llena el vector con resultados para mostrar
void procesarDatos(char resultados[][100], int* num_resultados) {
    float ventas[TAM] = { 0 }, valor;
    char categoria;

    FILE* bar;
    if (fopen_s(&bar, "bar.txt", "r") != 0) {
        strcpy_s(resultados[*num_resultados], sizeof(resultados[0]), "Error: No se pudo abrir 'bar.txt'");
        (*num_resultados)++;
        return;
    }
    FILE* salida;
    if (fopen_s(&salida, "salida.txt", "w") != 0) {
        strcpy_s(resultados[*num_resultados], sizeof(resultados[0]), "Error: No se pudo crear 'salida.txt'");
        (*num_resultados)++;
        fclose(bar);
        return;
    }

    while (fscanf_s(bar, " %c %f", &categoria, 1, &valor) == 2) {
        if ((categoria == 'N' || categoria == 'n') && valor == 0) {
            float ventasDia = 0;
            for (int i = 0; i < TAM; i++) ventasDia += ventas[i];
            float promVentas = ventasDia / TAM;
            const char* catMax = categoriaMax(ventas);
            const char* catMin = categoriaMin(ventas);

            if (!(strcmp(catMax, "EMPATE") == 0 && strcmp(catMin, "EMPATE") == 0)) {
                char respuesta[4];
                if (ventas[1] > promVentas) {
                    strcpy_s(respuesta, sizeof(respuesta), "SI");
                }
                else {
                    strcpy_s(respuesta, sizeof(respuesta), "NO");
                }
                char linea[100];
                snprintf(linea, sizeof(linea), "%s#%s#%s", catMax, catMin, respuesta);
                if (*num_resultados < 100) {
                    strcpy_s(resultados[*num_resultados], sizeof(resultados[0]), linea);
                    (*num_resultados)++;
                }
                fprintf(salida, "%s\n", linea);
            }

            for (int i = 0; i < TAM; i++) ventas[i] = 0;
        }
        else {
            switch (categoria) {
            case 'D': ventas[0] += valor; break;
            case 'A': ventas[1] += valor; break;
            case 'M': ventas[2] += valor; break;
            case 'I': ventas[3] += valor; break;
            case 'C': ventas[4] += valor; break;
            }
        }
    }

    fclose(bar);
    fclose(salida);

    if (*num_resultados == 0) {
        strcpy_s(resultados[*num_resultados], sizeof(resultados[0]), "No se encontraron datos para procesar.");
        (*num_resultados)++;
    }
}

// abre el archivo de salida con el programa predeterminado
void abrirArchivoSalida() {
#ifdef _WIN32
    system("start salida.txt");
#elif _APPLE_
    system("open salida.txt");
#else
    system("xdg-open salida.txt");
#endif
}

// funcion para mostrar categoria maxima
const char* categoriaMax(float ventas[]) {
    float valorMax = ventas[0]; // se empieza con el valor maximo de la primera posicion
    int contMax = 1; // sirve para detectar si hay empates en los valores maximos
    int indiceMax = 0; // indica la posicion en el arreglo del valor maximo

    for (int i = 1; i < TAM; i++) {
        if (ventas[i] > valorMax) { // evalua si las ventas de cierta posicion son mayores a las guardadas en valorMax
            valorMax = ventas[i];
            indiceMax = i;
            contMax = 1;
            // en caso de serlo se almacena este como el valor maximo, se guarda la posicion en la que se encuentra y el contador se queda en uno indicando que solo hay un valor maximo
        }
        else if (ventas[i] == valorMax) { // caso si es que es empate entre mayores ventas
            contMax++;
        }
    }
    // se evalua si es un empate y en caso de serlo se le asigna la cadena de EMPATE a la variable nombre y se retorna esta variable
    if (contMax > 1) return "EMPATE";

    // en caso de no ser un empate se retornara el nombre de la categoria con el valor maximo
    switch (indiceMax) {
    case 0: return "DESAYUNOS";
    case 1: return "COMIDAS";
    case 2: return "MERIENDAS";
    case 3: return "CENAS";
    case 4: return "COPAS";
    default: return "NINGUNO";
    }
}

// funcion para mostrar categoria minima
const char* categoriaMin(float ventas[]) {
    float valorMin = ventas[0]; // se empieza con el valor minimo de la primera posicion
    int contMin = 1; // sirve para detectar si hay empates en los valores minimos
    int indiceMin = 0; // indica la posicion en el arreglo del valor minimo

    for (int i = 1; i < TAM; i++) {
        if (ventas[i] < valorMin) { // evalua si las ventas de cierta posicion son menores a las guardadas en valorMin
            valorMin = ventas[i];
            indiceMin = i;
            contMin = 1;
            // en caso de serlo se almacena este como el valor minimo, se guarda la posicion en la que se encuentra y el contador se queda en uno indicando que solo hay un valor minimo
        }
        else if (ventas[i] == valorMin) { // caso si es que es empate entre los minimas ventas
            contMin++;
        }
    }
    // se evalua si es un empate y en caso de serlo se le asigna la cadena de EMPATE a la variable nombre y se retorna esta variable
    if (contMin > 1) return "EMPATE";

    // en caso de no ser un empate se retornara el nombre de la categoria con el valor minimo
    switch (indiceMin) {
    case 0: return "DESAYUNOS";
    case 1: return "COMIDAS";
    case 2: return "MERIENDAS";
    case 3: return "CENAS";
    case 4: return "COPAS";
    default: return "NINGUNO";
    }
}