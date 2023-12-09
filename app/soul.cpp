#include <iostream> // Biblioteca estándar para entrada y salida estándar
#include <fstream>  // Para manejar archivos de entrada y salida
#include <sstream>  // Utilizada para operaciones en cadenas de texto
#include <vector>   // Ofrece la implementación de contenedores vectoriales
#include <string>   // Proporciona funciones y clases para manipular cadenas de texto
#include <omp.h>    // Encabezado para la API de OpenMP (para programación paralela)

#include "funciones.h"

using namespace std; // Uso del espacio de nombres estándar de C++

int main(int argc, char *argv[])
{
    // Vectores para almacenar datos de las imágenes
    vector<vector<int>> alfa, rojo, verde, azul, promedio;

    // Verificación de argumentos de línea de comandos
    if (argc != 6)
    {
        cout << "Uso: " << argv[0] << " alfa.txt rojo.txt verde.txt azul.txt promedio.txt" << endl;
        return 1;
    }

    // Paralelización de la lectura de archivos utilizando OpenMP
#pragma omp parallel sections
    {
#pragma omp section
        {// Leer archivo alfa y almacenar datos en el vector correspondiente
         alfa = leerArchivo(argv[1]);
}

#pragma omp section
{
    // Leer archivo rojo y almacenar datos en el vector correspondiente
    rojo = leerArchivo(argv[2]);
}

#pragma omp section
{
    // Leer archivo verde y almacenar datos en el vector correspondiente
    verde = leerArchivo(argv[3]);
}

#pragma omp section
{
    // Leer archivo azul y almacenar datos en el vector correspondiente
    azul = leerArchivo(argv[4]);
}

#pragma omp section
{
    // Leer archivo promedio y almacenar datos en el vector correspondiente
    promedio = leerArchivo(argv[5]);
}
}

// Cálculo de los valores faltantes en los canales rojo, verde y azul
#pragma omp parallel sections
{
#pragma omp section
    {
        // Calcular valores faltantes en el canal rojo
        rojo = calcularValoresFaltantesRojo(rojo, promedio, verde, azul);
    }

#pragma omp section
    {
        // Calcular valores faltantes en el canal verde
        verde = calcularValoresFaltantesVerde(verde, promedio, rojo, azul);
    }

#pragma omp section
    {
        // Calcular valores faltantes en el canal azul
        azul = calcularValoresFaltantesAzul(azul, promedio, rojo, verde);
    }
}

// Guardar la imagen resultante con los canales rojo, verde, azul y alfa
guardarImagen(rojo, verde, azul, alfa, "galaxia_lejana.jpg");

return 0;
}