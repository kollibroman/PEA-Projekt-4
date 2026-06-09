#!/usr/bin/env python3
"""
Generator wykresów dla wyników eksperymentów SA (ATSP)
Wymaga: pandas, matplotlib
Instalacja: pip install pandas matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_error_vs_size(df, output_file='wykres_blad_vs_N.png'):
    """
    3.0: Zależność błędu od rozmiaru instancji
    """
    plt.figure(figsize=(10, 7))

    styles = {
        'Random': {'color': '#E63946', 'marker': 'o', 'linestyle': '--'},
        'NN': {'color': '#2A9D8F', 'marker': 's', 'linestyle': '-'}
    }

    for init_sol in df['InitialSolution'].unique():
        subset = df[df['InitialSolution'] == init_sol].sort_values('N')
        style = styles.get(init_sol, {'color': 'black', 'marker': 'o', 'linestyle': '-'})

        plt.plot(subset['N'], subset['AvgError_pct'],
                marker=style['marker'], markersize=8,
                linestyle=style['linestyle'], linewidth=2,
                color=style['color'], label=f'Rozw. początkowe: {init_sol}')

    plt.axhline(y=30, color='red', linestyle=':', alpha=0.5, label='Limit 30%')
    plt.title('Średni błąd względny SA w zależności od rozmiaru instancji', fontsize=13, fontweight='bold')
    plt.xlabel('Rozmiar instancji (N)', fontsize=12)
    plt.ylabel('Średni błąd względny [%]', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend(fontsize=11)
    plt.ylim(bottom=0)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ {output_file}")


def plot_time_vs_size(df, output_file='wykres_czas_vs_N.png'):
    """
    3.0: Zależność czasu uzyskania najlepszego wyniku od rozmiaru instancji
    """
    plt.figure(figsize=(10, 7))

    styles = {
        'Random': {'color': '#E63946', 'marker': 'o', 'linestyle': '--'},
        'NN': {'color': '#2A9D8F', 'marker': 's', 'linestyle': '-'}
    }

    for init_sol in df['InitialSolution'].unique():
        subset = df[df['InitialSolution'] == init_sol].sort_values('N')
        style = styles.get(init_sol, {'color': 'black', 'marker': 'o', 'linestyle': '-'})

        # Czas w sekundach
        plt.plot(subset['N'], subset['AvgBestTime_ms'] / 1000,
                marker=style['marker'], markersize=8,
                linestyle=style['linestyle'], linewidth=2,
                color=style['color'], label=f'Rozw. początkowe: {init_sol}')

    plt.title('Czas uzyskania najlepszego wyniku SA\nw zależności od rozmiaru instancji', fontsize=13, fontweight='bold')
    plt.xlabel('Rozmiar instancji (N)', fontsize=12)
    plt.ylabel('Czas znalezienia najlepszego [s]', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend(fontsize=11)
    plt.ylim(bottom=0)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ {output_file}")


def plot_initial_solution_comparison(df, output_file='wykres_rozw_poczatkowe.png'):
    """
    3.5: Wpływ rozwiązania początkowego na błąd (wykres słupkowy)
    """
    instances = df['Instance'].unique()
    # Bierzemy unikalne instancje w kolejności rosnącego N
    order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    random_errors = []
    nn_errors = []

    for inst in order:
        r = df[(df['Instance'] == inst) & (df['InitialSolution'] == 'Random')]['AvgError_pct']
        n = df[(df['Instance'] == inst) & (df['InitialSolution'] == 'NN')]['AvgError_pct']
        random_errors.append(r.values[0] if len(r) > 0 else 0)
        nn_errors.append(n.values[0] if len(n) > 0 else 0)

    x = range(len(order))
    width = 0.35

    fig, ax = plt.subplots(figsize=(12, 7))
    bars1 = ax.bar([i - width/2 for i in x], random_errors, width, label='Random', color='#E63946', alpha=0.8)
    bars2 = ax.bar([i + width/2 for i in x], nn_errors, width, label='NN', color='#2A9D8F', alpha=0.8)

    ax.axhline(y=30, color='red', linestyle=':', alpha=0.5, label='Limit 30%')
    ax.set_xlabel('Instancja', fontsize=12)
    ax.set_ylabel('Średni błąd względny [%]', fontsize=12)
    ax.set_title('Wpływ rozwiązania początkowego na błąd SA', fontsize=13, fontweight='bold')
    ax.set_xticks(list(x))
    ax.set_xticklabels(order, rotation=45, ha='right')
    ax.legend(fontsize=11)
    ax.grid(True, linestyle='--', alpha=0.4, axis='y')
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ {output_file}")


def plot_initial_solution_time_comparison(df, output_file='wykres_rozw_poczatkowe_czas.png'):
    """
    3.5: Wpływ rozwiązania początkowego na czas znalezienia najlepszego
    """
    order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    random_times = []
    nn_times = []

    for inst in order:
        r = df[(df['Instance'] == inst) & (df['InitialSolution'] == 'Random')]['AvgBestTime_ms']
        n = df[(df['Instance'] == inst) & (df['InitialSolution'] == 'NN')]['AvgBestTime_ms']
        random_times.append(r.values[0] / 1000 if len(r) > 0 else 0)
        nn_times.append(n.values[0] / 1000 if len(n) > 0 else 0)

    x = range(len(order))
    width = 0.35

    fig, ax = plt.subplots(figsize=(12, 7))
    ax.bar([i - width/2 for i in x], random_times, width, label='Random', color='#E63946', alpha=0.8)
    ax.bar([i + width/2 for i in x], nn_times, width, label='NN', color='#2A9D8F', alpha=0.8)

    ax.set_xlabel('Instancja', fontsize=12)
    ax.set_ylabel('Czas znalezienia najlepszego [s]', fontsize=12)
    ax.set_title('Wpływ rozwiązania początkowego na czas SA', fontsize=13, fontweight='bold')
    ax.set_xticks(list(x))
    ax.set_xticklabels(order, rotation=45, ha='right')
    ax.legend(fontsize=11)
    ax.grid(True, linestyle='--', alpha=0.4, axis='y')
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ {output_file}")


def main():
    csv_file = 'wyniki_batch.csv'

    if not os.path.exists(csv_file):
        # Szukamy w katalogu nadrzednym
        if os.path.exists('../wyniki_batch.csv'):
            csv_file = '../wyniki_batch.csv'
        else:
            print(f"✗ Nie znaleziono {csv_file}")
            print("  Uruchom najpierw eksperymenty (opcja 9 w menu)")
            return 1

    df = pd.read_csv(csv_file)
    print(f"Wczytano {len(df)} wierszy z {csv_file}\n")

    # 3.0: blad vs N, czas vs N
    plot_error_vs_size(df)
    plot_time_vs_size(df)

    # 3.5: porownanie Random vs NN
    plot_initial_solution_comparison(df)
    plot_initial_solution_time_comparison(df)

    print("\nWszystkie wykresy wygenerowane.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
