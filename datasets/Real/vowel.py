import pandas as pd

input_file = "./Real/vowel.tr-orig-order"

df = pd.read_csv(input_file, header=None)

n_features = df.shape[1] - 1
column_names = [f"X{i+1}" for i in range(n_features)] + ["class"]
df.columns = column_names

df.to_csv("./Real/vowel.csv", index=False)
print("Archivo 'vowel.csv' generado correctamente.")
