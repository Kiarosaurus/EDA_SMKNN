import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Inicializamos listas
dfs = []

# Banda superior
x1 = np.random.uniform(0.1, 0.85, 150)
y1 = np.random.normal(0.8, 0.01, 150)
df1 = pd.DataFrame(np.column_stack((x1, y1)), columns=["x", "y"])
df1["cluster"] = 0
dfs.append(df1)

# Cluster compacto central
x2 = np.random.uniform(0.3, 0.55, 130)
y2 = np.random.normal(0.6, 0.01, 130)
df2 = pd.DataFrame(np.column_stack((x2, y2)), columns=["x", "y"])
df2["cluster"] = 1
dfs.append(df2)

# Banda inferior
x3 = np.random.uniform(0.1, 0.85, 150)
y3 = np.random.normal(0.45, 0.01, 150)
df3 = pd.DataFrame(np.column_stack((x3, y3)), columns=["x", "y"])
df3["cluster"] = 2
dfs.append(df3)

# Cluster compacto inferior
x4 = np.random.uniform(0.35, 0.52, 110)
y4 = np.random.normal(0.3, 0.01, 110)
df4 = pd.DataFrame(np.column_stack((x4, y4)), columns=["x", "y"])
df4["cluster"] = 3
dfs.append(df4)

# Combinar todo
df_total = pd.concat(dfs, ignore_index=True)
X_total = df_total[["x", "y"]].to_numpy()

# Guardar CSV
df_total.to_csv("DS4.csv", index=False)

# Graficar con colores
plt.figure(figsize=(6, 5))
for label in df_total["cluster"].unique():
    puntos = df_total[df_total["cluster"] == label]
    plt.scatter(puntos["x"], puntos["y"], s=10, label=f"Cluster {label}")
plt.grid(True)
plt.show()