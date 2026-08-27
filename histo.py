from pathlib import Path

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

BASE_DIR = Path(__file__).resolve().parent

# Lista de dimensiones a graficar
dimensiones = [10, 50, 100, 500, 1000, 2000, 5000]

# Configuración específica para cada dimensión
configuracion = {
    10: {"bin_size": 0.25, "xlim_max": 2, "ylim_max": 1300},  
    50: {"bin_size": 0.5, "xlim_max": 4, "ylim_max": 1300},
    100: {"bin_size": 1, "xlim_max": 5, "ylim_max": 1500},
    500: {"bin_size": 1, "ylim_max": 1400},
    1000: {"bin_size": 2, "ylim_max": 1500},
    2000: {"bin_size": 2, "xlim_max": 20, "ylim_max": 1500},
    5000: {"bin_size": 2, "xlim_max": 30, "ylim_max": 1500},
}

for d in dimensiones:
    ruta_csv = BASE_DIR / f"distancias_{d}.csv"
    if not ruta_csv.exists():
        raise FileNotFoundError(
            f"No se encontró {ruta_csv}. Ejecuta lab2EDA.cpp con dimensión {d}."
        )

    data = pd.read_csv(ruta_csv)

    bin_size = configuracion[d]["bin_size"]
    xlim_max = configuracion[d].get("xlim_max", data["Distancia"].max())  # Si no está definido, usa el máximo
    ylim_max = configuracion[d]["ylim_max"]  

    plt.figure(figsize=(9, 5))

    plt.hist(data["Distancia"], bins=10, color='g', edgecolor="white")

    plt.xlabel("distancia ")
    plt.ylabel("frecuencia ")
    plt.title(f"dimension:  d={d}")

    plt.xlim(0, xlim_max)

    # Fijar el límite del eje Y
    plt.ylim(0, ylim_max)

    # Ajustar marcas en el eje X
    plt.xticks(np.arange(0, xlim_max + bin_size, bin_size))

     # Agregar cuadrícula con líneas punteadas
    plt.grid(True, linestyle="-", alpha=0.6)

    # Guardar la figura antes de mostrarla
    plt.savefig(BASE_DIR / f"histograma_{d}.png", dpi=300, bbox_inches='tight')

    # Mostrar el gráfico
    # Cerrar la figura para liberar memoria
    plt.close()
