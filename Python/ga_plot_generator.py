#!/usr/bin/env python3
"""
Generator wykresów dla wyników algorytmu genetycznego (TSP)
Ocena 4.0: rozmiar instancji, populacja, mutacja

Wymaga: pandas, matplotlib
Instalacja: pip install pandas matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

# Ustawienia globalne
plt.rcParams['font.size'] = 11
plt.rcParams['axes.titlesize'] = 13
plt.rcParams['axes.labelsize'] = 12
plt.rcParams['legend.fontsize'] = 10

COLORS = {
    'primary': '#2A9D8F',
    'secondary': '#E63946',
    'tertiary': '#457B9D',
    'accent': '#F4A261',
}


def load_data(filename):
    """Wczytaj plik CSV z katalogu skryptu."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    filepath = os.path.join(script_dir, filename)
    if not os.path.exists(filepath):
        print(f"✗ Nie znaleziono pliku: {filepath}")
        sys.exit(1)
    return pd.read_csv(filepath)


# =============================================================================
# 3.0 - Zależność czasu i błędu od rozmiaru instancji
# =============================================================================

def plot_error_vs_size(df, output_dir):
    """Wykres: błąd względny vs rozmiar instancji (best i avg)."""
    df_sorted = df.sort_values('N')

    fig, ax = plt.subplots(figsize=(11, 7))

    ax.plot(df_sorted['N'], df_sorted['AvgError_best_pct'],
            marker='o', markersize=7, linewidth=2,
            color=COLORS['primary'], label='Najlepszy wynik')
    ax.plot(df_sorted['N'], df_sorted['AvgError_avg_pct'],
            marker='s', markersize=6, linewidth=2, linestyle='--',
            color=COLORS['secondary'], label='Średnia z populacji')

    # Linie graniczne z wymagań
    ax.axhline(y=50, color='orange', linestyle=':', alpha=0.6, label='Limit 50% (24<n<74)')
    ax.axhline(y=100, color='red', linestyle=':', alpha=0.6, label='Limit 100% (75<n<449)')

    ax.set_title('Błąd względny GA w zależności od rozmiaru instancji', fontweight='bold')
    ax.set_xlabel('Rozmiar instancji (N)')
    ax.set_ylabel('Średni błąd względny [%]')
    ax.grid(True, linestyle='--', alpha=0.5)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_blad_vs_rozmiar.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_time_vs_size(df, output_dir):
    """Wykres: czas wykonania vs rozmiar instancji."""
    df_sorted = df.sort_values('N')

    fig, ax = plt.subplots(figsize=(11, 7))

    ax.plot(df_sorted['N'], df_sorted['AvgTime_ms'] / 1000,
            marker='o', markersize=7, linewidth=2,
            color=COLORS['tertiary'], label='Czas całkowity')
    ax.plot(df_sorted['N'], df_sorted['AvgBestFoundTime_ms'] / 1000,
            marker='^', markersize=6, linewidth=2, linestyle='--',
            color=COLORS['accent'], label='Czas znalezienia najlepszego')

    ax.axhline(y=900, color='red', linestyle=':', alpha=0.6, label='Limit 15 min')

    ax.set_title('Czas wykonania GA w zależności od rozmiaru instancji', fontweight='bold')
    ax.set_xlabel('Rozmiar instancji (N)')
    ax.set_ylabel('Czas [s]')
    ax.grid(True, linestyle='--', alpha=0.5)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_czas_vs_rozmiar.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_best_found_time_vs_size(df, output_dir):
    """Wykres: czas znalezienia najlepszego wyniku vs rozmiar (słupkowy)."""
    df_sorted = df.sort_values('N')

    fig, ax = plt.subplots(figsize=(12, 7))

    bars = ax.bar(range(len(df_sorted)), df_sorted['AvgBestFoundTime_ms'] / 1000,
                  color=COLORS['tertiary'], alpha=0.8, edgecolor='white', linewidth=0.5)

    ax.set_xticks(range(len(df_sorted)))
    ax.set_xticklabels([f"{row['Instance']}\n(n={row['N']})"
                        for _, row in df_sorted.iterrows()],
                       rotation=45, ha='right', fontsize=9)

    ax.axhline(y=900, color='red', linestyle=':', alpha=0.6, label='Limit 15 min')

    ax.set_title('Czas znalezienia najlepszego rozwiązania GA', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Czas znalezienia najlepszego [s]')
    ax.grid(True, linestyle='--', alpha=0.4, axis='y')
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_czas_best_found.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


# =============================================================================
# 3.5 - Wpływ rozmiaru populacji na czas i błąd
# =============================================================================

def plot_population_error(df, output_dir):
    """Wykres: błąd vs instancja dla różnych rozmiarów populacji."""
    pop_sizes = sorted(df['PopSize'].unique())

    # Sortuj instancje po N
    instances_order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    fig, ax = plt.subplots(figsize=(13, 7))

    colors_pop = [COLORS['secondary'], COLORS['tertiary'], COLORS['primary']]
    markers = ['o', 's', '^']

    for i, pop in enumerate(pop_sizes):
        subset = df[df['PopSize'] == pop]
        # Sortuj wg kolejności instancji
        subset = subset.set_index('Instance').loc[instances_order].reset_index()
        ax.plot(range(len(subset)), subset['AvgError_pct'],
                marker=markers[i], markersize=6, linewidth=1.8,
                color=colors_pop[i], label=f'Populacja = {pop}')

    ax.set_xticks(range(len(instances_order)))
    ax.set_xticklabels(instances_order, rotation=45, ha='right', fontsize=9)

    ax.axhline(y=50, color='orange', linestyle=':', alpha=0.5, label='Limit 50%')
    ax.axhline(y=100, color='red', linestyle=':', alpha=0.5, label='Limit 100%')

    ax.set_title('Wpływ rozmiaru populacji na błąd GA', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Średni błąd względny [%]')
    ax.grid(True, linestyle='--', alpha=0.4)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_populacja_blad.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_population_time(df, output_dir):
    """Wykres: czas vs instancja dla różnych rozmiarów populacji."""
    pop_sizes = sorted(df['PopSize'].unique())
    instances_order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    fig, ax = plt.subplots(figsize=(13, 7))

    colors_pop = [COLORS['secondary'], COLORS['tertiary'], COLORS['primary']]
    markers = ['o', 's', '^']

    for i, pop in enumerate(pop_sizes):
        subset = df[df['PopSize'] == pop]
        subset = subset.set_index('Instance').loc[instances_order].reset_index()
        ax.plot(range(len(subset)), subset['AvgTime_ms'] / 1000,
                marker=markers[i], markersize=6, linewidth=1.8,
                color=colors_pop[i], label=f'Populacja = {pop}')

    ax.set_xticks(range(len(instances_order)))
    ax.set_xticklabels(instances_order, rotation=45, ha='right', fontsize=9)

    ax.axhline(y=900, color='red', linestyle=':', alpha=0.6, label='Limit 15 min')

    ax.set_title('Wpływ rozmiaru populacji na czas GA', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Czas [s]')
    ax.grid(True, linestyle='--', alpha=0.4)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_populacja_czas.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_population_avg_bar(df, output_dir):
    """Wykres słupkowy: średni błąd i czas dla każdego rozmiaru populacji."""
    pop_sizes = sorted(df['PopSize'].unique())

    avg_errors = [df[df['PopSize'] == p]['AvgError_pct'].mean() for p in pop_sizes]
    avg_times = [df[df['PopSize'] == p]['AvgTime_ms'].mean() / 1000 for p in pop_sizes]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

    # Błąd
    bars1 = ax1.bar([str(p) for p in pop_sizes], avg_errors,
                    color=[COLORS['secondary'], COLORS['tertiary'], COLORS['primary']],
                    alpha=0.85, edgecolor='white', linewidth=1.5)
    ax1.set_title('Średni błąd wg rozmiaru populacji', fontweight='bold')
    ax1.set_xlabel('Rozmiar populacji')
    ax1.set_ylabel('Średni błąd względny [%]')
    ax1.grid(True, linestyle='--', alpha=0.4, axis='y')
    for bar, val in zip(bars1, avg_errors):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                 f'{val:.1f}%', ha='center', va='bottom', fontsize=10)

    # Czas
    bars2 = ax2.bar([str(p) for p in pop_sizes], avg_times,
                    color=[COLORS['secondary'], COLORS['tertiary'], COLORS['primary']],
                    alpha=0.85, edgecolor='white', linewidth=1.5)
    ax2.set_title('Średni czas wg rozmiaru populacji', fontweight='bold')
    ax2.set_xlabel('Rozmiar populacji')
    ax2.set_ylabel('Średni czas [s]')
    ax2.grid(True, linestyle='--', alpha=0.4, axis='y')
    for bar, val in zip(bars2, avg_times):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                 f'{val:.0f}s', ha='center', va='bottom', fontsize=10)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_populacja_srednie.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


# =============================================================================
# 4.0 - Wpływ metody mutacji na czas i błąd
# =============================================================================

def plot_mutation_error(df, output_dir):
    """Wykres: błąd vs instancja dla różnych metod mutacji."""
    methods = df['MutationMethod'].unique()
    instances_order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    fig, ax = plt.subplots(figsize=(13, 7))

    colors_mut = [COLORS['primary'], COLORS['secondary']]
    markers = ['o', 's']

    for i, method in enumerate(methods):
        subset = df[df['MutationMethod'] == method]
        subset = subset.set_index('Instance').loc[instances_order].reset_index()
        ax.plot(range(len(subset)), subset['AvgError_pct'],
                marker=markers[i], markersize=6, linewidth=1.8,
                color=colors_mut[i], label=f'Mutacja: {method}')

    ax.set_xticks(range(len(instances_order)))
    ax.set_xticklabels(instances_order, rotation=45, ha='right', fontsize=9)

    ax.axhline(y=50, color='orange', linestyle=':', alpha=0.5, label='Limit 50%')
    ax.axhline(y=100, color='red', linestyle=':', alpha=0.5, label='Limit 100%')

    ax.set_title('Wpływ metody mutacji na błąd GA', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Średni błąd względny [%]')
    ax.grid(True, linestyle='--', alpha=0.4)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_mutacja_blad.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_mutation_time(df, output_dir):
    """Wykres: czas vs instancja dla różnych metod mutacji."""
    methods = df['MutationMethod'].unique()
    instances_order = df.drop_duplicates('Instance').sort_values('N')['Instance'].tolist()

    fig, ax = plt.subplots(figsize=(13, 7))

    colors_mut = [COLORS['primary'], COLORS['secondary']]
    markers = ['o', 's']

    for i, method in enumerate(methods):
        subset = df[df['MutationMethod'] == method]
        subset = subset.set_index('Instance').loc[instances_order].reset_index()
        ax.plot(range(len(subset)), subset['AvgTime_ms'] / 1000,
                marker=markers[i], markersize=6, linewidth=1.8,
                color=colors_mut[i], label=f'Mutacja: {method}')

    ax.set_xticks(range(len(instances_order)))
    ax.set_xticklabels(instances_order, rotation=45, ha='right', fontsize=9)

    ax.axhline(y=900, color='red', linestyle=':', alpha=0.6, label='Limit 15 min')

    ax.set_title('Wpływ metody mutacji na czas GA', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Czas [s]')
    ax.grid(True, linestyle='--', alpha=0.4)
    ax.legend()
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_mutacja_czas.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_mutation_avg_bar(df, output_dir):
    """Wykres słupkowy: porównanie średnich dla metod mutacji."""
    methods = df['MutationMethod'].unique()

    avg_errors = [df[df['MutationMethod'] == m]['AvgError_pct'].mean() for m in methods]
    avg_times = [df[df['MutationMethod'] == m]['AvgTime_ms'].mean() / 1000 for m in methods]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 5))

    colors_bar = [COLORS['primary'], COLORS['secondary']]

    # Błąd
    bars1 = ax1.bar(methods, avg_errors, color=colors_bar, alpha=0.85,
                    edgecolor='white', linewidth=1.5)
    ax1.set_title('Średni błąd wg metody mutacji', fontweight='bold')
    ax1.set_xlabel('Metoda mutacji')
    ax1.set_ylabel('Średni błąd względny [%]')
    ax1.grid(True, linestyle='--', alpha=0.4, axis='y')
    for bar, val in zip(bars1, avg_errors):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                 f'{val:.1f}%', ha='center', va='bottom', fontsize=11)

    # Czas
    bars2 = ax2.bar(methods, avg_times, color=colors_bar, alpha=0.85,
                    edgecolor='white', linewidth=1.5)
    ax2.set_title('Średni czas wg metody mutacji', fontweight='bold')
    ax2.set_xlabel('Metoda mutacji')
    ax2.set_ylabel('Średni czas [s]')
    ax2.grid(True, linestyle='--', alpha=0.4, axis='y')
    for bar, val in zip(bars2, avg_times):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                 f'{val:.0f}s', ha='center', va='bottom', fontsize=11)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_mutacja_srednie.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


def plot_mutation_comparison_by_n(df, output_dir):
    """Wykres: porównanie metod mutacji grupowane wg rozmiaru N."""
    methods = df['MutationMethod'].unique()
    df_sorted = df.sort_values('N')
    instances = df_sorted.drop_duplicates('Instance')['Instance'].tolist()

    fig, ax = plt.subplots(figsize=(13, 7))

    x = np.arange(len(instances))
    width = 0.35

    for i, method in enumerate(methods):
        subset = df[df['MutationMethod'] == method].set_index('Instance').loc[instances]
        offset = (i - 0.5) * width
        bars = ax.bar(x + offset, subset['AvgError_pct'], width,
                      label=f'{method}',
                      color=[COLORS['primary'], COLORS['secondary']][i],
                      alpha=0.8, edgecolor='white', linewidth=0.5)

    ax.set_xticks(x)
    ax.set_xticklabels([f"{inst}\n(n={df[df['Instance']==inst]['N'].iloc[0]})"
                        for inst in instances],
                       rotation=45, ha='right', fontsize=8)

    ax.axhline(y=50, color='orange', linestyle=':', alpha=0.5)
    ax.axhline(y=100, color='red', linestyle=':', alpha=0.5)

    ax.set_title('Porównanie metod mutacji – błąd dla każdej instancji', fontweight='bold')
    ax.set_xlabel('Instancja')
    ax.set_ylabel('Błąd względny [%]')
    ax.grid(True, linestyle='--', alpha=0.3, axis='y')
    ax.legend(title='Metoda mutacji')
    ax.set_ylim(bottom=0)

    plt.tight_layout()
    out = os.path.join(output_dir, 'ga_mutacja_porownanie.png')
    plt.savefig(out, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"  ✓ {out}")


# =============================================================================
# MAIN
# =============================================================================

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(script_dir, 'wykresy')
    os.makedirs(output_dir, exist_ok=True)

    print("=" * 60)
    print("  Generator wykresów – Algorytm Genetyczny (TSP)")
    print("  Ocena: 4.0 (rozmiar + populacja + mutacja)")
    print("=" * 60)

    # --- 3.0: Rozmiar instancji ---
    print("\n[3.0] Zależność od rozmiaru instancji:")
    df_size = load_data('wyniki_size.csv')
    print(f"  Wczytano {len(df_size)} wierszy")
    plot_error_vs_size(df_size, output_dir)
    plot_time_vs_size(df_size, output_dir)
    plot_best_found_time_vs_size(df_size, output_dir)

    # --- 3.5: Populacja ---
    print("\n[3.5] Wpływ rozmiaru populacji:")
    df_pop = load_data('wyniki_populacja.csv')
    print(f"  Wczytano {len(df_pop)} wierszy")
    plot_population_error(df_pop, output_dir)
    plot_population_time(df_pop, output_dir)
    plot_population_avg_bar(df_pop, output_dir)

    # --- 4.0: Mutacja ---
    print("\n[4.0] Wpływ metody mutacji:")
    df_mut = load_data('wyniki_mutacja.csv')
    print(f"  Wczytano {len(df_mut)} wierszy")
    plot_mutation_error(df_mut, output_dir)
    plot_mutation_time(df_mut, output_dir)
    plot_mutation_avg_bar(df_mut, output_dir)
    plot_mutation_comparison_by_n(df_mut, output_dir)

    print("\n" + "=" * 60)
    print(f"  Wykresy zapisano w: {output_dir}/")
    print("=" * 60)
    return 0


if __name__ == "__main__":
    sys.exit(main())
