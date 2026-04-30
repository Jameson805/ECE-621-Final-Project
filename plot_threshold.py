import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

def main():
    filename = 'threshold_data.csv'
    try:
        df = pd.read_csv(filename)
    except FileNotFoundError:
        print(f"Error: Could not find {filename}. Make sure the C++ simulation has run.")
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
    ax.set_title('Surface Code Error Correction Threshold', fontsize=16)
    
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.grid(True, which="major", ls="-", alpha=0.6)
    ax.grid(True, which="minor", ls="--", alpha=0.3)
    ax.legend(fontsize=12, loc='lower right')

    output_image = 'threshold_plot.pdf'
    plt.tight_layout()
    plt.savefig(output_image, dpi=300)
    print(f"Plot saved successfully to {output_image}")

if __name__ == "__main__":
    main()
