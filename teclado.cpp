#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

// Prototipos
void defecto_num3(char*,int&,int);
void defecto_sigMas(char*,int&,int,int,char*,int);
void defecto_sigMenos(char* texto, int &size, int i, int n, char* auxiliar, int &contAux, int &tope, int &tope2);
void defecto_sigAst(char* texto, int &size, int i, int n);
void procesarTexto(char* texto, int &size, int n, char* auxiliar, int &contAux, int &tope, int &tope2);
void mostrar(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* imagen, const char* texto, int delay_ms);


int main(){
    int contAux=0, tope=1, tope2=1, n=100;
    char* texto=(char*)calloc(n, sizeof(char));
    char* auxiliar=(char*)calloc(n, sizeof(char));

    if(texto==NULL || auxiliar==NULL){
        cout<<"Error al reservar memoria"<<endl;
        return 1;
    }

    FILE* archivo=fopen("entrada.txt", "r");
    if(archivo==NULL){
        cout<<"Error. No se pudo abrir el archivo de entrada"<<endl;
        free(texto);
        free(auxiliar);
        return 1;
    }
    
    if (fgets(texto, n, archivo)==NULL){  //fgets nos permite leer una linea
        cout<<"Archivo vacio o error al leer"<<endl;
        fclose(archivo);
        free(texto);
        free(auxiliar);
        return 1;
    }
    fclose(archivo);

    int size_l=strlen(texto);
    if(size_l>0 && texto[size_l-1]=='\n'){ // si la ultima posicion es un salto de linea, lo eliminamos
        texto[size_l-1]='\0';
        size_l--;
    }
    int size=(int)size_l;

    procesarTexto(texto, size, n, auxiliar, contAux, tope, tope2);

    cout<<"Texto procesado: "<<texto<<endl;

    FILE* archivoSalida=fopen("salida.txt", "w");
    if (archivoSalida==NULL){
        cout<<"No se pudo abrir el archivo de salida"<<endl;
        free(texto);
        free(auxiliar);
        return 1;
    }
    fprintf(archivoSalida, "%s\n", texto);
    fclose(archivoSalida);

    // Inicialización de Allegro y addons
    if(!al_init()){
        cout<<"Error. No se pudo inicializar allegro"<<endl;
        free(texto);
        free(auxiliar);
        return 1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    
    ALLEGRO_DISPLAY* display=al_create_display(917,541);
    if(!display){
        cout<<"Error. No se pudo crear la ventana de allegro"<<endl;
        free(texto);
        free(auxiliar);
        return 1;
    }

    ALLEGRO_FONT* font=al_load_ttf_font("Lined.ttf", 70, 0);
    if(!font){
        cout<<"Error. No se pudo cargar la fuente (asegúrate de tener Lined.ttf o arial.ttf en la carpeta del ejecutable)"<<endl;
        al_destroy_display(display);
        free(texto);
        free(auxiliar);
        return 1;
    }

    // Cambia "teclado.png" por el nombre de tu imagen
    ALLEGRO_BITMAP* imagen=al_load_bitmap("teclado.png");
    if (!imagen){
        cout<<"Error. No se pudo cargar la imagen (asegúrate de tener teclado.png en la carpeta del ejecutable)"<<endl;
        al_destroy_font(font);
        al_destroy_display(display);
        free(texto);
        free(auxiliar);
        return 1;
    }
    mostrar(display, font, imagen, texto, 900); // Solo se envia el delay

    // Esperar cierre de ventana
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        cout << "Error al crear la cola de eventos" << endl;
        al_destroy_bitmap(imagen);
        al_destroy_font(font);
        al_destroy_display(display);
        free(texto);
        free(auxiliar);
        return 1;
    }
    al_register_event_source(event_queue, al_get_display_event_source(display));
    bool running=true;
    while(running){
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
            running=false;
        }
    }

    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagen);
    al_destroy_font(font);
    al_destroy_display(display);

    free(texto);
    free(auxiliar);
    return 0;
}

// Implementacion de las funciones (sin cambios)
void defecto_sigMenos(char* texto, int &size, int i, int n, char* auxiliar, int &contAux, int &tope, int &tope2){
    char *cadena1=(char*)malloc(n * sizeof(char));
    char *cadena2=(char*)malloc(n * sizeof(char));
    char *resultado=(char*)calloc(n * 2, sizeof(char));

    if (cadena1==NULL || cadena2==NULL || resultado==NULL){
        cout<<"Error al crear cadenas auxiliares"<<endl;
        return;
    }

    for (int k=0; k<i; k++){ // Copiamos la parte de texto antes del signo de menos
        cadena1[k]=texto[k];
    }
    cadena1[i]='\0'; // Nos aseguramos que termine en nulo

    int indiceCad2 = 0;
    for (int m=i+1; m<size; m++){ // Copiamos la parte de texto despues del signo de menos
        cadena2[indiceCad2]=texto[m];
        indiceCad2++;
        if (texto[m]=='+' && tope==1){ // Evaluamos si hay un signo de mas
            for (int j=m+1; j<size; j++){ // en caso de que haya, copiamos el resto del texto en una cadena auxiliar
                auxiliar[contAux]=texto[j];
                contAux++;
            }
            auxiliar[contAux]='\0'; // Aseguramos que la cadena auxiliar termine en nulo
            tope++; // tope nos indica que ya hemos encontrado un signo de mas
        } 
    }
    cadena2[indiceCad2]='\0';

    if (indiceCad2>0 && cadena2[0]=='3' && tope2==1){  // Si la cadena2 empieza con un 3, lo eliminamos 
        int tam_cadena1=strlen(cadena1);
        if (tam_cadena1>0){
            for (int h=0; h<tam_cadena1-1; h++){ // Desplazamos la primera cadena hacia la izquierda
                cadena1[h]=cadena1[h+1];
            }
            cadena1[tam_cadena1-1]='\0'; 
        }

        for (int k=0; k<indiceCad2-1; k++){ // Desplazamos la segunda cadena hacia la izquierda
            cadena2[k]=cadena2[k+1];
        }
        cadena2[indiceCad2-1]='\0';
        indiceCad2--;

        tope2++; // tope2 nos indica que ya hemos encontrado un 3 al inicio de la cadena2
    }
    strcpy(resultado, cadena2);
    strcat(resultado, cadena1);
    strcpy(texto, resultado);
    size=strlen(texto);

    free(cadena1);
    free(cadena2);
    free(resultado);
}

void defecto_sigAst(char* texto,int &size,int i,int n){
    if(i<size){
        for(int j=i;j<size-1;j++){ // Desplazamos el texto hacia la izquierda quitando el asterisco
            texto[j]=texto[j+1];
        }
        size--;
        texto[size]='\0';
    }
}

void defecto_sigMas(char* texto, int &size, int i, int n, char* auxiliar, int contAux){
    char *cadena1=(char*)calloc(n, sizeof(char)); 
    char *cadena2=(char*)calloc(n, sizeof(char));
    char *resultado=(char*)calloc(n * 2, sizeof(char));

    if(cadena1==NULL || cadena2==NULL || resultado==NULL){
        free(cadena1);
        free(cadena2);
        free(resultado);
        return;
    }

    strncpy(cadena1,texto,i); // Copiamos la parte de texto antes del signo de mas
    cadena1[i]='\0';

    int indiceCad2=0;
    for(int k=i+1;k<size;k++){
        cadena2[indiceCad2++]=texto[k]; // Copiamos la parte de texto despues del signo de mas
    }
    cadena2[indiceCad2]='\0';

    strcpy(resultado,cadena1); // Juntamos las cadenas
    strcat(resultado,cadena2);
    strcat(resultado,auxiliar);

    strcpy(texto,resultado);
    size=strlen(texto); // Ae actualiza el tamano

    for(int k=0;k<n;k++){
        auxiliar[k]='\0'; //Vaciar la cadena auxirl
    }
    contAux=0; // Vuelve a 0 porque ya no se necesita
    free(cadena1);
    free(cadena2);
    free(resultado);
}

void defecto_num3(char* texto,int &size,int i){
    if((i+1)<size&&texto[i+1]=='3'){ // si el siguiente numero es 3 lo eliminamos
        defecto_num3(texto,size,i+1); // llamada recursiva para eliminar 3 consecutivos
    }
    if((i+1)<size){
        for(int j=i+1;j<size-1;j++){ // Desplazamos el texto hacia la izquierda quitando el 3
            texto[j]=texto[j+1];
        }
        size--; // actualizar tamano
        texto[size]='\0';
    }
    if(i<size){
        for(int j=i;j<size-1;j++){
            texto[j]=texto[j+1];
        }
        size--;
        texto[size]='\0';
    }
}

void procesarTexto(char* texto, int &size, int n, char* auxiliar, int &contAux, int &tope, int &tope2) {
    for(int i=0; i<size; i++){
        switch(texto[i]){
            case '3': defecto_num3(texto,size,i);
                      i=-1;
                      break; 
            case '-': defecto_sigMenos(texto,size,i,n,auxiliar,contAux,tope,tope2);
                      i=-1; 
                      break;
            case '*': defecto_sigAst(texto,size,i,n);
                      i=-1; 
                      break;
            case '+': defecto_sigMas(texto,size,i,n,auxiliar,contAux);
                      i=-1; 
                      break;
        }
    }
}

void mostrar(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* imagen, const char* texto, int delay_ms) {
    int len=strlen(texto);
    char buffer[1024]={0}; //buffer es una cadena para mostrar el texto progresivamente

    int ancho_ventana=al_get_display_width(display); // al_get_display_width obtiene el ancho de la ventana
    int alto_ventana=al_get_display_height(display); // height obtiene el alto de la ventana

    int ancho_img=al_get_bitmap_width(imagen); // al_get_bitmap_width obtiene el ancho de la imagen
    int alto_img=al_get_bitmap_height(imagen);

    // Para centrar la imagen en la ventana
    int x_img=(ancho_ventana - ancho_img)/2;
    int y_img=(alto_ventana - alto_img)/2;
    
    for (int i=1; i<=len; i++){
        // Construir cadena parcial (efecto máquina de escribir)
        strncpy(buffer, texto, i);
        buffer[i] = '\0';
        // Dibujar imagen centrada
        al_draw_bitmap(imagen, x_img, y_img, 0);

        // Calcular posicion centrada horizontal para texto
        int ancho_texto=al_get_text_width(font, buffer);
        int x_texto=(ancho_ventana-ancho_texto)/2;
        int y_texto=50; // a 50 pixeles del tope superior

        // Dibujar texto con color negro
        al_draw_text(font, al_map_rgb(0, 0, 0), x_texto, y_texto, 0, buffer);

        // Actualizar para mostrar cambios
        al_flip_display();

        // Retardo
        al_rest(delay_ms/1000.0);
    }
}



