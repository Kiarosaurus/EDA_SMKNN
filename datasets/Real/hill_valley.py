import pandas as pd

# Ruta del archivo .data original
input_file = "./Real/Hill_Valley_without_noise_Testing.data"

# Leer el archivo usando separador de comas, y con encabezado en la primera fila
df = pd.read_csv(input_file)

# Guardar como CSV (sin índice adicional)
df.to_csv("./Real/hill_valley.csv", index=False)

print("Archivo 'hill_valley.csv' generado correctamente.")