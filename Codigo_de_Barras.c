#define _CRT_SECURE_NO_WARNINGS  // Desactiva advertencias sobre funciones clasicas como fopen y strcpy

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define MAX_DIGITS 13   // Longitud maxima de un codigo EAN-13
#define MAX_LINE 32     // Tamaño maximo de linea leida del archivo

// Tipo de codigo de barras (EAN-8 o EAN-13)
typedef enum { EAN8, EAN13 } TipoEAN;

// Estructura para representar un codigo de barras
typedef struct {
    char digitos[MAX_DIGITS + 1]; // Arreglo de caracteres que contiene los digitos del codigo
    TipoEAN tipo;                 // EAN8 o EAN13
    int longitud;                 // Cantidad de digitos reales del codigo
} CodigoBarras;

// Estructura que relaciona prefijos con paises
typedef struct {
    char prefijo[4];              // Prefijo numerico de 1 a 3 cifras
    char nombre[32];              // Nombre del pais asociado al prefijo
} PaisEAN;

// Lista de paises validos con sus prefijos EAN
PaisEAN paises[] = {
    {"0",   "EEUU"},
    {"50",  "Inglaterra"},
    {"380", "Bulgaria"},
    {"539", "Irlanda"},
    {"560", "Portugal"},
    {"70",  "Noruega"},
    {"759", "Venezuela"},
    {"850", "Cuba"},
    {"890", "India"},
};

// Declaracion de funciones utilizadas en el programa
FILE* abrirArchivoLecturaOCrear(char* nombre, size_t tam);
bool leerCodigo(FILE* fin, CodigoBarras* codigo);
void completarCeros(CodigoBarras* codigo);
bool validarDigitoControl(const CodigoBarras* codigo);
int sumaRecursiva(const char* digitos, int index, int longitud);
int calcularDigitoControlRecursivo(const CodigoBarras* codigo);
const char* obtenerPais(const CodigoBarras* codigo);
void imprimirResultado(FILE* fout, const CodigoBarras* codigo, bool esValido, const char* pais);
void graficarCodigoBarras(const CodigoBarras* codigo, ALLEGRO_DISPLAY* display);

int main() {
    // Inicializar Allegro y sus complementos
    if (!al_init()) return 1;
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();

    // Crear ventana para mostrar el codigo graficamente
    ALLEGRO_DISPLAY* display = al_create_display(400, 200);
    if (!display) return 1;

    // Abrir archivo de entrada ingresado por el usuario
    char nombreArchivo[100];
    FILE* fin = abrirArchivoLecturaOCrear(nombreArchivo, sizeof(nombreArchivo));

    // Crear archivo de salida donde se escriben los resultados
    FILE* fout = fopen("salida.txt", "w");
    if (!fout) return 1;

    // Preparar lista dinamica para almacenar codigos leidos
    CodigoBarras* lista = NULL;
    int capacidad = 10, usados = 0;
    lista = (CodigoBarras*)malloc(capacidad * sizeof(CodigoBarras));

    CodigoBarras temp;
    while (leerCodigo(fin, &temp)) {
        // Reasignar memoria si la lista esta llena
        if (usados >= capacidad) {
            capacidad *= 2;
            CodigoBarras* nuevaLista = realloc(lista, capacidad * sizeof(CodigoBarras));
            if (!nuevaLista) {
                free(lista);
                return 1;
            }
            lista = nuevaLista;
        }

        // Asegurar que el codigo tenga el tamaño correcto rellenando ceros a la izquierda
        completarCeros(&temp);

        // Guardar codigo en la lista
        lista[usados++] = temp;

        // Validar si el digito de control es correcto
        bool esValido = validarDigitoControl(&temp);

        // Determinar el pais asociado si es un EAN-13 valido
        const char* pais = (temp.tipo == EAN13 && esValido) ? obtenerPais(&temp) : NULL;

        // Imprimir el resultado en archivo y pantalla
        imprimirResultado(fout, &temp, esValido, pais);

        // Si el codigo es valido, mostrar grafica con Allegro
        if (esValido) {
            graficarCodigoBarras(&temp, display);
        }
    }

    // Liberar recursos al final
    fclose(fin);
    fclose(fout);
    al_destroy_display(display);
    free(lista);
    return 0;
}

// Solicita al usuario un nombre de archivo y lo abre para lectura
// Si no existe, da opcion a crearlo vacio
FILE* abrirArchivoLecturaOCrear(char* nombre, size_t tam) {
    FILE* f = NULL;
    printf("Introduce el nombre del archivo de entrada: ");
    fgets(nombre, (int)tam, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    f = fopen(nombre, "r");
    if (!f) {
        printf("El archivo '%s' no existe. Deseas crearlo? (s/n): ", nombre);
        char resp[8];
        fgets(resp, sizeof(resp), stdin);
        if (resp[0] == 's' || resp[0] == 'S') {
            f = fopen(nombre, "w");
            if (!f) exit(1);
            fclose(f);
            printf("Archivo creado. Agrega datos y vuelve a ejecutar el programa.\n");
            exit(0);
        }
        else {
            exit(1);
        }
    }

    return f;
}

bool leerCodigo(FILE* fin, CodigoBarras* codigo) {
    char linea[MAX_LINE];
    if (!fgets(linea, MAX_LINE, fin)) return false;

    // Eliminar salto de línea
    linea[strcspn(linea, "\n")] = '\0';

    // Verificar si la línea es exactamente "0"
    if (strcmp(linea, "0") == 0) return false;

    // Procesar dígitos como antes
    int j = 0;
    for (int i = 0; linea[i] != '\0'; ++i) {
        if (linea[i] >= '0' && linea[i] <= '9') {
            codigo->digitos[j++] = linea[i];
        }
    }

    codigo->digitos[j] = '\0';
    codigo->longitud = j;
    codigo->tipo = (j <= 8) ? EAN8 : EAN13;
    return true;
}

// Agrega ceros a la izquierda si el codigo es mas corto de lo que deberia
void completarCeros(CodigoBarras* codigo) {
    int requerido = (codigo->tipo == EAN8) ? 8 : 13;
    if (codigo->longitud < requerido) {
        char temp[MAX_DIGITS + 1];
        int ceros = requerido - codigo->longitud;
        memset(temp, '0', ceros);
        strcpy(temp + ceros, codigo->digitos);
        strcpy(codigo->digitos, temp);
        codigo->longitud = requerido;
    }
}

// Compara el digito de control real con el calculado
bool validarDigitoControl(const CodigoBarras* codigo) {
    int calculado = calcularDigitoControlRecursivo(codigo);
    int real = codigo->digitos[codigo->longitud - 1] - '0';
    return calculado == real;
}

// Suma los digitos con pesos alternos 1 y 3 segun su posicion desde la derecha
int sumaRecursiva(const char* digitos, int index, int longitud) {
    if (index >= longitud) return 0;
    int posDesdeDerecha = longitud - index;
    int factor = (posDesdeDerecha % 2 == 1) ? 3 : 1;
    int valor = (digitos[index] - '0') * factor;
    return valor + sumaRecursiva(digitos, index + 1, longitud);
}

// Calcula el digito de control esperado segun el algoritmo EAN
int calcularDigitoControlRecursivo(const CodigoBarras* codigo) {
    int suma = sumaRecursiva(codigo->digitos, 0, codigo->longitud - 1);
    int resto = suma % 10;
    return (resto == 0) ? 0 : (10 - resto);
}

// Busca el pais asociado a un codigo EAN-13 a partir de sus prefijos
const char* obtenerPais(const CodigoBarras* codigo) {
    char prefijo3[4], prefijo2[3];
    strncpy(prefijo3, codigo->digitos, 3);
    prefijo3[3] = '\0';
    strncpy(prefijo2, codigo->digitos, 2);
    prefijo2[2] = '\0';

    for (int i = 0; i < sizeof(paises) / sizeof(paises[0]); ++i) {
        if (strcmp(paises[i].prefijo, prefijo3) == 0 || strcmp(paises[i].prefijo, prefijo2) == 0) {
            return paises[i].nombre;
        }
    }

    return "Desconocido";
}

// Imprime si el codigo es valido o no, y muestra el pais si aplica
void imprimirResultado(FILE* fout, const CodigoBarras* codigo, bool esValido, const char* pais) {
    if (esValido) {
        if (codigo->tipo == EAN13 && pais) {
            printf("SI %s\n", pais);
            fprintf(fout, "SI %s\n", pais);
        }
        else {
            printf("SI\n");
            fprintf(fout, "SI\n");
        }
    }
    else {
        printf("NO\n");
        fprintf(fout, "NO\n");
    }
}

// Dibuja el codigo de barras en una ventana grafica usando Allegro
void graficarCodigoBarras(const CodigoBarras* codigo, ALLEGRO_DISPLAY* display) {
    ALLEGRO_FONT* font = al_create_builtin_font();
    al_clear_to_color(al_map_rgb(255, 255, 255)); // Fondo blanco

    int x = 50;
    int y = 50;
    int anchoBarra = 10;
    int altoBarra = 80;
    int n = codigo->longitud;

    for (int i = 0; i < n; ++i) {
        int valor = codigo->digitos[i] - '0';
        ALLEGRO_COLOR color = (valor % 2 == 0) ? al_map_rgb(0, 0, 0) : al_map_rgb(128, 128, 128);

        // Dibuja la barra
        al_draw_filled_rectangle(x + i * anchoBarra, y,
            x + (i + 1) * anchoBarra - 2,
            y + altoBarra, color);

        // Muestra el digito debajo de cada barra
        char num[2] = { codigo->digitos[i], '\0' };
        al_draw_text(font, al_map_rgb(0, 0, 0),
            x + i * anchoBarra + anchoBarra / 2,
            y + altoBarra + 5,
            ALLEGRO_ALIGN_CENTRE, num);
    }

    // Muestra el titulo con el tipo de codigo
    char titulo[64];
    sprintf(titulo, "Codigo EAN-%d: %s", n, codigo->digitos);
    al_draw_text(font, al_map_rgb(0, 0, 255), 200, 10, ALLEGRO_ALIGN_CENTRE, titulo);

    // Agrega una leyenda en la parte inferior para continuar
    al_draw_text(font, al_map_rgb(150, 150, 150), 200, 160, ALLEGRO_ALIGN_CENTRE,
        "Presiona ESC, ENTER o ESPACIO para continuar");

    al_flip_display(); // Muestra todo lo dibujado en pantalla

    // Espera hasta que el usuario presione una tecla valida
    while (true) {
        ALLEGRO_KEYBOARD_STATE teclado;
        al_get_keyboard_state(&teclado);
        if (al_key_down(&teclado, ALLEGRO_KEY_ESCAPE) ||
            al_key_down(&teclado, ALLEGRO_KEY_ENTER) ||
            al_key_down(&teclado, ALLEGRO_KEY_SPACE)) {
            break;
        }
        al_rest(0.05); // Espera para evitar usar CPU innecesariamente
    }

    al_destroy_font(font); // Libera el recurso de la fuente al salir
}
