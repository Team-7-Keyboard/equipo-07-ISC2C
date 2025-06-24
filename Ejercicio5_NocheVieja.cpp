#include <iostream>
#include <stdio.h>

using namespace std;

int main() {
    FILE* archivo_entrada;
    fopen_s(&archivo_entrada, "horas.txt", "r");
    if (archivo_entrada == NULL) {
        cerr << "Error al abrir archivo de entrada." << endl;
        return 1;
    }

    FILE* archivo_salida;
    fopen_s(&archivo_salida, "salida.txt", "w");
    if (archivo_salida == NULL) {
        cerr << "Error al abrir archivo de salida." << endl;
        fclose(archivo_entrada);
        return 1;
    }
    cout << "\t NOCHE VIEJA"<<endl;


    int h, m;

    while (fscanf_s(archivo_entrada, "%d:%d", &h, &m) == 2) {
        if (h == 0 && m == 0) break;
        if (h < 0 || h > 23 || m < 0 || m > 59) continue; // Validación extra

        int minutos_pasados = h * 60 + m;
        int minutos_faltantes = 1440 - minutos_pasados;

        cout << minutos_faltantes << endl;
        fprintf(archivo_salida, "%d\n", minutos_faltantes);
    }

    fclose(archivo_entrada);
    fclose(archivo_salida);

    return 0;
}
