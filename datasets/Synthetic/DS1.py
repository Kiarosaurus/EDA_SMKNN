import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_blobs

# Parámetros
n_clusters = 4
n_samples_per_cluster = 300
n_outliers = 30
random_state = 42

# Centros predefinidos
centers = [(0, 10), (10, 10), (0, 0), (10, 0)]

# Generar blobs
X, labels = make_blobs(n_samples=n_clusters * n_samples_per_cluster,
                       centers=centers,
                       cluster_std=2,
                       random_state=random_state)

# Crear DataFrame con clusters válidos
df_clusters = pd.DataFrame(X, columns=['x', 'y'])
df_clusters["cluster"] = labels

# Generar outliers aleatorios
x_min, x_max = -5, 15
y_min, y_max = -5, 15
outliers = np.random.uniform(low=(x_min, y_min), high=(x_max, y_max), size=(n_outliers, 2))

# Combinar todo
df_total = pd.concat([df_clusters], ignore_index=True)

# Guardar CSV con cluster
df_total.to_csv("DS1.csv", index=False)

# Graficar por cluster
plt.figure(figsize=(6, 5))
for label in sorted(df_total["cluster"].unique()):
    puntos = df_total[df_total["cluster"] == label]
    plt.scatter(puntos["x"], puntos["y"], s=10, label=f"Cluster {label}" if label != -1 else "Outliers")
plt.axis("equal")
plt.show()