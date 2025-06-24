import pandas as pd
from ucimlrepo import fetch_ucirepo

# Cargar dataset
#soybean_small = fetch_ucirepo(id=91)
#connectionist_bench_sonar_mines_vs_rocks = fetch_ucirepo(id=151) 
#glass_identification = fetch_ucirepo(id=42) 
#ionosphere = fetch_ucirepo(id=52) 
#page_blocks_classification = fetch_ucirepo(id=78) 
#ecoli = fetch_ucirepo(id=39) 
#haberman_s_survival = fetch_ucirepo(id=43) 
#liver_disorders = fetch_ucirepo(id=60) 
pen_based_recognition_of_handwritten_digits = fetch_ucirepo(id=81) 

# Extraer features y targets
#X = soybean_small.data.features
#y = soybean_small.data.targets
#X = connectionist_bench_sonar_mines_vs_rocks.data.features 
#y = connectionist_bench_sonar_mines_vs_rocks.data.targets 
#X = glass_identification.data.features 
#y = glass_identification.data.targets 
#X = ionosphere.data.features 
#y = ionosphere.data.targets 
#X = page_blocks_classification.data.features 
#y = page_blocks_classification.data.targets 
#X = ecoli.data.features 
#y = ecoli.data.targets
#X = haberman_s_survival.data.features 
#y = haberman_s_survival.data.targets
#X = liver_disorders.data.features 
#y = liver_disorders.data.targets 
X = pen_based_recognition_of_handwritten_digits.data.features 
y = pen_based_recognition_of_handwritten_digits.data.targets 

# Combinar en un solo DataFrame
df = pd.concat([X, y], axis=1)  # y contiene la columna con el nombre del cluster

# Guardar en CSV
df.to_csv("pendesign.csv", index=False)

print("Archivo guardado con éxito.")