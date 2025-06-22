#include <iostream>
#include <stdio.h>
#define TAM 5
using namespace std;


int main() {
    cout << "\tBAR -EL PEPE-" << endl;
    cout << "Se debe de tener la lista de las ventas en un archivo de texto llamado bar.txt" << endl;
    cout << "Para indicar que termino el dia, en categoria indicar N y en valor un 0" << endl;
	float vector[TAM]={0},valor;
	char categoria;

    FILE *bar = fopen("bar.txt", "r");
    if (bar == NULL) {
        cout << "Error al abrir el archivo" << endl;
        return 1;
	}
    FILE *salida = fopen("salida.txt", "w");
    if (salida == NULL) {
        cout << "Error al abrir el archivo" << endl;
        return 1;
	}
	/*Nota: cada que se vuelva a correr los datos anteriormente
	 almacenados se van a borrar, esto para tener mas claridad en cada dato guardado*/
	while(fscanf(bar,"%c %f",&categoria,&valor)==2){
		if((categoria == 'N' && categoria == 'n')&&valor==0){//Condicion para complobar si se termino el dia ( N 0 )
			
		}
	}
	
	
}

