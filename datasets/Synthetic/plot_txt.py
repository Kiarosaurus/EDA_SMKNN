import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("./Synthetic/DS7.txt", sep="\t", header=None, names=["x", "y", "label"])

plt.figure(figsize=(6, 6))
for label in df["label"].unique():
    subset = df[df["label"] == label]
    plt.scatter(subset["x"], subset["y"], s=10, label=f"Cluster {label}")

plt.axis("equal")
plt.show()
