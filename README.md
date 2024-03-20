# Uso:
## Compilación:
- Se ha de utilizar el argumento -std=c++17 tanto en mpicxx como en upcxx
- Todos los resultados actuales se han compilado en -O3
- Hay una prueba de Makefile en benchmarks/ping_pong, pero no está completa
## Ejecución:
- Ejecutable:
```
mpirun/upcxx-run -np <nº procesos> <ejecutable> <args>
```
- Ejecución en slurm:
```
sbatch -N <nodos> -n <procesos> --ntasks-per-node=<procesos por nodo> -c 1 -t <tiempo esperado> -p compute2 -o upcxx_reduce_array.txt ./new_upcxx_sbatch.sh <path ejecutable> <args>
```
### Argumentos:
1. --value <number>: El significado puede depender del benchmark. Actualmente, es el total de elementos sobre los que se opera en los benchmarks que usan arrays y el número de iteraciones en el benchmark de ping-pong.
El valor puede ser un entero o un entero seguido de K, KB, M, MB, G, GB o B. Por el momento, los valores en múltiplos de bytes solo son multiplicadores (1K == 1 * 1024), no se corresponden con el tamaño en bytes.
2. --repetitions <number>: El número de veces que se repite la prueba. Cada iteración ejecuta la prueba, mide el tiempo y reinicia las variables que haya modificado.
3. --no-warmup: Por defecto, se ejecutan algunas iteraciones como warmup, sin medición de tiempos. Con este flag se desactivan.
# Salida
Algunos resultados de benchmarks en el cluster están guardados en results/
