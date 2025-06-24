from sklearn.datasets import fetch_olivetti_faces
import numpy as np
import pandas as pd

# 1️⃣ Descargar el dataset completo
data = fetch_olivetti_faces(shuffle=False, download_if_missing=True)
X, y = data.data, data.target  # X: (400, 4096) imágenes aplanadas; y: etiquetas 0–39 :contentReference[oaicite:1]{index=1}

# 2️⃣ Seleccionar solo imágenes de sujetos 0–19 (20 sujetos × 10 imágenes = 200)
mask = y < 20
X_sel = X[mask]
y_sel = y[mask]

print(f"Imágenes seleccionadas: {X_sel.shape[0]} (esperado 200)")

# 3️⃣ Crear DataFrame combinando píxeles + etiqueta de sujeto
pixel_cols = [i for i in range(X_sel.shape[1])]
df = pd.DataFrame(X_sel, columns=pixel_cols)
df["subject"] = y_sel

# 4️⃣ Guardar a CSV
df.to_csv("olivetti.csv", index=False)
print("Guardado: olivetti.csv")
