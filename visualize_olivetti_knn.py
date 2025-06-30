import numpy as np
import matplotlib.pyplot as plt
import json
import pandas as pd

# Cargar resultados k-NN
def load_knn_result(json_path='knn_result.json'):
    with open(json_path) as f:
        result = json.load(f)
    return result['query'], set(result['neighbors'])

# Cargar datos Olivetti (asumiendo 4096 columnas de píxeles)
def load_olivetti_data(csv_path='datasets/olivetti/olivetti.csv'):
    # Lee solo las primeras 4096 columnas como float
    data = pd.read_csv(csv_path, header=None, usecols=range(4096), dtype=float).values
    return data

# Visualizar galería de rostros
def plot_gallery(data, query_idx, neighbors, n_rows=10, n_cols=10):
    fig, axes = plt.subplots(n_rows, n_cols, figsize=(12, 12))
    for idx, ax in enumerate(axes.flat):
        if idx >= data.shape[0]:
            ax.axis('off')
            continue
        img = data[idx].reshape(64, 64)
        if idx == query_idx:
            ax.imshow(img, cmap='hot')  # Query en colores cálidos
        elif idx in neighbors:
            ax.imshow(img, cmap='cool_r')  # Vecinos en colores fríos
        else:
            ax.imshow(img, cmap='gray')  # El resto en gris
        ax.axis('off')
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    query_idx, neighbors = load_knn_result('knn_result.json')
    data = load_olivetti_data('datasets/olivetti/olivetti.csv')
    plot_gallery(data, query_idx, neighbors, n_rows=15, n_cols=10)