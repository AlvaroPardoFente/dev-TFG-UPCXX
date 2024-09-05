# Uso:
## Compilación:
Para compilar todos los benchmarks:
```
make all 
```
Para compilar un binario específico, se puede señalar con el nombre del archivo sin la extensión. Ejemplo:
```
make mpi_gather
```
Los archivos ejecutables se encuentran en el directorio generado ```build/```
## Ejecución:
Hay una plantilla de script ```submit_jobs.sh``` en el directorio raíz. En el directorio ```results/```, hay un directorio por cada benchmark con su propia versión de ```submit_jobs.sh```. Dentro del script se pueden ajustar los parámetros principales de las pruebas, así como el número de repeticiones y el número de procesos y nodos. Para ejecutar las pruebas en uno de los repositorios de results:
```
./submit_jobs.sh
```
Si no se desea ejecutar todas las pruebas de esta forma, hay un script para MPI y uno para UPC++ que se pueden utilizar directamente en slurm. Estos son a su vez utilizados por los scripts ```submit_jobs.sh```.
UPC++:
```
sbatch -N <nodos> -n <procesos> --ntasks-per-node=<procesos por nodo> -c 1 -t <tiempo esperado> -p compute2 -o upcxx_reduce_array.txt ./new_upcxx_sbatch.sh <args> <path ejecutable> <args>
```
MPI:
```
sbatch -N <nodos> -n <procesos> --ntasks-per-node=<procesos por nodo> -c 1 -t <tiempo esperado> -p compute2 -o upcxx_reduce_array.txt ./new_mpi_sbatch.sh <args> <path ejecutable> <args>
```

Estos scripts pueden utilizar como argumento un archivo con un valor en cada línea. En los scripts ```submit_jobs.sh``` se hace automáticamente y se pueden utilizar de ejemplo, utilizando a menudo el argumento ```--value=sizes.txt```. El script ejecutará el benchmark una vez por cada valor en el archivo ```sizes.txt```, usándolo como argumento del ejecutable ```--value <sizes[i]>```.
En ambos casos los scripts están diseñados para el cluster Pluton y Slurm. En otros entornos puede ser necesario editarlos.

Los binarios se pueden ejecutar directamente en local.
UPC++:
```
upcxx-run -np <nº procesos> <ejecutable> <args>
```
MPI:
```
mpirun -np <nº procesos> <ejecutable> <args>
```

### Argumentos generales de los ejecutables:
1. --value <number>: Argumento genérico cuyo significado puede depender del benchmark. Generalmente, es el tamaño de bloque que se comunica en número de elementos o el número de iteraciones de un bucle.
El valor puede ser un entero o un entero seguido de K, K, M, M, G o B.
2. --repetitions <number>: El número de veces que se repite la prueba. Cada iteración ejecuta la prueba, mide el tiempo y reinicia las variables que haya modificado.
3. --no-warmup: Por defecto, se ejecutan algunas iteraciones como warmup, sin medición de tiempos. Con este flag se desactivan.
4. --warmup-repetitions <number>: El número de repeticiones de warmup que se realizan. Por defecto son 10.
5. -q: Elimina información adicional del resultado y convierte la salida a formato csv. Este argumento es necesario para procesar los resultados con los scripts de MATLAB.
6. --measurement-mode / -m: Determina qué proceso mide el tiempo de ejecución del benchmark. Opciones:
     - root: El comportamiento por defecto. El proceso de rango 0 hace todas las mediciones.
     - max: El tiempo medido en cada repetición es el máximo de todos los procesos.
     - min: El tiempo medido en cada repetición es el mínimo de todos los procesos.
     - max: Se calcula la media de tiempo para todos los procesos en cada repetición. Este argumento no se ha utilizado en las pruebas porque se han procesado los datos en MATLAB.
     - all: El resultado incluye las mediciones de tiempo de todos los procesos. En el caso del formato csv, la columna Rank indica el rango del proceso de cada entrada.
