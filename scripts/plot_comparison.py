"""Generate the README figure from recorded CSVs: python scripts/plot_comparison.py.
Requires matplotlib; no pricing simulations are rerun.
"""
from pathlib import Path
import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

ROOT = Path(__file__).resolve().parents[1]
rows = []
for n in (20, 40, 80, 120):
    with (ROOT / 'results' / f'mesh{n}.csv').open() as f:
        rows.append(next(csv.DictReader(f)))
# Keep the benchmark identical across all plotted mesh refinements.
keys = ('s1', 's2', 'strike', 'rate', 'maturity_years', 'domain', 'steps',
        'pairs', 'seed', 'mc', 'ci_low', 'ci_high')
assert all(all(row[k] == rows[0][k] for k in keys) for row in rows)
x = [int(r['mesh']) for r in rows]
y = [float(r['fem']) for r in rows]
mc, low, high = (float(rows[0][k]) for k in ('mc', 'ci_low', 'ci_high'))
plt.rcParams.update({'font.family': 'DejaVu Sans', 'font.size': 11,
                     'axes.spines.top': False, 'axes.spines.right': False})
fig, ax = plt.subplots(figsize=(11, 6.3), facecolor='white')
fig.subplots_adjust(left=.12, right=.97, bottom=.23, top=.78)
fig.text(.12, .93, 'Call européen à deux actifs', fontsize=21, weight='bold', color='#142b40')
fig.text(.12, .87, 'Le prix par éléments finis se rapproche de l’estimation Monte-Carlo',
         fontsize=12, color='#435569')
ax.axhspan(low, high, color='#35a287', alpha=.16, label='Intervalle MC à 95 %')
ax.axhline(mc, color='#13836c', linewidth=2, linestyle='--', label=f'Monte-Carlo : {mc:.6f}')
ax.plot(x, y, '-o', color='#3158bf', linewidth=2.5, markersize=7,
        label='Éléments finis (244 pas temporels)')
for xx, yy in zip(x, y):
    ax.annotate(f'{yy:.6f}'.replace('.', ','), (xx, yy), xytext=((0, 14) if xx == 20 else (12, -25) if xx == 40 else (0, -23)),
                textcoords='offset points', ha=('left' if xx == 40 else 'center'), color='#254ba9', fontsize=10)
ax.set(xlabel='Subdivisions du maillage par axe', ylabel='Prix de l’option',
       xticks=x, xlim=(12, 130), ylim=(.11010, .11188))
ax.yaxis.set_major_formatter(FuncFormatter(lambda val, _: f'{val:.4f}'.replace('.', ',')))
ax.grid(axis='y', color='#e1e7ed', linewidth=.8)
ax.set_axisbelow(True)
ax.legend(loc='upper left', frameon=False, fontsize=10)
fig.text(.12, .12, 'S₁ = S₂ = 0,5  •  K = 1  •  r = 5 %  •  T = 2 ans  •  σ₁ = σ₂ = 20 %  •  ρ = −0,6',
         fontsize=10, color='#435569')
fig.text(.12, .075, 'MC : 500 000 paires antithétiques (1 million de trajectoires), graine 42. Domaine EF : [0, 2]².',
         fontsize=9, color='#435569')
fig.text(.12, .035, 'Axe des prix resserré. La bande décrit uniquement l’incertitude MC, pas l’erreur de discrétisation EF.',
         fontsize=9, color='#657587')
out = ROOT / 'docs' / 'images'
out.mkdir(parents=True, exist_ok=True)
fig.savefig(out / 'comparison_fem_mc.png', dpi=160, facecolor='white')
plt.close(fig)
print(out / 'comparison_fem_mc.png')
