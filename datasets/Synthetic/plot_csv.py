import pandas as pd
import matplotlib.pyplot as plt

# Ignorar la primera fila con comillas
df = pd.read_csv("rings.csv", skiprows=1, header=None, names=["x", "y", "id"])

# Convertir a tipos adecuados
df["x"] = df["x"].astype(float)
df["y"] = df["y"].astype(float)
df["id"] = df["id"].astype(int)

# Graficar por ID
plt.figure(figsize=(6, 6))
for label in df["id"].unique():
    cluster = df[df["id"] == label]
    plt.scatter(cluster["x"], cluster["y"], s=10, label=f"Cluster {label}")

plt.title("Rings Dataset (clusters por ID)")
plt.axis("equal")
plt.grid(True)
plt.legend()
plt.show()
