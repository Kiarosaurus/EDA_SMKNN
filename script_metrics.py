import subprocess
import os
import platform
import re
import numpy as np
import matplotlib.pyplot as plt
import json

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#                                 CONFIGURACIÓN
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

DATASETS_DIR = "datasets/Real"
K_RANGE = range(2, 31)
THR_RANGE = np.arange(0.1, 2.1, 0.1)
DEFAULT_K = 10
DEFAULT_THR = 1.0
BUILD_DIR = "build"
EXE_NAME = "Debug/Eda_Proyecto.exe" if platform.system() == "Windows" else "main"
EXE_PATH = os.path.join(BUILD_DIR, EXE_NAME)
METRICS_FILE = "metrics.json"

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

def compile_project():
    print("─" * 60)
    if not os.path.exists(BUILD_DIR):
        os.makedirs(BUILD_DIR)

    print("Configurando proyecto con CMake...")
    subprocess.run(["cmake", ".."], cwd=BUILD_DIR, check=True, capture_output=True)

    print("Compilando proyecto...")
    subprocess.run(["cmake", "--build", "."], cwd=BUILD_DIR, check=True, capture_output=True)
    print("Proyecto compilado con éxito.")
    print("─" * 60)

def parse_f1_from_output(output):
    match = re.search(r"F1 Score\s*\(F1\):\s*([0-9.]+)", output)
    if match:
        return float(match.group(1))
    print(f"Advertencia: No se pudo encontrar el F1-Score en la salida.")
    return None

def run_smknn_and_get_f1(csv_path, k, thr):
    final_k = 0  # detectado desde el CSV por el ejecutable
    args = [EXE_PATH, csv_path, str(k), str(thr), str(final_k)]

    try:
        result = subprocess.run(args, check=True, capture_output=True, text=True, encoding='utf-8')
        return parse_f1_from_output(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error ejecutando SMKNN para {os.path.basename(csv_path)} con k={k}, thr={thr}")
        print(e.stderr)
        return None
    except FileNotFoundError:
        print(f"Error: El ejecutable '{EXE_PATH}' no fue encontrado. Asegúrate de compilar el proyecto.")
        exit(1)

def plot_results(results, x_values, x_label, title):
    plt.style.use('default')
    plt.figure(figsize=(12, 8))

    for dataset_name, data in results.items():
        plt.plot(x_values, data, marker='o', linestyle='-', markersize=4, label=dataset_name)

    plt.xlabel(x_label, fontsize=12)
    plt.ylabel("F1 Score (Macro)", fontsize=12)
    plt.title(title, fontsize=16, weight='bold')
    plt.legend(title="Datasets", bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.tight_layout(rect=[0, 0, 0.85, 1])
    plt.show()

def save_metrics_to_json(results_k, results_thr):
    with open(METRICS_FILE, 'w') as f:
        json.dump({
            "vs_k": results_k,
            "vs_thr": results_thr
        }, f, indent=2)

def load_metrics_from_json():
    if not os.path.exists(METRICS_FILE):
        return None, None
    with open(METRICS_FILE, 'r') as f:
        data = json.load(f)
    return data.get("vs_k", {}), data.get("vs_thr", {})

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#                                FLUJO PRINCIPAL
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

if __name__ == "__main__":
    compile_project()

    results_vs_k, results_vs_thr = load_metrics_from_json()

    if results_vs_k is None or results_vs_thr is None:
        results_vs_k = {}
        results_vs_thr = {}

        try:
            csv_files = [os.path.join(DATASETS_DIR, f) for f in os.listdir(DATASETS_DIR) if f.endswith('.csv')]
        except FileNotFoundError:
            print(f"Error: El directorio de datasets '{DATASETS_DIR}' no fue encontrado.")
            exit(1)

        if not csv_files:
            print(f"Error: No se encontraron archivos .csv en '{DATASETS_DIR}'.")
            exit(1)

        print(f"Encontrados {len(csv_files)} datasets para analizar.\n")

        # F1 vs k
        print("\n--- Analizando F1 Score vs. k-Neighbors ---")
        for csv_file in csv_files:
            dataset_name = os.path.basename(csv_file)
            print(f"\nProcesando dataset: {dataset_name}")
            f1_scores = []
            for k in K_RANGE:
                print(f"  Ejecutando con k = {k} (thr = {DEFAULT_THR})... ", end='', flush=True)
                f1 = run_smknn_and_get_f1(csv_file, k, DEFAULT_THR)
                f1_scores.append(f1 if f1 is not None else np.nan)
                print(f"F1 = {f1:.4f}" if f1 is not None else "F1 = Error")
            results_vs_k[dataset_name] = f1_scores

        # F1 vs thr
        print("\n--- Analizando F1 Score vs. Threshold ---")
        for csv_file in csv_files:
            dataset_name = os.path.basename(csv_file)
            print(f"\nProcesando dataset: {dataset_name}")
            f1_scores = []
            for thr in THR_RANGE:
                print(f"  Ejecutando con thr = {thr:.2f} (k = {DEFAULT_K})... ", end='', flush=True)
                f1 = run_smknn_and_get_f1(csv_file, DEFAULT_K, thr)
                f1_scores.append(f1 if f1 is not None else np.nan)
                print(f"F1 = {f1:.4f}" if f1 is not None else "F1 = Error")
            results_vs_thr[dataset_name] = f1_scores

        # Guardar resultados
        print("\n💾 Guardando resultados en metrics.json...")
        save_metrics_to_json(results_vs_k, results_vs_thr)
    else:
        print("Resultados cargados desde metrics.json (no se recalculó nada).")

    print("\nGenerando gráficos...")
    plot_results(results_vs_k, list(K_RANGE), "k-Nearest Neighbors", "Sensibilidad de SMKNN al parámetro 'k'")
    plot_results(results_vs_thr, list(THR_RANGE), "Threshold (thr)", "Sensibilidad de SMKNN al parámetro 'threshold'")
