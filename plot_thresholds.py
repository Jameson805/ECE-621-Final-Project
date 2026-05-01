import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import glob
import os

def plot_threshold(csv_filepath):
    try:
        df = pd.read_csv(csv_filepath)
    except Exception as e:
        print(f"  Error reading {csv_filepath}: {e}")
        return

    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(8, 6))

    distances = df['d'].unique()
    distances.sort()
    
    colors = plt.cm.viridis(np.linspace(0.1, 0.9, len(distances)))

    for d, color in zip(distances, colors):
        subset = df[df['d'] == d]
        
        ax.plot(
            subset['p'], 
            subset['logical_error_rate'], 
            marker='o',      
            linestyle='-', 
            label=f'd = {d}', 
            color=color,
            linewidth=2,
            markersize=6
        )

    ax.set_xscale('log')
    ax.set_yscale('log')
    
    ax.set_xlabel('Physical Error Rate (p)', fontsize=14)
    ax.set_ylabel('Logical Error Rate (p_L)', fontsize=14)
    
    filename = os.path.basename(csv_filepath)
    model_type = ""
    if "meas1" in filename:
        model_type = " - Phenomenological (3D)"
    elif "meas0" in filename:
        model_type = " - Code Capacity (2D)"
        
    ax.set_title(f'Surface Code Error Correction Threshold{model_type}', fontsize=16)
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.grid(True, which="major", ls="-", alpha=0.6)
    ax.grid(True, which="minor", ls="--", alpha=0.3)
    ax.legend(fontsize=12, loc='lower right')

    output_image = os.path.splitext(csv_filepath)[0] + '.pdf'
    plt.tight_layout()
    plt.savefig(output_image, dpi=300)
    
    plt.close(fig)
    print(f"  -> Plot saved to {output_image}")

def main():
    results_dir = 'results'
    
    if not os.path.exists(results_dir):
        print(f"Error: Directory '{results_dir}' not found. Run the C++ simulation first.")
        return

    csv_files = glob.glob(os.path.join(results_dir, '*.csv'))
    
    if not csv_files:
        print(f"No CSV files found in '{results_dir}'.")
        return
        
    print(f"Found {len(csv_files)} CSV file(s). Generating plots...")
    
    for csv_file in csv_files:
        print(f"Processing {csv_file}...")
        plot_threshold(csv_file)
        
    print("All plots generated successfully!")

if __name__ == "__main__":
    main()
    