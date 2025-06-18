//Ejercicio #4 – El bar
#include <iostream>

using namespace std;

struct Datos{
	char categoria;
	float valor;
	float ventaMax=0,ventaMin=1000;
	char catAux;
	float valAux;
};


int menu();

int main(){
	menu();
}

int menu(){
	Datos barDatos;
	cout <<"\tBAR -EL PEPE-"<<endl;
	cout << "Se debe de tener la lista de las ventas en un archivo de texto llamado bar.txt"<<endl;
	cout<< "Se indica que para indicar que termino el dia, en categoria indicar N y en valor un 0"<<endl;

	FILE *bar = fopen("bar.txt","r");
	if(bar==NULL){
		cout <<"Error al abrir el archivo";
		return 1;
	}else{
		
	cout <<"\tMenu:\n"<<endl;
	
	cout <<"Desayuno - D"<<endl;
	cout <<"Comidas - A"<<endl;
	cout <<"Meriendas - M"<<endl;
	cout <<"Copas - C"<<endl;

	
	while (!feof(bar)){

	fscanf(bar, "%c\t%d", &barDatos.categoria, &barDatos.valor);
		if (barDatos.categoria != 'N' && barDatos.categoria != 'n' ){
		barDatos.valAux= barDatos.valor;
		if(barDatos.valor < barDatos.ventaMin){
			barDatos.ventaMin = barDatos.valor;
			barDatos.catAux = barDatos.categoria;
			}
		if(barDatos.valor > barDatos.ventaMax){
			barDatos.ventaMax = barDatos.valor;
			barDatos.catAux = barDatos.categoria;
			}	
		}
    }
	 fclose(bar);
	}
}
