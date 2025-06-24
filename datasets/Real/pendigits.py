import pandas as pd

# Ruta del archivo .tes
input_file = "./Real/pendigits.tes"

# Leer el archivo, limpiando espacios luego de las comas
df = pd.read_csv(input_file, header=None, skipinitialspace=True)

# Agregar nombres de columnas (X1, X2, ..., X16, class)
n_features = df.shape[1] - 1
column_names = [f"X{i+1}" for i in range(n_features)] + ["class"]
df.columns = column_names

# Guardar como CSV
df.to_csv("./Real/pendigits.csv", index=False)

print("Archivo 'pendigits.csv' generado correctamente.")