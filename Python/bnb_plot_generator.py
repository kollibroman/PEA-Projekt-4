#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

STYLES = {
    'BnB_DFS':       {'color': '#E63946', 'marker': 'o', 'label': 'B&B DFS'},
    'BnB_BFS':       {'color': '#457B9D', 'marker': 's', 'label': 'B&B BFS'},
    'BnB_BestFirst': {'color': '#2A9D8F', 'marker': '^', 'label': 'B&B Best-First'},
}

def plot_bnb_times(csv_file='bnb_times.csv'):
    if not os.path.exists(csv_file):
        print(f'Brak pliku {csv_file}')
        return False

    df = pd.read_csv(csv_file)
    print(f'Wczytano {len(df)} wierszy z {csv_file}')

    # Wykres 1: sredni czas
    fig, ax = plt.subplots(figsize=(10, 7))
    for algo, s in STYLES.items():
        sub = df[df['Algorithm'] == algo]
        if sub.empty:
            continue
        ax.plot(sub['N'], sub['Avg_Time_ms'], marker=s['marker'], ms=10, lw=2.5, color=s['color'], label=s['label'])
        for _, r in sub.iterrows():
            ax.annotate(f'{r["Avg_Time_ms"]:.1f}', (r['N'], r['Avg_Time_ms']),
                        textcoords='offset points', xytext=(6, 4), fontsize=8, color=s['color'])
    ax.set_title('Sredni czas wykonania algorytmow B&B\n(100 instancji na N, z NN)', fontsize=13, fontweight='bold')
    ax.set_xlabel('N', fontsize=12)
    ax.set_ylabel('Czas [ms]', fontsize=12)
    ax.grid(True, ls='--', alpha=0.6)
    ax.set_xticks(sorted(df['N'].unique()))
    ax.legend(fontsize=11)
    fig.tight_layout()
    fig.savefig('wykres_bnb_czasu.png', dpi=300)
    print('Zapisano: wykres_bnb_czasu.png')
    plt.close(fig)

    # Wykres 2: % timeout
    if 'Timeout_pct' in df.columns:
        fig, ax = plt.subplots(figsize=(10, 7))
        for algo, s in STYLES.items():
            sub = df[df['Algorithm'] == algo]
            if sub.empty:
                continue
            ax.plot(sub['N'], sub['Timeout_pct'], marker=s['marker'], ms=10, lw=2.5, color=s['color'], label=s['label'])
            for _, r in sub.iterrows():
                ax.annotate(f"{r['Timeout_pct']:.0f}%", (r['N'], r['Timeout_pct']),
                            textcoords='offset points', xytext=(6, 4), fontsize=9, color=s['color'])
        ax.set_title('Procent instancji z przekroczonym limitem (300s)\n(100 instancji na N)', fontsize=13, fontweight='bold')
        ax.set_xlabel('N', fontsize=12)
        ax.set_ylabel('Timeout [%]', fontsize=12)
        ax.set_ylim(-5, 105)
        ax.grid(True, ls='--', alpha=0.6)
        ax.set_xticks(sorted(df['N'].unique()))
        ax.legend(fontsize=11)
        fig.tight_layout()
        fig.savefig('wykres_bnb_timeout.png', dpi=300)
        print('Zapisano: wykres_bnb_timeout.png')
        plt.close(fig)

    return True


def plot_initial_solution(csv_file='bnb_initial_solution.csv'):
    if not os.path.exists(csv_file):
        print(f'Brak pliku {csv_file}')
        return False

    df = pd.read_csv(csv_file)
    print(f'Wczytano {len(df)} wierszy z {csv_file}')

    # Wykresy per algorytm: czas z NN vs bez NN
    for algo, s in STYLES.items():
        sub = df[df['Algorithm'] == algo]
        if sub.empty:
            continue
        fig, ax = plt.subplots(figsize=(10, 7))
        ax.plot(sub['N'], sub['Avg_Time_With_NN_ms'], marker='o', ms=10, lw=2.5, color='#2A9D8F', label='Z NN')
        ax.plot(sub['N'], sub['Avg_Time_Without_NN_ms'], marker='s', ms=10, lw=2.5, color='#E63946', label='Bez NN')
        ax.set_title(f'Wplyw rozwiazania poczatkowego — {s["label"]}\n(100 instancji na N)', fontsize=13, fontweight='bold')
        ax.set_xlabel('N', fontsize=12)
        ax.set_ylabel('Czas [ms]', fontsize=12)
        ax.grid(True, ls='--', alpha=0.6)
        ax.set_xticks(sorted(sub['N'].unique()))
        ax.legend(fontsize=11)
        fig.tight_layout()
        fname = f'wykres_bnb_nn_{algo.lower()}.png'
        fig.savefig(fname, dpi=300)
        print(f'Zapisano: {fname}')
        plt.close(fig)

    # Wykres zbiorczy: speedup
    if 'Speedup' in df.columns:
        fig, ax = plt.subplots(figsize=(10, 7))
        for algo, s in STYLES.items():
            sub = df[df['Algorithm'] == algo]
            if sub.empty:
                continue
            ax.plot(sub['N'], sub['Speedup'], marker=s['marker'], ms=10, lw=2.5, color=s['color'], label=s['label'])
            for _, r in sub.iterrows():
                ax.annotate(f"{r['Speedup']:.1f}x", (r['N'], r['Speedup']),
                            textcoords='offset points', xytext=(6, 4), fontsize=9, color=s['color'])
        ax.axhline(y=1.0, color='gray', ls=':', lw=1, alpha=0.7)
        ax.set_title('Przyspieszenie (czas bez NN / czas z NN)\n(100 instancji na N)', fontsize=13, fontweight='bold')
        ax.set_xlabel('N', fontsize=12)
        ax.set_ylabel('Przyspieszenie [x]', fontsize=12)
        ax.grid(True, ls='--', alpha=0.6)
        ax.set_xticks(sorted(df['N'].unique()))
        ax.legend(fontsize=11)
        fig.tight_layout()
        fig.savefig('wykres_bnb_speedup.png', dpi=300)
        print('Zapisano: wykres_bnb_speedup.png')
        plt.close(fig)

    return True


def main():
    print('\n' + '=' * 50)
    print('  Generator wykresow B&B — ATSP')
    print('=' * 50 + '\n')

    ok1 = plot_bnb_times()
    print()
    ok2 = plot_initial_solution()

    print('\n' + '=' * 50)
    if ok1 and ok2:
        print('Wszystkie wykresy wygenerowane.')
    elif ok1 or ok2:
        print('Czesc wykresow wygenerowana (brakuje jednego CSV).')
    else:
        print('Brak plikow CSV — najpierw uruchom eksperymenty.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
