#define _CRT_SECURE_NO_WARNINGS  // evita advertencias  por el fopen al usar allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <stdio.h>

using namespace std;

const int MAX_HORAS = 1000; // maximo de horas que se pueden almacenar

// estructura para almacenar hora y minuto
struct HoraMinuto {
    int hora;
    int minuto;
};

// estructura para crear botones interactivos
struct Boton {
    int x, y, w, h;          // posicion y dimensiones
    const char* texto;        // texto del boton
    ALLEGRO_COLOR color_fondo; // color de fondo
    ALLEGRO_COLOR color_texto; // color del texto

    // verifica si un punto (px,py) esta dentro del boton
    bool contiene(int px, int py)
    {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }

    // dibuja el boton en pantalla
    void dibujar(ALLEGRO_FONT* fuente)
    {
        al_draw_filled_rectangle(x, y, x + w, y + h, color_fondo);
        al_draw_rectangle(x, y, x + w, y + h, al_map_rgb(0, 0, 0), 2);
        int th = al_get_font_line_height(fuente);
        al_draw_text(fuente, color_texto, x + w / 2.0f, y + h / 2.0f - th / 2.0f,
            ALLEGRO_ALIGN_CENTRE, texto);
    }
};

int main()
{
    // inicializacion de allegro y sus componentes
    if (!al_init())
    {
        cout << "error al inicializar allegro" << endl;
        return -1;
    }
    al_init_font_addon();      // soporte para fuentes
    al_init_ttf_addon();       // soporte para fuentes ttf
    al_init_primitives_addon(); // soporte para dibujar formas
    al_install_mouse();         // soporte para raton

    // creacion de la ventana principal
    const int ANCHO = 640, ALTO = 480;
    ALLEGRO_DISPLAY* display = al_create_display(ANCHO, ALTO);
    if (!display)
    {
        cout << "no se pudo crear la ventana" << endl;
        return -1;
    }
    al_set_window_title(display, "Noche vieja - minutos para medianoche");

    // carga de fuentes para texto
    ALLEGRO_FONT* fuente_titulo = al_load_ttf_font("arial.ttf", 36, 0);
    ALLEGRO_FONT* fuente_texto = al_load_ttf_font("arial.ttf", 20, 0);
    if (!fuente_titulo || !fuente_texto)
    {
        cout << "error al cargar fuentes" << endl;
        al_destroy_display(display);
        return -1;
    }

    // lectura de datos desde archivo
    FILE* archivo = fopen("horas.txt", "r");
    if (!archivo)
    {
        cout << "error al abrir archivo horas.txt" << endl;
        al_destroy_display(display);
        return -1;
    }

    // almacenamiento de horas leidas
    HoraMinuto horas[MAX_HORAS];
    int cantidad = 0;

    int h, m;
    // procesamiento de cada linea del archivo
    while (fscanf(archivo, "%d:%d", &h, &m) == 2)
    {
        if (h == 0 && m == 0)
        {
            break;  // fin del archivo
        }
        if (h < 0 || h > 23 || m < 0 || m > 59)
        {
            continue; // validacion
        }

        // almacenar datos validos
        if (cantidad < MAX_HORAS)
        {
            horas[cantidad] = { h, m };
            cantidad++;
        }
        else
        {
            cout << "se alcanzo el maximo de horas almacenadas." << endl;
            break;
        }
    }
    fclose(archivo);

    // creacion de botones interactivos
    Boton boton_mostrar =
    {
        50, ALTO - 100, 220, 50,
        "Minutos faltantes",
        al_map_rgb(70,130,180),   // azul 
        al_map_rgb(255,255,255)   // blanco
    };

    Boton boton_salir =
    {
        ANCHO - 150, ALTO - 100, 100, 50,
        "Salir",
        al_map_rgb(220,20,60),    // rojo 
        al_map_rgb(255,255,255)   // blanco
    };

    // variables de control del programa
    bool running = true;
    bool mostrar_minutos = false;

    // configuracion de eventos
    ALLEGRO_EVENT_QUEUE* cola = al_create_event_queue();
    al_register_event_source(cola, al_get_display_event_source(display));
    al_register_event_source(cola, al_get_mouse_event_source());

    // bucle principal del programa
    while (running)
    {
        ALLEGRO_EVENT ev;
        // manejo de eventos
        while (al_get_next_event(cola, &ev))
        {
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                running = false;
            }
            else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                int x = ev.mouse.x, y = ev.mouse.y;

                if (boton_mostrar.contiene(x, y))
                {
                    mostrar_minutos = true;

                    // abrir archivo para escribir resultados
                    FILE* resultados = fopen("resultados.txt", "w");
                    if (resultados)
                    {
                        for (int i = 0; i < cantidad; i++)
                        {
                            // calcula el total de minutos transcurridos desde las 00:00
                            int total_minutos = horas[i].hora * 60 + horas[i].minuto;

                            // calcula los minutos que faltan para medianoche (1440 minutos en un dia)
                            int faltan = 1440 - total_minutos;

                            // escribe el resultado en el archivo, un numero por linea
                            fprintf(resultados, "%d\n", faltan);
                        }
                        fclose(resultados);
                    }
                }
                else if (boton_salir.contiene(x, y))
                {
                    running = false;
                }
            }
        }

        // dibujado de la interfaz
        al_clear_to_color(al_map_rgb(30, 30, 60)); // fondo azul oscuro

        // titulo principal
        al_draw_text(fuente_titulo, al_map_rgb(255, 215, 0), ANCHO / 2, 20,
            ALLEGRO_ALIGN_CENTRE, "Noche vieja");

        // listado de horas leidas
        int y0 = 80;
        al_draw_text(fuente_texto, al_map_rgb(255, 255, 255), 50, y0 - 30, 0, "Horas registradas:");
        for (int i = 0; i < cantidad; i++)
        {
            char buf[20];
            sprintf(buf, "%02d:%02d", horas[i].hora, horas[i].minuto);
            al_draw_text(fuente_texto, al_map_rgb(200, 200, 200), 50, y0 + i * 25, 0, buf);
        }

        // mostrar minutos faltantes si esta activado
        if (mostrar_minutos)
        {
            al_draw_text(fuente_texto, al_map_rgb(144, 238, 144), 350, y0 - 30, 0,
                "Minutos para medianoche:");
            for (int i = 0; i < cantidad; i++)
            {
                int total_minutos = horas[i].hora * 60 + horas[i].minuto;
                int faltan = 1440 - total_minutos;
                char buf[10];
                sprintf(buf, "%d", faltan);
                al_draw_text(fuente_texto, al_map_rgb(144, 238, 144), 350, y0 + i * 25, 0, buf);
            }
        }

        // dibujar botones
        boton_mostrar.dibujar(fuente_texto);
        boton_salir.dibujar(fuente_texto);

        // actualizar pantalla
        al_flip_display();
        al_rest(0.01); // pequena pausa para reducir uso de cpu
    }

    // limpieza final
    al_destroy_font(fuente_titulo);
    al_destroy_font(fuente_texto);
    al_destroy_display(display);
    al_destroy_event_queue(cola);

    return 0;
}

