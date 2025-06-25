"""Renderiza los clusters finales después del Merge de SM-KNN.

• Cada cluster en un color distinto.
• Los pivotes se dibujan como estrellas **del mismo color del cluster al que pertenecen** (borde negro para distinguirlos).

Ejecuta:
    python frontend/render_knn_merge.py
"""

import json
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.cm as cm

# ───────────────────────────── Leer datos ──────────────────────────────
json_path = Path(__file__).parent / "visualization.json"
with json_path.open() as f:
    data = json.load(f)

points   = data["points"]          # [[x, y], ...]
edges    = data["edges"]           # [[i, j], ...]
pivotes  = set(data["pivotes"])    # {idx, ...}
clusters = data["componentes"]     # [[idx, idx, ...], ...]

# ─────────────────────── Mapear índice → cluster id ─────────────────────
label = [-1] * len(points)
for cid, cluster in enumerate(clusters):
    for idx in cluster:
        label[idx] = cid

# ───────────────────────── Configurar colores ──────────────────────────
num_clusters = max(1, len(clusters))
cmap = plt.colormaps["tab20"].resampled(num_clusters)

def color_for(cid: int):
    """Devuelve un color estable para el cluster cid (cíclico si >20)."""
    return cmap(cid % 20)

# ───────────────────────────── Dibujar ─────────────────────────────────
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_aspect("equal", adjustable="box")
ax.set_title("Clusters finales tras el merge (SM-KNN)")

# 1) Aristas intra-cluster (suaves y semi-transparentes)
for i, j in edges:
    if i in pivotes or j in pivotes:
        continue  # omitimos aristas con pivotes para no saturar la figura
    cid = label[i]
    if cid != -1 and cid == label[j]:
        xi, yi = points[i]
        xj, yj = points[j]
        ax.plot([xi, xj], [yi, yj], color=color_for(cid), alpha=0.25, zorder=0)

# 2) Puntos por cluster
for cid, cluster in enumerate(clusters):
    xs = [points[i][0] for i in cluster]
    ys = [points[i][1] for i in cluster]
    ax.scatter(xs, ys, s=30, color=color_for(cid), label=f"C{cid + 1}", alpha=0.8, zorder=2)

# 3) Pivotes con el color de su cluster
if pivotes:
    xs = [points[i][0] for i in pivotes]
    ys = [points[i][1] for i in pivotes]
    colors = [color_for(label[i]) for i in pivotes]
    ax.scatter(xs, ys, s=90, marker="*", c=colors, edgecolors="black", linewidths=0.6,
               label="Pivotes", zorder=3)

ax.legend(loc="upper right", bbox_to_anchor=(1.35, 1))
plt.tight_layout()
plt.show()