import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def generate_ring(n_points, radius, jitter):
    angles = np.random.normal(0, 2 * np.pi, n_points)
    radii = np.random.normal(loc=radius, scale=jitter, size=n_points)
    x = 0.42 + radii * np.cos(angles)
    y = 0.42 + radii * np.sin(angles)
    return np.column_stack((x, y))

# Generar los anillos
ring1 = generate_ring(n_points=80, radius=0.003, jitter=0.015)
ring2 = generate_ring(n_points=180, radius=0.12, jitter=0.01)
ring3 = generate_ring(n_points=130, radius=0.30, jitter=0.005)

# Crear DataFrames por cluster
df1 = pd.DataFrame(ring1, columns=["x", "y"])
df1["cluster"] = 0  # Anillo interno

df2 = pd.DataFrame(ring2, columns=["x", "y"])
df2["cluster"] = 1  # Anillo medio

df3 = pd.DataFrame(ring3, columns=["x", "y"])
df3["cluster"] = 2  # Anillo externo

# Combinar todo
df_total = pd.concat([df1, df2, df3], ignore_index=True)
X_total = df_total[["x", "y"]].to_numpy()

# Guardar como CSV
df_total.to_csv("DS6.csv", index=False)

# Graficar con colores por cluster
plt.figure(figsize=(6, 5))
for label in df_total["cluster"].unique():
    puntos = df_total[df_total["cluster"] == label]
    plt.scatter(puntos["x"], puntos["y"], s=10, label=f"Cluster {label}")
plt.axis("equal")
plt.show()