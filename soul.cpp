#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <omp.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION // Define esta macro antes de incluir stb_image_write.h
#include "stb_image_write.h"
// #include <opencv2/opencv.hpp>

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

// Función para contar los valores faltantes en una matriz
int contarValoresFaltantes(const vector<vector<int>> &matriz)
{
    int faltantes = 0;
    for (const auto &fila : matriz)
    {
        for (int valor : fila)
        {
            if (valor == -1)
            {
                faltantes++;
            }
        }
    }
    return faltantes;
}

// Depurar este bloque de código, ya que aqui está el calculo de todo
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
                float resultado = (promedio[i][j] - 0.59 * verde[i][j] - 0.11 * azul[i][j]) / 0.3;
                resultado = max(0.0f, min(resultado, 255.0f)); // Ajustar los valores fuera de rango
                rojo[i][j] = static_cast<int>(resultado);
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
                float resultado = (promedio[i][j] - 0.3 * rojo[i][j] - 0.11 * azul[i][j]) / 0.59;
                resultado = max(0.0f, min(resultado, 255.0f)); // Ajustar los valores fuera de rango
                verde[i][j] = static_cast<int>(resultado);
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
                float resultado = (promedio[i][j] - 0.3 * rojo[i][j] - 0.59 * verde[i][j]) / 0.11;
                resultado = max(0.0f, min(resultado, 255.0f)); // Ajustar los valores fuera de rango
                azul[i][j] = static_cast<int>(resultado);
            }
        }
    }
    return azul;
}

// Función para guardar la imagen en formato JPEG
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
}

int main(int argc, char *argv[])
{
    // Verificación de argumentos de línea de comandos
    if (argc != 6)
    {
        cout << "Uso: " << argv[0] << " alfa.txt rojo.txt verde.txt azul.txt promedio.txt" << endl;
        return 1;
    }

    vector<vector<int>> alfa, rojo, verde, azul, promedio;

// Paralelizar la lectura de archivos utilizando OpenMP
#pragma omp parallel sections
    {
#pragma omp section
        {
            alfa = leerArchivo(argv[1]);
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

    // Contar los valores faltantes en cada archivo
    int faltantesAlfa = contarValoresFaltantes(alfa);
    int faltantesRojo = contarValoresFaltantes(rojo);
    int faltantesVerde = contarValoresFaltantes(verde);
    int faltantesAzul = contarValoresFaltantes(azul);

    // Mostrar la cantidad de valores faltantes en cada archivo
    cout << "Valores faltantes en alfa.txt: " << faltantesAlfa << endl;
    cout << "Valores faltantes en rojo.txt: " << faltantesRojo << endl;
    cout << "Valores faltantes en verde.txt: " << faltantesVerde << endl;
    cout << "Valores faltantes en azul.txt: " << faltantesAzul << endl;

    vector<vector<int>> rojoOtrosCanales(rojo.size(), vector<int>(rojo[0].size(), 0));
    vector<vector<int>> verdeOtrosCanales(verde.size(), vector<int>(verde[0].size(), 0));
    vector<vector<int>> azulOtrosCanales(azul.size(), vector<int>(azul[0].size(), 0));

    // Inicializar las matrices verdeOtrosCanales y azulOtrosCanales con el tamaño correcto
    verdeOtrosCanales.resize(verde.size(), vector<int>(verde[0].size(), 0));
    azulOtrosCanales.resize(azul.size(), vector<int>(azul[0].size(), 0));

    // Asignar valores correspondientes de los otros canales a los vectores de otros canales
    for (size_t i = 0; i < rojo.size(); ++i)
    {
        for (size_t j = 0; j < rojo[i].size(); ++j)
        {
            rojoOtrosCanales[i][j] = verde[i][j] + azul[i][j];
            verdeOtrosCanales[i][j] = rojo[i][j] + azul[i][j];
            azulOtrosCanales[i][j] = rojo[i][j] + verde[i][j];
        }
    }

    rojo = calcularValoresFaltantesRojo(rojo, promedio, verde, azul);
    verde = calcularValoresFaltantesVerde(verde, promedio, rojo, azul);
    azul = calcularValoresFaltantesAzul(azul, promedio, rojo, verde);

    // Contar los valores faltantes en cada archivo nuevamente
    faltantesAlfa = contarValoresFaltantes(alfa);
    faltantesRojo = contarValoresFaltantes(rojo);
    faltantesVerde = contarValoresFaltantes(verde);
    faltantesAzul = contarValoresFaltantes(azul);

    // Mostrar la cantidad de valores faltantes en cada archivo nuevamente
    cout << "Valores faltantes en alfa.txt: " << faltantesAlfa << endl;
    cout << "Valores faltantes en rojo.txt: " << faltantesRojo << endl;
    cout << "Valores faltantes en verde.txt: " << faltantesVerde << endl;
    cout << "Valores faltantes en azul.txt: " << faltantesAzul << endl;

    // Procesar y encontrar valores faltantes en la imagen final
    // encontrarValoresFaltantes(promedio);

    // Guardar la imagen resultante
    guardarImagen(rojo, verde, azul, alfa, "imagen_resultante.jpg");

    return 0;
}
