#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>                    // Biblioteca estándar para entrada y salida estándar
#include <fstream>                     // Para manejar archivos de entrada y salida
#include <sstream>                     // Utilizada para operaciones en cadenas de texto
#include <vector>                      // Ofrece la implementación de contenedores vectoriales
#include <string>                      // Proporciona funciones y clases para manipular cadenas de texto
#include <omp.h>                       // Encabezado para la API de OpenMP (para programación paralela)
#define STB_IMAGE_WRITE_IMPLEMENTATION // Macro para definir la implementación de stb_image_write.h
#include "stb_image_write.h"           // Biblioteca para escribir imágenes en formato JPEG, PNG, BMP, entre otros

using namespace std; // Uso del espacio de nombres estándar de C++

// Función para leer un archivo y almacenar los datos en una matriz
vector<vector<int>> leerArchivo(const string &nombreArchivo)
{
    ifstream archivo(nombreArchivo); // Abrir el archivo especificado en modo lectura
    vector<vector<int>> matriz;      // Matriz para almacenar los datos del archivo

    // Verificar si el archivo está abierto
    if (archivo.is_open())
    {
        string fila; // Cadena para almacenar cada línea del archivo
        // Leer cada línea del archivo hasta el final
        while (getline(archivo, fila))
        {
            vector<int> valoresFila; // Vector para almacenar los valores de cada fila
            stringstream ss(fila);   // Objeto stringstream para procesar la línea

            string valor_str; // Cadena para almacenar cada valor separado por espacios
            // Extraer cada valor de la línea separado por espacios
            while (ss >> valor_str)
            {
                // Verificar si el valor es "*" (indicando un valor faltante)
                if (valor_str == "*")
                {
                    valoresFila.push_back(-1); // Agregar -1 para representar un valor faltante
                }
                else
                {
                    try
                    {
                        int valor = stoi(valor_str); // Convertir la cadena a entero
                        // Verificar si el valor está dentro del rango válido (0-255)
                        if (valor >= 0 && valor <= 255)
                        {
                            valoresFila.push_back(valor); // Agregar el valor a la fila
                        }
                        else
                        {
                            // Valor fuera del rango, marcar como valor inválido (-2)
                            valoresFila.push_back(-2);
                        }
                    }
                    catch (const std::exception &)
                    {
                        // No se pudo convertir a entero, marcar como valor inválido (-2)
                        valoresFila.push_back(-2);
                    }
                }
            }
            matriz.push_back(valoresFila); // Agregar la fila a la matriz
        }
        archivo.close(); // Cerrar el archivo después de leer todas las líneas
    }
    else
    {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl; // Mensaje de error si no se puede abrir el archivo
    }
    return matriz; // Devolver la matriz con los datos leídos del archivo
}

// Función para calcular valores faltantes en el canal rojo de una imagen
vector<vector<int>> calcularValoresFaltantesRojo(vector<vector<int>> &rojo, const vector<vector<int>> &promedio, const vector<vector<int>> &verde, const vector<vector<int>> &azul)
{
    // Iteración sobre las filas de la imagen
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < rojo.size(); ++i)
    {
        // Iteración sobre las columnas de la imagen
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            // Verificar si el valor en el canal rojo es un valor faltante
            if (rojo[i][j] == -1)
            {
                // Obtener los valores de los canales promedio, verde y azul para el píxel actual
                int promedio_ij = promedio[i][j];
                int verde_ij = verde[i][j];
                int azul_ij = azul[i][j];

                // Calcular el valor faltante en el canal rojo usando una fórmula específica
                int resultado = (promedio_ij - 59 * verde_ij - 11 * azul_ij) / 3;

                // Ajustar el valor calculado para mantenerlo dentro del rango válido (0-255)
                resultado = max(0, min(resultado, 255));

                // Asignar el valor calculado al canal rojo para el píxel actual
                rojo[i][j] = resultado;
            }
        }
    }
    // Devolver la matriz actualizada con los valores faltantes en el canal rojo calculados
    return rojo;
}

// Función para calcular valores faltantes en el canal verde de una imagen
vector<vector<int>> calcularValoresFaltantesVerde(vector<vector<int>> &verde, const vector<vector<int>> &promedio, const vector<vector<int>> &rojo, const vector<vector<int>> &azul)
{
    // Iteración sobre las filas de la imagen
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < verde.size(); ++i)
    {
        // Iteración sobre las columnas de la imagen
        for (size_t j = 0; j < verde[i].size(); ++j)
        {
            // Verificar si el valor en el canal verde es un valor faltante
            if (verde[i][j] == -1)
            {
                // Obtener los valores de los canales promedio, rojo y azul para el píxel actual
                int promedio_ij = promedio[i][j];
                int rojo_ij = rojo[i][j];
                int azul_ij = azul[i][j];

                // Calcular el valor faltante en el canal verde usando una fórmula específica
                int resultado = (promedio_ij - 30 * rojo_ij - 11 * azul_ij) / 59;

                // Ajustar el valor calculado para mantenerlo dentro del rango válido (0-255)
                resultado = max(0, min(resultado, 255));

                // Asignar el valor calculado al canal verde para el píxel actual
                verde[i][j] = resultado;
            }
        }
    }
    // Devolver la matriz actualizada con los valores faltantes en el canal verde calculados
    return verde;
}

// Función para calcular valores faltantes en el canal azul de una imagen
vector<vector<int>> calcularValoresFaltantesAzul(vector<vector<int>> &azul, const vector<vector<int>> &promedio, const vector<vector<int>> &rojo, const vector<vector<int>> &verde)
{
    // Iteración sobre las filas de la imagen
#pragma omp parallel for collapse(1)
    for (size_t i = 0; i < azul.size(); ++i)
    {
        // Iteración sobre las columnas de la imagen
        for (size_t j = 0; j < azul[i].size(); ++j)
        {
            // Verificar si el valor en el canal azul es un valor faltante
            if (azul[i][j] == -1)
            {
                // Obtener los valores de los canales promedio, rojo y verde para el píxel actual
                int promedio_ij = promedio[i][j];
                int rojo_ij = rojo[i][j];
                int verde_ij = verde[i][j];

                // Calcular el valor faltante en el canal azul usando una fórmula específica
                int resultado = (promedio_ij - 30 * rojo_ij - 59 * verde_ij) / 11;

                // Ajustar el valor calculado para mantenerlo dentro del rango válido (0-255)
                resultado = max(0, min(resultado, 255));

                // Asignar el valor calculado al canal azul para el píxel actual
                azul[i][j] = resultado;
            }
        }
    }
    // Devolver la matriz actualizada con los valores faltantes en el canal azul calculados
    return azul;
}

// Función para guardar una sección de la imagen en el vector de datos
void guardarSeccionImagen(const vector<vector<int>> &rojo, const vector<vector<int>> &verde, const vector<vector<int>> &azul, const vector<vector<int>> &alfa, vector<unsigned char> &data, int inicio, int fin)
{
    // Cálculo del índice inicial basado en el inicio de la sección
    int index = inicio * rojo[0].size() * 4; // Multiplicado por 4 porque hay 4 canales de color (rojo, verde, azul, alfa)

    // Iteración a través de las filas de la sección asignada a este hilo
    for (int i = inicio; i < fin; ++i)
    {
        // Iteración a través de las columnas de la imagen
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            // Almacenar los valores de cada canal de color en el vector de datos
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

    // Paralelización de la creación de secciones de la imagen utilizando OpenMP
#pragma omp parallel num_threads(numThreads)
    {
        int threadID = omp_get_thread_num();                                    // Obtener el ID del hilo actual
        int sectionSize = alto / numThreads;                                    // Calcular el tamaño de sección para cada hilo
        int inicio = threadID * sectionSize;                                    // Calcular el inicio de la sección para el hilo actual
        int fin = (threadID == numThreads - 1) ? alto : (inicio + sectionSize); // Calcular el fin de la sección

        // Llamada a la función para guardar una sección de la imagen
        guardarSeccionImagen(rojo, verde, azul, alfa, data, inicio, fin);
    }

    // Escribir la imagen en formato JPEG utilizando stbi_write_jpg
    stbi_write_jpg(nombreArchivo.c_str(), ancho, alto, 4, data.data(), 100); // Calidad: 100
}

#endif
