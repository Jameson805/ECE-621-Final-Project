import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import glob
import os
import re

def load_data(filepath):
    try:
        return pd.read_csv(filepath)
    except Exception as e:
        print(f"  Error reading {filepath}: {e}")
        return None

def get_plot_title(filename):
    title_parts = []
    
    if "ind" in filename: title_parts.append("Independent Noise")
    elif "dep" in filename: title_parts.append("Depolarizing Noise")
    elif "bias" in filename: 
        match = re.search(r'bias([0-9.]+)', filename)
        title_parts.append(f"Biased Noise (\u03B7={match.group(1) if match else 'Z'})")

    if "measRatio0.0_" in filename or "measRatio0_" in filename: 
        title_parts.append("Code Capacity (2D)")
    else:
        match = re.search(r'measRatio([0-9.]+)', filename)
        title_parts.append(f"Meas Ratio: {match.group(1)} (3D)" if match else "Phenomenological (3D)")
            
    return f"Surface Code Threshold\n{' | '.join(title_parts)}"

def calculate_threshold_local(df, distances):
    if len(distances) < 2:
        return None, None

    d_max, d_next = distances[-1], distances[-2]
    
    df_max = df[df['d'] == d_max][['p', 'logical_error_rate']]
    df_next = df[df['d'] == d_next][['p', 'logical_error_rate']]
    
    merged = pd.merge(df_max, df_next, on='p', suffixes=('_max', '_next'))
    merged = merged[(merged['logical_error_rate_max'] > 0) & (merged['logical_error_rate_next'] > 0)].sort_values('p')
    
    if merged.empty:
        return None, None

    merged['diff'] = merged['logical_error_rate_max'] - merged['logical_error_rate_next']
    
    crossings = []
    for i in range(len(merged) - 1):
        row1 = merged.iloc[i]
        row2 = merged.iloc[i+1]
        
        if row1['diff'] <= 0 and row2['diff'] > 0:
            crossings.append((row1, row2))
            
    if not crossings:
        return None, None
        
    # If noise causes multiple wobbles/crossings, pick the one at the highest p
    # This guarantees we find the true asymptotic breakdown point.
    row1, row2 = crossings[-1]
    
    log_p1, log_p2 = np.log10(row1['p']), np.log10(row2['p'])
    
    log_pL1_max = np.log10(row1['logical_error_rate_max'])
    log_pL2_max = np.log10(row2['logical_error_rate_max'])
    
    log_pL1_next = np.log10(row1['logical_error_rate_next'])
    log_pL2_next = np.log10(row2['logical_error_rate_next'])
    
    m_max = (log_pL2_max - log_pL1_max) / (log_p2 - log_p1)
    b_max = log_pL1_max - m_max * log_p1
    
    m_next = (log_pL2_next - log_pL1_next) / (log_p2 - log_p1)
    b_next = log_pL1_next - m_next * log_p1
    
    if m_max == m_next: # Parallel lines, shouldn't happen but safe to catch
        return None, None
        
    log_p_th = (b_next - b_max) / (m_max - m_next)
    
    p_th = 10**log_p_th
    p_L_th = 10**(m_max * log_p_th + b_max)
    
    return p_th, p_L_th

def create_plot(df, filepath, pdf_pages):
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(8, 6))

    distances = sorted(df['d'].unique())
    colors = plt.cm.viridis(np.linspace(0.1, 0.9, len(distances)))

    for d, color in zip(distances, colors):
        subset = df[df['d'] == d].sort_values('p')
        ax.plot(subset['p'], subset['logical_error_rate'], marker='o', linestyle='-', label=f'd = {d}', color=color, linewidth=2, markersize=6)

    ax.set_xscale('log')
    ax.set_yscale('log')
    
    # Hide the 1/5000 statistical artifacts
    ax.set_ylim(bottom=1e-3, top=1.5)
        
    p_th, p_L_th = calculate_threshold_local(df, distances)

    if p_th is not None:
        ax.axvline(x=p_th, color='black', linestyle='--', alpha=0.7, zorder=2)
        
        text_str = f"Estimated Threshold\n$p_{{th}} \\approx {p_th:.4f}$"
        ax.text(0.05, 0.95, text_str, transform=ax.transAxes, 
                color='black', fontsize=12, verticalalignment='top',
                bbox=dict(facecolor='white', alpha=0.9, edgecolor='gray', boxstyle='round,pad=0.5'), 
                zorder=4)

    ax.set_xlabel('Total Physical Error Rate (p)', fontsize=14)
    ax.set_ylabel('Logical Error Rate (p_L)', fontsize=14)
    ax.set_title(get_plot_title(os.path.basename(filepath)), fontsize=16)
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.grid(True, which="major", ls="-", alpha=0.6)
    ax.grid(True, which="minor", ls="--", alpha=0.3)
    ax.legend(fontsize=12, loc='lower right')

    output_image = os.path.splitext(filepath)[0] + '.pdf'
    
    plt.tight_layout()
    plt.savefig(output_image, dpi=300)
    
    if pdf_pages:
        pdf_pages.savefig(fig)
        
    plt.close(fig)
    print(f"  -> Plot saved to {output_image}")

def purge_old_pdfs(results_dir):
    old_pdfs = glob.glob(os.path.join(results_dir, '*.pdf'))
    if old_pdfs:
        print(f"Purging {len(old_pdfs)} old PDF(s) from results directory...")
        for pdf_file in old_pdfs:
            try:
                os.remove(pdf_file)
            except OSError as e:
                print(f"  Error deleting {pdf_file}: {e}")

def main():
    results_dir = 'results'
    
    if not os.path.exists(results_dir):
        print(f"Error: Directory '{results_dir}' not found. Run the simulation first.")
        return

    purge_old_pdfs(results_dir)

    csv_files = glob.glob(os.path.join(results_dir, '*.csv'))
    if not csv_files:
        print(f"No CSV files found in '{results_dir}'.")
        return
        
    print(f"Found {len(csv_files)} CSV file(s). Generating plots...")
    combined_pdf_path = os.path.join(results_dir, 'all_thresholds_combined.pdf')
    
    with PdfPages(combined_pdf_path) as pdf:
        for csv_file in csv_files:
            print(f"Processing {csv_file}...")
            df = load_data(csv_file)
            if df is not None:
                create_plot(df, csv_file, pdf)
            
    print(f"\nAll plots generated successfully!")
    print(f"Combined PDF saved to: {combined_pdf_path}")

if __name__ == "__main__":
    main()
