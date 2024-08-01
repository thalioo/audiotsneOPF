import pandas as pd
import json
# Function to read CSV and get unique values from a specified column
def get_unique_species(file_path, column_name='sp'):
    # Read the CSV file into a DataFrame
    df = pd.read_csv(file_path)
    
    # Get unique values from the specified column
    unique_values = df[column_name].unique()
    print(unique_values)
    return unique_values

def write_to_json(data, json_file_path):
    with open(json_file_path, 'w') as json_file:
        json.dump(data.tolist(), json_file, indent=4)

file_path = '/Users/thalia/Antoine/data/euro_cities_recordings/04_dataset-europe-DimRed-PCA-tSNE-UMAP-with-metadata.csv'
unique_species = get_unique_species(file_path)
write_to_json(unique_species, "/Users/thalia/Antoine/audiotsne/ml4a-ofx/apps/AudioTSNEViewer/bin/data/species.json")

print("Unique species:", unique_species)
