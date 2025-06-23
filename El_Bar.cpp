//Ejercicio 4 - El Bar
#include <iostream>
#include <stdio.h>
#include <string.h>
#define TAM 5//numero de categorias existentes
using namespace std;

const char* categoriaMax(float []);
const char* categoriaMin(float []);



int main() {
    cout << "\tBAR -EL PEPE-" << endl;
    cout << "Se debe de tener la lista de las ventas en un archivo de texto llamado bar.txt" << endl;
    cout << "Para indicar que termino el dia, en categoria indicar N y en valor un 0" << endl;
	float ventas[TAM]={0},valor;
	/*el vector estara guardado los valores
	 por categoria en cada una de sus casillas*/
	 // 0 - DESAYUNOS / 1 - COMIDAS / 2 - MERIENDAS / 3 - CENAS / 4 - COPAS
	char categoria;
	
	/*Abrir los archivos, del cual se recibe la 
	 informacion es uno llamado bar,
	 el otro archivo sera el que guarde los resultados*/
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
	
	/*Nota: cada que se ejecuta nuevamente se borran los datos 
	 almacenados anteriormente, esto para tener mas claridad en cada dato guardado*/
	while(fscanf(bar," %c %f",&categoria,&valor)==2){
		if((categoria == 'N' || categoria == 'n')&&valor==0){//Condicion para complobar si se termino el dia ( N 0 )
		
			//Caso base (si es N 0) - Fin del dia
			
			float ventasDia = 0;
			for (int i= 0; i< TAM; i++){
				ventasDia += ventas[i];//se guarda el costo segun la categoria
			}
			float promVentas = ventasDia / TAM;
			
			const char* catMax = categoriaMax(ventas);
			const char* catMin = categoriaMin(ventas);
			//ahora evaluar si las ventas de comida superan al promedio de ventas del dia
			//ventas en 1 hacen referencia a las ventas conseguidas en las comidas
			if(!(strcmp(catMax,"EMPATE")==0 && strcmp(catMin,"EMPATE")==0)){//ignorar casos en los que es EMPATE#EMPATE#y
			
			char respuesta[3];
			if(ventas[1] > promVentas){
				strcpy(respuesta,"SI");
			}else{
				strcpy(respuesta,"NO");
			}
		//mostrar en pantalla
		cout<<catMax<<"#"<<catMin<<"#"<<respuesta<<endl;
		//escribir en archivo nuevo
		fprintf(salida,"%s#%s#%s\n",catMax,catMin,respuesta);
		} 
		//inicializar las ventas en 0 para no tener datos almacenados de otros dias
		for(int i = 0; i<TAM; i++){
			ventas[i]=0;
			}
		}else{
			//acumilar las ventas de cada categoria segun su codigo
			switch (categoria){
				
				//Caso de los desayunos
				case 'D':
					ventas[0] +=valor;
					break;
					
				//Caso de los comidas
				case 'A':
					ventas[1] +=valor;
					break;
					
				//Caso de los meriendas
				case 'M':
					ventas[2] +=valor;
					break;
					
				//Caso de los cenas
				case 'I':
					ventas[3] +=valor;
					break;
					
				//Caso de los copas
				case 'C':
					ventas[4] +=valor;
					break;
					
				default:
					break;
				}
			
		}
			//en caso de que no sea N 0
			
	}		
		float ventasDia = 0;
			for (int i= 0; i< TAM; i++){
				ventasDia += ventas[i];//se guarda el costo segun la categoria
			}
			float promVentas = ventasDia / TAM;
			
			const char* catMax = categoriaMax(ventas);
			const char* catMin = categoriaMin(ventas);
			//ahora evaluar si las ventas de comida superan al promedio de ventas del dia
			//ventas en 1 hacen referencia a las ventas conseguidas en las comidas
			if(!(strcmp(catMax,"EMPATE")==0 && strcmp(catMin,"EMPATE")==0)){//ignorar casos en los que es EMPATE#EMPATE#
			char respuesta[3];
			if(ventas[1] > promVentas){
				strcpy(respuesta,"SI");
			}else{
				strcpy(respuesta,"NO");
			}
		
		//mostrar en pantalla
		cout<<catMax<<"#"<<catMin<<"#"<<respuesta<<endl;
		
		//escribir en archivo nuevo
		fprintf(salida,"%s#%s#%s\n",catMax,catMin,respuesta);	
		}
	fclose(bar);
	fclose(salida);
	return 0;
}



//funcion para mostrar categoria maxima
//se declara const porque no se debe de modificar los nombres
const char* categoriaMax(float ventas[]){
	static char nombre[20];
	float valorMax = ventas[0];//se empieza con el valor maximo de la primera posicion 
	int contMax=1;//sirve para detectar si hay empates en los valores maximos
	int indiceMax=0;//indica la posicion en el arreglo del valor maximo
	
	
	for (int i=1; i<TAM; i++){
		if(ventas[i]>valorMax){//evalua si las ventas de cierta posicion son mayores a las guardadas en valorMax
			valorMax = ventas[i];
			indiceMax = i;
			contMax= 1;
			/*en caso de serlo se almacena este como 
			  el valor maximo, se guarda la posicion en la que se
			  encuentra y el contador se queda en uno indicando
			  que solo hay un valor maximo*/
			  
			  
		}else if(ventas[i]==valorMax){//caso si es que es empate entre mayores ventas
			contMax ++;
		}
	}
	/*se evalua si es un empate y en caso de serlo se le
	 asigna la cadena de EMPATE a la variable nombre y se retorna esta variable*/
	if (contMax > 1){
		return"EMPATE";
	}
	
	/*en caso de no ser un empate se 
	retornara el nombre de la categoria con el valor maximo*/
	switch(indiceMax){
		
		case 0:
			return"DESAYUNOS";
			
		case 1:
			return"COMIDAS";
			
		case 2:
			return"MERIENDAS";
			
		case 3:
			return"CENAS";
		
		case 4:
			return"COPAS";
			
		default:
			return"NINGUNO";

	}
}


//funcion para mostrar categoria maxima
//se declara const porque no se debe de modificar los nombres
const char* categoriaMin(float ventas[]){
	static char nombre[20];
	float valorMin = ventas[0];//se empieza con el valor min de la primera posicion 
	int contMin=1;//sirve para detectar si hay empates en los valores minimos
	int indiceMin=0;//indica la posicion en el arreglo del valor minimo
	
	
	for (int i=1; i<TAM; i++){
		if(ventas[i]<valorMin){//evalua si las ventas de cierta posicion son menores a las guardadas en valorMin
			valorMin = ventas[i];
			indiceMin = i;
			contMin= 1;
			/*en caso de serlo se almacena este como 
			  el valor minimo, se guarda la posicion en la que se
			  encuentra y el contador se queda en uno indicando
			  que solo hay un valor minimo*/
			  
			  
		}else if(ventas[i]==valorMin){//caso si es que es empate entre los minimas ventas
			contMin ++;
		}
	}
	/*se evalua si es un empate y en caso de serlo se le
	 asigna la cadena de EMPATE a la variable nombre y se retorna esta variable*/
	if (contMin > 1){
		return "EMPATE";
	}
	
	/*en caso de no ser un empate se 
	retornara el nombre de la categoria con el valor minimo*/
	switch(indiceMin){
		
		case 0:
			return "DESAYUNOS";
			
		case 1:
			return"COMIDAS";
			
		case 2:
			return "MERIENDAS";
			
		case 3:
			return"CENAS";
		
		case 4:
			return"COPAS";
			
		default:
			return"NINGUNO";
	}
}



