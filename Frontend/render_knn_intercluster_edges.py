import json
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.cm as cm

# ───────────────────────────── Leer datos ──────────────────────────────
json_path = Path(__file__).parent / "visualization.json"
with json_path.open() as f:
    data = json.load(f)

points      = data["points"]  
edges       = data["edges"]   
pivotes     = set(data["pivotes"]) 
clusters    = data["componentes"] 

label = [-1] * len(points)
for cid, cluster in enumerate(clusters):
    for idx in cluster:
        label[idx] = cid

num_clusters = max(1, len(clusters))
cmap = cm.get_cmap("tab20", num_clusters)

def color_for(cid: int):
    return cmap(cid % 20)

# ───────────────────────────── Dibujar ─────────────────────────────────
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_aspect("equal", adjustable="box")
ax.set_title("Aristas inter‑cluster antes del merge (SM‑KNN)")

# 1) Puntos por cluster (más tenues)
for cid, cluster in enumerate(clusters):
    xs = [points[i][0] for i in cluster]
    ys = [points[i][1] for i in cluster]
    ax.scatter(xs, ys, s=20, color=color_for(cid), alpha=0.4, label=f"C{cid+1}", zorder=1)

# 2) Aristas que conectan clusters distintos
for i, j in edges:
    if label[i] == -1 or label[j] == -1:
        continue  # safety
    if label[i] != label[j]:
        xi, yi = points[i]
        xj, yj = points[j]
        ax.plot([xi, xj], [yi, yj], color="red", linewidth=0.8, alpha=0.9, zorder=0)

# 3) Pivotes (opcional, por si quieres verlos)
if pivotes:
    xs, ys = zip(*(points[i] for i in pivotes))
    ax.scatter(xs, ys, s=60, marker="*", color="black", edgecolors="yellow", label="Pivotes", zorder=2)

ax.legend(loc="upper right", bbox_to_anchor=(1.35, 1))
plt.tight_layout()
plt.show()
