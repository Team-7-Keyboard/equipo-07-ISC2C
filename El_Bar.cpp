//Ejercicio 4 - El Bar
#include <iostream>
#include <stdio.h>
#include <string.h>
#define TAM 5//numero de categorias existentes
using namespace std;


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
	while(fscanf(bar,"%c %f",&categoria,&valor)==2){
		if((categoria == 'N' && categoria == 'n')&&valor==0){//Condicion para complobar si se termino el dia ( N 0 )
			//Caso base (si es N 0) - Fin del dia
			float ventasDia = 0;
			for (int i= 0; i< TAM; i++){
				ventasDia += ventas[i];//se guarda el costo segun la categoria
			}
			float promVentas = ventasDia / TAM;
			
			char categoriaMax,categoriaMin;
			//ahora evaluar si las ventas de comida superan al promedio de ventas del dia
			//ventas en 1 hacen referencia a las ventas conseguidas en las comidas
			char respuesta[2];
			if(ventas[1] > promVentas){
				strcpy(respuesta,"SI");
			}else{
				strcpy(respuesta,"NO");
			}
		//mostrar en pantalla
		cout<<categoriaMax<<"#"<<categoriaMin"#"<<respuesta<<endl;
		
		//escribir en archivo nuevo
		fprint(salida,"%s#%s#%s\n",categoriaMax,categoriaMin,respuesta);
	 
		//inicializar las ventas en 0 para no tener datos almacenados de otros dias
		for(int i = 0; i<TAM; i++){
			ventas[i]=0;
			
		}else{
			//acumilar las ventas de cada categoria segun su codigo
			bool encontrada = false ;
			switch (categoria){
				
				//Caso de los desayunos
				case 'D':
					ventas[0] +=valor;
					encontrada = true;
					break;
					
					
			}
		}

		}
	}

	
}

