# Telescopio Soul - Programa Paralelo

Este proyecto consiste en un proyecto paralelo para calcular los pixeles faltantes de una imagen de una galaxia lejana, esto en base al sensor del promedio y a los distintos canales de color (ARGB) esto para la asignatura de Computación Paralela en el segundo semestre de 2023.

## Instrucciones de Instalación (Linux)

Sigue estos pasos para configurar y ejecutar el proyecto localmente:

1. Clona el repositorio `git clone https://github.com/axeloon/Telescopio-Soul`. 

2. Copia la carpeta `datos` dentro de la raíz del proyecto    

3. Ejecuta el comando `sudo apt-get install openmpi-bin libopenmpi-dev` para instalar OpenMP y OpenMPI.

4. Ejecuta el comando `ompi_info | less ` para comprobar la instalación y ver información de OpenMPI

5. Reemplaza el archivo `maquinas.txt` por tu hostfile para ejecutar el programa paralelo.

6. Compila la aplicación con `g++ -Wall -Wextra -g3 -fopenmp -Wno-missing-field-initializers app/soul.cpp -o output/soul`

    6.1. Flags
    -Wall: Activa la mayoría de los avisos (warnings) de compilación. Esto incluye advertencias sobre posibles errores de programación o prácticas que podrían no ser seguras.

    -Wextra: Habilita aún más avisos que el -Wall, proporcionando una cobertura más amplia de posibles problemas en el código fuente.

    -g3: Incluye información de depuración adicional en el binario generado. El nivel 3 (-g3) indica un alto nivel de detalle en la información de depuración, lo que puede ser útil para depurar el programa con herramientas como GDB.

    -fopenmp: Habilita el soporte para OpenMP, que es una API utilizada para la programación paralela. Esta bandera permite que el programa utilice directivas de OpenMP para explotar la capacidad de procesamiento en paralelo de los sistemas multicore.

    -Wno-missing-field-initializers: Desactiva un aviso específico relacionado con inicializadores de campos faltantes. Esto suprime las advertencias relacionadas con campos de estructuras o clases que no se inicializan explícitamente en su declaración.

7. Ejecuta el programa con `mpirun -hostfile maquinas.txt ./output/soul datos/alfa.txt datos/rojo.txt datos/verde.txt datos/azul.txt datos/promedio.txt`.

## Requisitos del Sistema

Asegúrate de tener instaladas las siguientes versiones:

- g++: 11.4.0
- OpenMPI: 4.1.2

## Estructura del Proyecto

- **datos/**: Archivos txt de los canales (ARGB)
- **output/**: Binario resultante de la compilación.
- **app/**: Código fuente del programa
    - **funciones.h**: Header con Funciones para Cargar Calcular los pixeles faltantes y cargar la imagen.
    - **stb_image_write.h**: Header con la función encargada para crear la imagen en base a los canales ARGB.
    - **soul.cpp**: Archivo main que paraleliza las funciones.
- **galaxia_lejana.jpg**: Imagen resultante luego de ejecutar el programa
