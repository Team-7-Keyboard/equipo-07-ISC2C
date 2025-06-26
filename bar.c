#define _CRT_SECURE_NO_WARNINGS // Evita advertencias por el fopen al usar Allegro en Visual Studio

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>   // Para printf, fopen, fclose, fscanf_s, fprintf, snprintf
#include <string.h>  // Para strcpy_s, strcmp, strlen
#include <stdlib.h>  // Para system
#include <stdbool.h> // Para el tipo bool

#define TAM 5 // Número de categorías existentes
#define MAX_LINE_LENGTH 100 // Longitud máxima para las líneas de resultados de texto en la GUI
#define MAX_RESULTS 50 // Número máximo de líneas de resultados para mostrar en la GUI

// Estructura para los botones
typedef struct {
    int x, y, w, h;
    const char* texto;
} Boton;

// --- Prototipos de funciones ---
// Funciones de lógica de negocio (cálculos de categorías)
const char* categoriaMax(float ventas[]);
const char* categoriaMin(float ventas[]);

// Función de procesamiento principal de datos
// Ahora toma un arreglo bidimensional de caracteres y un puntero para el conteo
void procesarDatos(char resultados[][MAX_LINE_LENGTH], int* num_resultados);

// Función para abrir el archivo de salida
void abrirArchivoSalida();

// Funciones de utilidad para la interfaz gráfica
void dibujarBoton(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int x, int y, int w, int h, const char* texto, bool hover);
bool puntoEnRect(int px, int py, int rx, int ry, int rw, int rh);

// --- Función principal ---
int main() {
    // Inicialización de Allegro y sus módulos
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

    // Creación de la ventana principal
    ALLEGRO_DISPLAY* display = al_create_display(640, 480);
    if (!display) {
        printf("Error al crear ventana.\n");
        return -1;
    }
    al_set_window_title(display, "BAR - EL PEPE"); // Establecer título de la ventana

    // Cargar fuente TTF
    ALLEGRO_FONT* font = al_load_ttf_font("arial.ttf", 18, 0);
    if (!font) {
        printf("Error al cargar fuente 'arial.ttf'. Asegúrate de que el archivo existe.\n");
        al_destroy_display(display);
        return -1;
    }

    // Crear y registrar la cola de eventos
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());

    // Definición de los botones
    Boton btnProcesar = { 50, 400, 150, 40, "Procesar datos" };
    Boton btnAbrir = { 250, 400, 150, 40, "Abrir archivo salida" };
    Boton btnSalir = { 450, 400, 100, 40, "Salir" };

    // Arreglo para mostrar resultados en pantalla y su contador
    char resultados[MAX_RESULTS][MAX_LINE_LENGTH];
    int num_resultados = 0;

    bool redraw = true;
    bool running = true;

    // Bucle principal de la interfaz gráfica
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev); // Espera un evento para ahorrar CPU

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            // Detectar en qué botón se hizo clic
            if (puntoEnRect(mx, my, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h)) {
                // Reiniciar resultados antes de procesar nuevos datos
                num_resultados = 0;
                procesarDatos(resultados, &num_resultados);
                redraw = true; // Forzar redibujado
            }
            else if (puntoEnRect(mx, my, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h)) {
                abrirArchivoSalida();
            }
            else if (puntoEnRect(mx, my, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h)) {
                running = false; // Salir del programa
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            // Este evento ocurre cuando el mouse se mueve. Lo usamos para el efecto hover.
            // Solo necesitamos redibujar si el mouse entra o sale de un botón.
            static int prev_mx = -1, prev_my = -1;
            if (prev_mx != ev.mouse.x || prev_my != ev.mouse.y) {
                redraw = true;
                prev_mx = ev.mouse.x;
                prev_my = ev.mouse.y;
            }
        }

        // Redibujar la pantalla si es necesario y la cola de eventos está vacía
        // (Esto evita redibujar múltiples veces por eventos de mouse muy rápidos)
        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(118, 22, 196)); // Color de fondo púrpura oscuro

            // Título
            al_draw_text(font, al_map_rgb(209, 104, 97), 320, 20, ALLEGRO_ALIGN_CENTRE, "BAR - EL PEPE");

            // Dibujar botones (con efecto hover)
            ALLEGRO_MOUSE_STATE mouse_state;
            al_get_mouse_state(&mouse_state); // Obtener la posición actual del mouse

            dibujarBoton(display, font, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h, btnProcesar.texto,
                         puntoEnRect(mouse_state.x, mouse_state.y, btnProcesar.x, btnProcesar.y, btnProcesar.w, btnProcesar.h));
            dibujarBoton(display, font, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h, btnAbrir.texto,
                         puntoEnRect(mouse_state.x, mouse_state.y, btnAbrir.x, btnAbrir.y, btnAbrir.w, btnAbrir.h));
            dibujarBoton(display, font, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h, btnSalir.texto,
                         puntoEnRect(mouse_state.x, mouse_state.y, btnSalir.x, btnSalir.y, btnSalir.w, btnSalir.h));

            // Mostrar resultados
            int y_texto = 80;
            if (num_resultados == 0) {
                al_draw_text(font, al_map_rgb(255, 255, 255), 30, y_texto, ALLEGRO_ALIGN_LEFT,
                    "Presiona 'Procesar datos' para comenzar.");
            } else {
                for (int i = 0; i < num_resultados; ++i) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 30, y_texto, ALLEGRO_ALIGN_LEFT, resultados[i]);
                    y_texto += 24; // Espacio entre líneas
                }
            }
            
            al_flip_display(); // Actualizar la pantalla
            redraw = false;    // Ya se ha redibujado
        }
    }

    // Liberar recursos de Allegro al salir
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    return 0;
}

// --- Implementación de funciones de utilidad para la interfaz gráfica ---

// Función para dibujar un botón con efecto hover
void dibujarBoton(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int x, int y, int w, int h, const char* texto, bool hover) {
    ALLEGRO_COLOR color_fondo = hover ? al_map_rgb(100, 100, 255) : al_map_rgb(50, 50, 150); // Azul claro/oscuro
    ALLEGRO_COLOR color_borde = al_map_rgb(200, 200, 255); // Borde claro
    ALLEGRO_COLOR color_texto = al_map_rgb(255, 255, 255); // Texto blanco

    al_draw_filled_rectangle(x, y, x + w, y + h, color_fondo);
    al_draw_rectangle(x, y, x + w, y + h, color_borde, 2); // Borde de 2px
    // Centrar texto en el botón
    al_draw_text(font, color_texto, x + w / 2, y + h / 2 - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_CENTRE, texto);
}

// Verifica si un punto (px, py) está dentro de un rectángulo (rx, ry, rw, rh)
bool puntoEnRect(int px, int py, int rx, int ry, int rw, int rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

// --- Implementación de funciones de lógica de negocio ---

// Procesa los datos del archivo 'bar.txt' y llena el arreglo 'resultados' para mostrar en pantalla
void procesarDatos(char resultados[][MAX_LINE_LENGTH], int* num_resultados) {
    float ventas[TAM] = { 0 }; // Inicializar todas las ventas a 0
    char categoria;
    float valor; // Variable para el valor de venta

    FILE* bar;
    // Usar fopen_s para seguridad en Windows, si no, usar fopen
#ifdef _WIN32
    if (fopen_s(&bar, "bar.txt", "r") != 0) {
        strcpy_s(resultados[*num_resultados], MAX_LINE_LENGTH, "Error: No se pudo abrir 'bar.txt'");
        (*num_resultados)++;
        return;
    }
#else
    bar = fopen("bar.txt", "r");
    if (!bar) {
        // strncpy para evitar desbordamiento y asegurar terminación nula
        strncpy(resultados[*num_resultados], "Error: No se pudo abrir 'bar.txt'", MAX_LINE_LENGTH - 1);
        resultados[*num_resultados][MAX_LINE_LENGTH - 1] = '\0';
        (*num_resultados)++;
        return;
    }
#endif

    FILE* salida;
#ifdef _WIN32
    if (fopen_s(&salida, "salida.txt", "w") != 0) {
        strcpy_s(resultados[*num_resultados], MAX_LINE_LENGTH, "Error: No se pudo crear 'salida.txt'");
        (*num_resultados)++;
        fclose(bar); // Asegurarse de cerrar el archivo de entrada
        return;
    }
#else
    salida = fopen("salida.txt", "w");
    if (!salida) {
        strncpy(resultados[*num_resultados], "Error: No se pudo crear 'salida.txt'", MAX_LINE_LENGTH - 1);
        resultados[*num_resultados][MAX_LINE_LENGTH - 1] = '\0';
        (*num_resultados)++;
        fclose(bar); // Asegurarse de cerrar el archivo de entrada
        return;
    }
#endif

    // Bucle para leer y procesar datos del archivo 'bar.txt'
    // fscanf_s requiere el tamaño del búfer para '%c' en Windows
    while (fscanf_s(bar, " %c %f", &categoria, 1, &valor) == 2) {
        if ((categoria == 'N' || categoria == 'n') && valor == 0) {
            // Fin de un día o sección de datos
            float ventasDia = 0;
            for (int i = 0; i < TAM; i++) {
                ventasDia += ventas[i];
            }
            float promVentas = ventasDia / TAM;
            const char* catMax = categoriaMax(ventas);
            const char* catMin = categoriaMin(ventas);

            // Solo imprimir si no hay empate en máximo y mínimo
            if (!(strcmp(catMax, "EMPATE") == 0 && strcmp(catMin, "EMPATE") == 0)) {
                char respuesta[4]; // Suficiente para "SI" o "NO" y terminador nulo
                if (ventas[1] > promVentas) { // ventas[1] corresponde a "COMIDAS"
                    strcpy_s(respuesta, sizeof(respuesta), "SI");
                } else {
                    strcpy_s(respuesta, sizeof(respuesta), "NO");
                }
                
                // Formatear la línea de resultado en C
                char linea[MAX_LINE_LENGTH];
                // snprintf es más seguro que sprintf ya que previene desbordamientos de búfer
                snprintf(linea, sizeof(linea), "%s#%s#%s", catMax, catMin, respuesta);
                
                // Almacenar en el arreglo de resultados si hay espacio
                if (*num_resultados < MAX_RESULTS) {
                    // strcpy_s para Windows, strncpy para otros SO
#ifdef _WIN32
                    strcpy_s(resultados[*num_resultados], MAX_LINE_LENGTH, linea);
#else
                    strncpy(resultados[*num_resultados], linea, MAX_LINE_LENGTH - 1);
                    resultados[*num_resultados][MAX_LINE_LENGTH - 1] = '\0'; // Asegurar terminación nula
#endif
                    (*num_resultados)++;
                }
                fprintf(salida, "%s\n", linea); // Escribir en el archivo de salida
            }

            // Reiniciar ventas para la siguiente sección de datos
            for (int i = 0; i < TAM; i++) ventas[i] = 0;
        }
        else {
            // Acumular ventas por categoría
            switch (categoria) {
            case 'D': ventas[0] += valor; break; // Desayunos
            case 'A': ventas[1] += valor; break; // Comidas (Almuerzos)
            case 'M': ventas[2] += valor; break; // Meriendas
            case 'I': ventas[3] += valor; break; // Cenas
            case 'C': ventas[4] += valor; break; // Copas
            }
        }
    }

    fclose(bar);
    fclose(salida);

    // Si no se procesaron datos, agregar un mensaje
    if (*num_resultados == 0) {
#ifdef _WIN32
        strcpy_s(resultados[*num_resultados], MAX_LINE_LENGTH, "No se encontraron datos para procesar o hubo un error.");
#else
        strncpy(resultados[*num_resultados], "No se encontraron datos para procesar o hubo un error.", MAX_LINE_LENGTH - 1);
        resultados[*num_resultados][MAX_LINE_LENGTH - 1] = '\0';
#endif
        (*num_resultados)++;
    }
}

// Abre el archivo de salida con el programa predeterminado del sistema
void abrirArchivoSalida() {
#ifdef _WIN32
    system("start salida.txt"); // Comando para Windows
#elif __APPLE__
    system("open salida.txt"); // Comando para macOS
#else
    system("xdg-open salida.txt"); // Comando para Linux (muchas distribuciones)
#endif
}

// Función para determinar la categoría con el valor máximo de ventas
const char* categoriaMax(float ventas[]) {
    float valorMax = ventas[0];
    int contMax = 1; // Contador para detectar empates
    int indiceMax = 0; // Índice de la categoría con el valor máximo

    for (int i = 1; i < TAM; i++) {
        if (ventas[i] > valorMax) {
            valorMax = ventas[i];
            indiceMax = i;
            contMax = 1; // Reiniciar contador si se encuentra un nuevo máximo
        }
        else if (ventas[i] == valorMax) {
            contMax++; // Incrementar si hay un empate
        }
    }
    // Si hay más de un valor máximo, es un empate
    if (contMax > 1) return "EMPATE";

    // Retorna el nombre de la categoría correspondiente
    switch (indiceMax) {
    case 0: return "DESAYUNOS";
    case 1: return "COMIDAS";
    case 2: return "MERIENDAS";
    case 3: return "CENAS";
    case 4: return "COPAS";
    default: return "ERROR"; // Debería ser inalcanzable con TAM=5 y 0-4
    }
}

// Función para determinar la categoría con el valor mínimo de ventas
const char* categoriaMin(float ventas[]) {
    float valorMin = ventas[0];
    int contMin = 1; // Contador para detectar empates
    int indiceMin = 0; // Índice de la categoría con el valor mínimo

    for (int i = 1; i < TAM; i++) {
        if (ventas[i] < valorMin) {
            valorMin = ventas[i];
            indiceMin = i;
            contMin = 1; // Reiniciar contador si se encuentra un nuevo mínimo
        }
        else if (ventas[i] == valorMin) {
            contMin++; // Incrementar si hay un empate
        }
    }
    // Si hay más de un valor mínimo, es un empate
    if (contMin > 1) return "EMPATE";

    // Retorna el nombre de la categoría correspondiente
    switch (indiceMin) {
    case 0: return "DESAYUNOS";
    case 1: return "COMIDAS";
    case 2: return "MERIENDAS";
    case 3: return "CENAS";
    case 4: return "COPAS";
    default: return "ERROR"; // Debería ser inalcanzable con TAM=5 y 0-4
    }
}