import pandas as pd

# Ruta del archivo .data original
input_file = "./Real/semeion.data"

# Leer el archivo (espacio como separador, sin encabezado)
df = pd.read_csv(input_file, sep=" ", header=None)

# Eliminar columnas vacías que podrían surgir por múltiples espacios
df = df.loc[:, ~df.columns.duplicated()]
df = df.dropna(axis=1, how='all')

# Separar características y etiquetas (últimas 10 columnas son one-hot)
X = df.iloc[:, :-10]
Y = df.iloc[:, -10:]

# Convertir one-hot a etiqueta entera (0–9)
df["label"] = Y.idxmax(axis=1)

# Combinar todo (si quieres las 256 columnas + label al final)
X["label"] = df["label"]

# Guardar como CSV
X.to_csv("semeion.csv", index=False)

print("✅ Archivo 'semeion.csv' generado correctamente.")