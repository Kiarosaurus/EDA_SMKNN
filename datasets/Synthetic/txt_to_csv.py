import csv

name = "./Synthetic/DS10"

# Leer datos desde archivo .txt
with open(f'{name}.txt', 'r') as txt_file:
    lines = txt_file.readlines()

# Convertir y guardar como .csv
with open(f"{name}.csv", 'w', newline='') as csv_file:
    writer = csv.writer(csv_file)
    for line in lines:
        # Separar por tabulación y escribir como fila CSV
        row = line.strip().split('\t')
        writer.writerow(row)

print(F"{name}.csv guardado con éxito.")