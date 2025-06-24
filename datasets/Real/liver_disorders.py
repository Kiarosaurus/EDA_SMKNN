import pandas as pd

# Ruta del archivo .data original
input_file = "./Real/bupa.data"

# Leer archivo (sin encabezado), separador de coma
df = pd.read_csv(input_file, header=None)

# Eliminar columna 6 (índice 5)
df.drop(columns=5, inplace=True)

# Asignar nombres de columnas (opcional pero recomendable)
# Asumimos que son 6 columnas ahora: 5 features + 1 clase
df.columns = [f"X{i+1}" for i in range(5)] + ["class"]

# Guardar como CSV
df.to_csv("./Real/liver_disorders.csv", index=False)

print("Archivo 'liver_disorders.csv' generado correctamente sin la columna 6 y sin duplicados.")
