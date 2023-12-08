#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <omp.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION // Define esta macro antes de incluir stb_image_write.h
#include "stb_image_write.h"

using namespace std;

// Función para leer un archivo y almacenar los datos en una matriz
vector<vector<int>> leerArchivo(const string &nombreArchivo)
{
    ifstream archivo(nombreArchivo);
    vector<vector<int>> matriz;
    if (archivo.is_open())
    {
        string fila;
        while (getline(archivo, fila))
        {
            vector<int> valoresFila;
            stringstream ss(fila);
            string valor_str;
            while (ss >> valor_str)
            {
                if (valor_str == "*")
                {
                    valoresFila.push_back(-1); // Valor faltante
                }
                else
                {
                    try
                    {
                        int valor = stoi(valor_str);
                        if (valor >= 0 && valor <= 255)
                        {
                            valoresFila.push_back(valor);
                        }
                        else
                        {
                            // Valor fuera del rango
                            valoresFila.push_back(-2); // Marcar como valor inválido
                        }
                    }
                    catch (const std::exception &)
                    {
                        // No se pudo convertir a entero
                        valoresFila.push_back(-2); // Marcar como valor inválido
                    }
                }
            }
            matriz.push_back(valoresFila);
        }
        archivo.close();
    }
    else
    {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
    }
    return matriz;
}

// Función para calcular valores faltantes en el canal rojo
vector<vector<int>> calcularValoresFaltantesRojo(vector<vector<int>> &rojo, const vector<vector<int>> &promedio, const vector<vector<int>> &verde, const vector<vector<int>> &azul)
{
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < rojo.size(); ++i)
    {
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            if (rojo[i][j] == -1)
            {
                int promedio_ij = promedio[i][j];
                int verde_ij = verde[i][j];
                int azul_ij = azul[i][j];

                int resultado = (promedio_ij - 59 * verde_ij - 11 * azul_ij) / 3;
                resultado = max(0, min(resultado, 255)); // Ajustar los valores fuera de rango
                rojo[i][j] = resultado;
            }
        }
    }
    return rojo;
}

// Función para calcular valores faltantes en el canal verde
vector<vector<int>> calcularValoresFaltantesVerde(vector<vector<int>> &verde, const vector<vector<int>> &promedio, const vector<vector<int>> &rojo, const vector<vector<int>> &azul)
{
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < verde.size(); ++i)
    {
        for (size_t j = 0; j < verde[i].size(); ++j)
        {
            if (verde[i][j] == -1)
            {
                int promedio_ij = promedio[i][j];
                int rojo_ij = rojo[i][j];
                int azul_ij = azul[i][j];

                int resultado = (promedio_ij - 30 * rojo_ij - 11 * azul_ij) / 59;
                resultado = max(0, min(resultado, 255)); // Ajustar los valores fuera de rango
                verde[i][j] = resultado;
            }
        }
    }
    return verde;
}

// Función para calcular valores faltantes en el canal azul
vector<vector<int>> calcularValoresFaltantesAzul(vector<vector<int>> &azul, const vector<vector<int>> &promedio, const vector<vector<int>> &rojo, const vector<vector<int>> &verde)
{
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < azul.size(); ++i)
    {
        for (size_t j = 0; j < azul[i].size(); ++j)
        {
            if (azul[i][j] == -1)
            {
                int promedio_ij = promedio[i][j];
                int rojo_ij = rojo[i][j];
                int verde_ij = verde[i][j];

                int resultado = (promedio_ij - 30 * rojo_ij - 59 * verde_ij) / 11;
                resultado = max(0, min(resultado, 255)); // Ajustar los valores fuera de rango
                azul[i][j] = resultado;
            }
        }
    }
    return azul;
}

/* // Función para guardar la imagen en formato JPEG
void guardarImagen(const vector<vector<int>> &rojo, const vector<vector<int>> &verde, const vector<vector<int>> &azul, const vector<vector<int>> &alfa, const string &nombreArchivo)
{
    int ancho = rojo[0].size(); // Ancho de la imagen
    int alto = rojo.size();     // Alto de la imagen

    // Crear un vector para almacenar los datos de la imagen
    vector<unsigned char> data(ancho * alto * 4); // 4 canales: rojo, verde, azul, alfa

    // Rellenar el vector de datos con los valores de los canales
    int index = 0;
    for (size_t i = 0; i < rojo.size(); ++i)
    {
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            data[index++] = static_cast<unsigned char>(rojo[i][j]);  // Canal Rojo
            data[index++] = static_cast<unsigned char>(verde[i][j]); // Canal Verde
            data[index++] = static_cast<unsigned char>(azul[i][j]);  // Canal Azul
            data[index++] = static_cast<unsigned char>(alfa[i][j]);  // Canal Alfa
        }
    }

    // Escribir la imagen en formato JPEG
    stbi_write_jpg(nombreArchivo.c_str(), ancho, alto, 4, data.data(), 100); // Calidad: 100
} */

void guardarSeccionImagen(const vector<vector<int>> &rojo, const vector<vector<int>> &verde, const vector<vector<int>> &azul, const vector<vector<int>> &alfa, vector<unsigned char> &data, int inicio, int fin)
{
    int index = inicio * rojo[0].size() * 4;

    for (int i = inicio; i < fin; ++i)
    {
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            data[index++] = static_cast<unsigned char>(rojo[i][j]);  // Canal Rojo
            data[index++] = static_cast<unsigned char>(verde[i][j]); // Canal Verde
            data[index++] = static_cast<unsigned char>(azul[i][j]);  // Canal Azul
            data[index++] = static_cast<unsigned char>(alfa[i][j]);  // Canal Alfa
        }
    }
}

// Función para guardar la imagen en formato JPEG
void guardarImagen(const vector<vector<int>> &rojo, const vector<vector<int>> &verde, const vector<vector<int>> &azul, const vector<vector<int>> &alfa, const string &nombreArchivo)
{
    int ancho = rojo[0].size(); // Ancho de la imagen
    int alto = rojo.size();     // Alto de la imagen

    // Crear un vector para almacenar los datos de la imagen
    vector<unsigned char> data(ancho * alto * 4); // 4 canales: rojo, verde, azul, alfa

    const int numThreads = omp_get_max_threads(); // Obtener el número máximo de hilos

#pragma omp parallel num_threads(numThreads)
    {
        int threadID = omp_get_thread_num();
        int sectionSize = alto / numThreads;
        int inicio = threadID * sectionSize;
        int fin = (threadID == numThreads - 1) ? alto : (inicio + sectionSize);

        guardarSeccionImagen(rojo, verde, azul, alfa, data, inicio, fin);
    }

    // Escribir la imagen en formato JPEG
    stbi_write_jpg(nombreArchivo.c_str(), ancho, alto, 4, data.data(), 100); // Calidad: 100
}

int main(int argc, char *argv[])
{
    vector<vector<int>> alfa, rojo, verde, azul, promedio;

    // Verificación de argumentos de línea de comandos
    if (argc != 6)
    {
        cout << "Uso: " << argv[0] << " alfa.txt rojo.txt verde.txt azul.txt promedio.txt" << endl;
        return 1;
    }

// Paralelizar la lectura de archivos utilizando OpenMP
#pragma omp parallel sections
    {
#pragma omp section
        {alfa = leerArchivo(argv[1]);
}

#pragma omp section
{
    rojo = leerArchivo(argv[2]);
}

#pragma omp section
{
    verde = leerArchivo(argv[3]);
}

#pragma omp section
{
    azul = leerArchivo(argv[4]);
}

#pragma omp section
{
    promedio = leerArchivo(argv[5]);
}
}

// Cálculo de los valores faltantes en los canales rojo, verde y azul
#pragma omp parallel sections
{
#pragma omp section
    {
        rojo = calcularValoresFaltantesRojo(rojo, promedio, verde, azul);
    }

#pragma omp section
    {
        verde = calcularValoresFaltantesVerde(verde, promedio, rojo, azul);
    }

#pragma omp section
    {
        azul = calcularValoresFaltantesAzul(azul, promedio, rojo, verde);
    }
}

// Guardar la imagen resultante
guardarImagen(rojo, verde, azul, alfa, "galaxia_lejana.jpg");

return 0;
}