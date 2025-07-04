
#  Makefile — Proyecto Eldoria (por ahora estatos y matriz especie-género-edad)
#  Uso:
#     make          → compila el binario 'programa'
#     make run      → compila y ejecuta
#     make clean    → elimina binario, CSV generados y las imagenes .png
#
#  se puede ajustar THREADS=X en la línea de abajo
# ───────────────────────────────────────────────────────────────

CXX        := g++
CXXFLAGS   := -O2 -Wall -Wextra -pedantic -std=c++17 -fopenmp
TARGET     := programa
SRCS       := test.cpp
OBJS       := $(SRCS:.cpp=.o)

# Número de hilos por defecto, se puede cambiar pero mi notebook no da más
THREADS    ?= 6

# ──────── objetivos ───────

.PHONY: all run clean

all: $(TARGET)

run: $(TARGET)
	@echo
	@./$(TARGET) $(THREADS)
	@echo "-- Generando gráficas…"
	@python3 resultados.py   

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET) estratos.csv especie_genero_edad.csv *.png
