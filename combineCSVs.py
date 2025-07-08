import os
import csv
from pathlib import Path

def get_folders_by_numeric_order():
    """Returns a list of folders in the current directory sorted numerically."""
    parent_dir = Path.cwd()
    folders = sorted([f for f in parent_dir.iterdir() if f.is_dir()], key=lambda f: int(f.name))
    return folders

def merge_csv_files():
    """Merges the second line of result.csv files from folders in numeric order into one CSV file,
       prepending the folder name to each row."""
    folders = get_folders_by_numeric_order()
    path_parts = Path.cwd().parts[4:]  # Remove the first 3 parts to keep from the 4th '/' onward
    output_file = "_".join(path_parts) + ".csv"
    
    with open(output_file, 'w', newline='', encoding='utf-8') as outfile:
        writer = csv.writer(outfile)
        
        for folder in folders:
            csv_file = folder / "result.csv"
            if csv_file.exists():
                with open(csv_file, 'r', encoding='utf-8') as infile:
                    reader = csv.reader(infile)
                    next(reader, None)  # Skip the header
                    second_line = next(reader, None)  # Read the second line
                    
                    if second_line:
                        writer.writerow([folder.name] + second_line)  # Prepend folder name and write row

if __name__ == "__main__":
    merge_csv_files()
    print(f"Merged CSV file saved")# as: {output_file}")
