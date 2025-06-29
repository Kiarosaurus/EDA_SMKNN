import subprocess
import os
import platform

# ─────────────── Configura tus parámetros ───────────────
#csv_path   = "datasets/synthetic/DS3.csv"
csv_path = "datasets/olivetti/olivetti.csv"
k          = 10
threshold  = 1.0
final_k    = 0

visualizations = [
    #"Frontend/render_knn_graph.py",
    #"Frontend/render_knn_split.py",
    #"Frontend/render_knn_intercluster_edges.py",
    #"Frontend/render_knn_merge.py"
    "visualize_olivetti_knn.py"
]

# ─────────────── Preparar compilación con CMake ───────────────
build_dir = "build"
exe_name  = "Debug/Eda_Proyecto.exe" if platform.system() == "Windows" else "main"
exe_path  = os.path.join(build_dir, exe_name)

if not os.path.exists(build_dir):
    os.makedirs(build_dir)

print("🔨 Configurando proyecto con CMake...")
subprocess.run(["cmake", ".."], cwd=build_dir, check=True)

print("🔨 Compilando proyecto...")
subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)

# ─────────────── Ejecutar el binario ───────────────
args = [exe_path, csv_path, str(k), str(threshold), str(final_k)]
print(f"🚀 Ejecutando: {' '.join(args)}")
subprocess.run(args, check=True)

# ─────────────── Ejecutar visualizaciones ───────────────
for script in visualizations:
    print(f"🖼️  Ejecutando visualización: {script}")
    subprocess.run(["python", script], check=True)
