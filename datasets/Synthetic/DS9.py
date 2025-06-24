import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_moons, make_blobs

def generate_single_crescent(n_samples=120, noise=0.15, label=0, seed=0):
    moons, labels = make_moons(n_samples=n_samples * 2, noise=noise, random_state=seed)
    crescent = moons[labels == label]
    return crescent

def rotate(points, angle_rad):
    R = np.array([
        [np.cos(angle_rad), -np.sin(angle_rad)],
        [np.sin(angle_rad),  np.cos(angle_rad)]
    ])
    return points @ R.T

# Sonrisa
smile = generate_single_crescent(n_samples=150, noise=0.11, label=1, seed=40)
smile = smile * [0.9, 0.8] + [-0.85, -0.4]
df_smile = pd.DataFrame(smile, columns=["x", "y"])
df_smile["cluster"] = 0

# Ojo izquierdo
left_eye = generate_single_crescent(label=0, seed=2)
left_eye = left_eye * [0.45, 0.7]
left_eye = rotate(left_eye, angle_rad=+0.2) + [-0.95, 0.1]
df_left = pd.DataFrame(left_eye, columns=["x", "y"])
df_left["cluster"] = 1

# Ojo derecho
right_eye = generate_single_crescent(label=0, seed=2)
right_eye = right_eye * [0.5, 0.7]
right_eye = rotate(right_eye, angle_rad=-0.2) + [0.95, 0.1]
df_right = pd.DataFrame(right_eye, columns=["x", "y"])
df_right["cluster"] = 2

# Nariz
nose, _ = make_blobs(n_samples=50,
                     centers=[[0, 0]],
                     cluster_std=0.1,
                     random_state=40)
df_nose = pd.DataFrame(nose, columns=["x", "y"])
df_nose["cluster"] = 3

# Combinar todos
df_total = pd.concat([df_smile, df_left, df_right, df_nose], ignore_index=True)

# Guardar CSV
df_total.to_csv("DS9.csv", index=False)

# Visualizar
plt.figure(figsize=(6, 5))
for label in df_total["cluster"].unique():
    points = df_total[df_total["cluster"] == label]
    plt.scatter(points["x"], points["y"], s=10, label=f"Cluster {label}")
plt.axis("equal")
plt.show()