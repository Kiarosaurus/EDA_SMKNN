import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_blobs

# Definición de centros (mismos que DS1 pero con distintos tamaños)
centers = [(0, 10), (10, 10), (0, 0), (10, 0)]
cluster_std = [1.7, 2, 1.7, 1.7]  # DS2: uno denso, otros dispersos
samples = [80, 750, 80, 80]          # Aumentamos el denso

# Generar blobs uno por uno
dfs = []
for i, std in enumerate(cluster_std):
    X_i, _ = make_blobs(n_samples=samples[i],
                        centers=[centers[i]],
                        cluster_std=std,
                        random_state=1 + i)
    df_i = pd.DataFrame(X_i, columns=["x", "y"])
    df_i["cluster"] = i  # Asignar cluster
    dfs.append(df_i)

# Concatenar
df_total = pd.concat(dfs, ignore_index=True)
X_total = df_total[["x", "y"]].to_numpy()

# Guardar CSV con cluster
df_total.to_csv("DS2.csv", index=False)

# Graficar sin cambiar nada
plt.figure(figsize=(6, 5))
for label in sorted(df_total["cluster"].unique()):
    puntos = df_total[df_total["cluster"] == label]
    plt.scatter(puntos["x"], puntos["y"], s=10, label=f"Cluster {label}" if label != -1 else "Outliers")
plt.show()