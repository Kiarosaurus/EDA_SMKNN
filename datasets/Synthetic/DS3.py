import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_blobs

# Centros observados en la imagen
centers = [(3.5, 2), (3, 8.5), (7, 9), (13, 8), (12.5, 3)]
cluster_std = [0.9, 0.8, 0.8, 0.85, 0.9]
samples = [50, 250, 380, 320, 600]

# Generar cada cluster individualmente y agregar etiquetas
dfs = []
for i in range(len(centers)):
    X_i, _ = make_blobs(n_samples=samples[i],
                        centers=[centers[i]],
                        cluster_std=cluster_std[i],
                        random_state=100 + i)
    df_i = pd.DataFrame(X_i, columns=["x", "y"])
    df_i["cluster"] = i
    dfs.append(df_i)

# Concatenar todo
df_total = pd.concat(dfs, ignore_index=True)
X_total = df_total[["x", "y"]].to_numpy()

# Guardar CSV
df_total.to_csv("DS3.csv", index=False)

# Graficar con colores según cluster
plt.figure(figsize=(6, 5))
for label in df_total["cluster"].unique():
    puntos = df_total[df_total["cluster"] == label]
    plt.scatter(puntos["x"], puntos["y"], s=10, label=f"Cluster {label}")
plt.grid(True)
plt.show()