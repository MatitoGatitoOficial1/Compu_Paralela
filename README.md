# Trabajo Paralela   ·  Eldoria Data Processing
> Estratos · Matriz Especie–Género–Edad · Gráficas automáticas (OpenMP + Python)

---

## 1. Descripción breve

Este proyecto analiza el megafichero eldoria.csv

| Métrica                     | Salida CSV               | Descripción                                               |
|-----------------------------|--------------------------|-----------------------------------------------------------|
| Conteo por **estrato 0-9**  | `estratos.csv`           | Basado en el primer dígito de **CP Origen**               |
| Histograma **4 × 3 × 100**  | `especie_genero_edad.csv`| 4 especies × 3 géneros × 100 edades (0-99 años)           |
| Gráficas automáticas        | `estratos.png`, `matriz_heatmap_*.png` | Se generan con `resultados.py` usando pandas / matplotlib |

*Todo el pre-procesado está paralelizado con **OpenMP**
---

## 2. Estructura de archivos
Trabajo_Paralela/
├─ Makefile # compila y ejecuta
├─ test.cpp # código C++17 + OpenMP
├─ resultados.py # visualización con pandas & matplotlib
└─ eldoria.csv # 💾 NO incluido pq github no me dejó subirlo

## 3.-Compilación y ejecucion

# 1) compila
make

# 2) ejecuta con 8 hilos (ajustable)
make run THREADS=8
# o directamente   ./programa 8

Esto va a generar: 

estratos.csv
especie_genero_edad.csv
estratos.png
matriz_heatmap_abs.png

## 5. Dependencias

| Parte  | Requisito                                                                   |
| ------ | --------------------------------------------------------------------------- |
| C++    | **g++ ≥ 9** o clang ≥ 11 con soporte OpenMP                                 |
| Python | `pandas`, `matplotlib`, `seaborn` (`pip install pandas matplotlib seaborn`) |


## Detalles de implementación
Lectura streaming: bloques de 100 000 líneas para no sobrecargar ram, se puede modificar pero este programa esta así debido a las limitaciones de mi notebook

mmap + OpenMP: cada hilo procesa su segmento y mantiene contadores locales.

Normalización de cabeceras y campos (minúsculas, tildes fuera, comillas fuera).

Visualización:

estratos.png – barras de conteo.

matriz_heatmap_abs.png – heat-map log-scale (bins de 5 años).

## Limpieza 

make clean   # elimina programa, *.o, *.csv, *.png


