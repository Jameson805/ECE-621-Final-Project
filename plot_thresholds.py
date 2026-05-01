import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import glob
import os
import re

def plot_threshold(csv_filepath, pdf_pages=None):
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
    
    ax.set_xlabel('Total Physical Error Rate (p)', fontsize=14)
    ax.set_ylabel('Logical Error Rate (p_L)', fontsize=14)
    
    filename = os.path.basename(csv_filepath)
    title_parts = []
    
    if "ind" in filename: title_parts.append("Independent Noise")
    elif "dep" in filename: title_parts.append("Depolarizing Noise")
    elif "bias" in filename: 
        match = re.search(r'bias([0-9.]+)', filename)
        eta = match.group(1) if match else "Z"
        title_parts.append(f"Biased Noise (\u03B7={eta})")

    if "measRatio0.0_" in filename or "measRatio0_" in filename: 
        title_parts.append("Code Capacity (2D)")
    else:
        match = re.search(r'measRatio([0-9.]+)', filename)
        if match:
            title_parts.append(f"Meas Ratio: {match.group(1)} (3D)")
        else:
            title_parts.append("Phenomenological (3D)")
            
    ax.set_title(f"Surface Code Threshold\n{' | '.join(title_parts)}", fontsize=16)
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.grid(True, which="major", ls="-", alpha=0.6)
    ax.grid(True, which="minor", ls="--", alpha=0.3)
    ax.legend(fontsize=12, loc='lower right')

    output_image = os.path.splitext(csv_filepath)[0] + '.pdf'
    plt.tight_layout()
    plt.savefig(output_image, dpi=300)
    
    if pdf_pages:
        pdf_pages.savefig(fig)
        
    plt.close(fig)
    print(f"  -> Plot saved to {output_image}")

def main():
    results_dir = 'results'
    
    if not os.path.exists(results_dir):
        print(f"Error: Directory '{results_dir}' not found. Run the C++ simulation first.")
        return

    old_pdfs = glob.glob(os.path.join(results_dir, '*.pdf'))
    if old_pdfs:
        print(f"Purging {len(old_pdfs)} old PDF(s) from results directory...")
        for pdf_file in old_pdfs:
            try:
                os.remove(pdf_file)
            except OSError as e:
                print(f"  Error deleting {pdf_file}: {e}")

    csv_files = glob.glob(os.path.join(results_dir, '*.csv'))
    
    if not csv_files:
        print(f"No CSV files found in '{results_dir}'.")
        return
        
    print(f"Found {len(csv_files)} CSV file(s). Generating plots...")
    
    combined_pdf_path = os.path.join(results_dir, 'all_thresholds_combined.pdf')
    
    with PdfPages(combined_pdf_path) as pdf:
        for csv_file in csv_files:
            print(f"Processing {csv_file}...")
            plot_threshold(csv_file, pdf_pages=pdf)
            
    print(f"\nAll plots generated successfully!")
    print(f"Combined PDF saved to: {combined_pdf_path}")

if __name__ == "__main__":
    main()
