# Call européen à deux actifs : éléments finis et Monte-Carlo

Projet C++ de valorisation du payoff **max(S₁(T) + S₂(T) − K, 0)**, sans dividendes.
Les deux pricers utilisent les mêmes paramètres et la même covariance, définie dans
`mat_cov()` (`elementsFinies.cpp`) :

```text
C = [ 0.040  -0.024 ]
    [-0.024   0.040 ]
```

Les volatilités annuelles valent 20 % et la corrélation vaut **−0,6**.
Le coefficient −0,024 est une covariance, pas la corrélation.

## Compilation et comparaison sans interface graphique

Il suffit d'un compilateur C++17 et de Make, sans Qt ni SFML :

```bash
git clone https://github.com/med-rjili/option-europeenne.git
cd option-europeenne
make compare check
./compare_pricers
./compare_pricers --mesh 80 --pairs 500000 --output comparison.csv
```

Depuis un terminal Windows avec MinGW et sans Make :

```powershell
g++ -std=c++17 -O2 compare_pricers.cpp monte_carlo.cpp fem_pricer.cpp maillage.cpp point.cpp triangle.cpp matrice.cpp elementsFinies.cpp -o compare_pricers.exe
.\compare_pricers.exe --mesh 80
```

Options (`./compare_pricers --help`) :

| Option | Défaut | Signification |
|---|---:|---|
| `--s1`, `--s2` | 0.5, 0.5 | Prix initiaux |
| `--strike` | 1 | Strike du panier |
| `--rate` | 0.05 | Taux annuel continu |
| `--maturity` | 2 | Maturité **en années** |
| `--domain` | 2 | Domaine EF [0,a]² |
| `--mesh` | 40 | Subdivisions par axe, soit (n+1)² nœuds |
| `--steps` | 244 | Pas temporels, atteignant exactement T |
| `--pairs` | 500000 | Paires antithétiques indépendantes : 1 million de trajectoires |
| `--seed` | 42 | Graine aléatoire |
| `--output` | comparison.csv | Export des paramètres, prix, erreurs, IC et durées |

L'écart relatif exporté est une fraction (multiplier par 100 pour un pourcentage).
Il est laissé vide lorsque le prix Monte-Carlo est nul.

## Résultats exécutés

Pour S₁=S₂=0,5, K=1, r=5 %, T=2 ans, domaine [0,2]²,
244 pas et 500 000 paires (seed 42) :

| Subdivisions EF | Prix EF | Prix MC | Écart absolu | Écart relatif |
|---:|---:|---:|---:|---:|
| 20 | 0.11024025 | 0.11150602 | 0.00126577 | 1.135 % |
| 40 | 0.11118801 | 0.11150602 | 0.00031800 | 0.285 % |
| 80 | 0.11141712 | 0.11150602 | 0.00008890 | 0.080 % |
| 120 | 0.11145924 | 0.11150602 | 0.00004678 | 0.042 % |

**Intervalle MC à 95 % : [0.11135907 ; 0.11165297]**, erreur standard 0.00007498.
Les prix EF aux maillages 80 et 120 sont dans cet intervalle. L'écart diminue ici
avec le raffinement spatial ; cela ne constitue pas une preuve de convergence
pour tous les paramètres. Le MC n'est pas un prix exact.

Contrôles supplémentaires : doubler les pas temporels à 488 sur le maillage 80
produit 0.11141711 ; étendre le domaine à [0,3]² avec 120 subdivisions (même
pas spatial 0,025) produit 0.11141712. Dans ce scénario, l'erreur spatiale domine
les effets observés du temps et du domaine. Les CSV complets sont dans `results/`.
Les temps dépendent de la machine ; la séquence normale peut varier d'une
bibliothèque standard C++ à l'autre, même avec la même graine.

Pour reproduire :

```bash
./compare_pricers --mesh 20 --output results/mesh20.csv
./compare_pricers --mesh 40 --output results/mesh40.csv
./compare_pricers --mesh 80 --output results/mesh80.csv
./compare_pricers --mesh 120 --output results/mesh120.csv
./compare_pricers --domain 3 --mesh 120 --output results/domain3.csv
./compare_pricers --mesh 80 --steps 488 --output results/time488.csv
```

## Modèle et Monte-Carlo

Sous la probabilité risque-neutre :

$$dS_i = r S_i\,dt + \sigma_i S_i\,dW_i, \qquad d\langle W_1,W_2\rangle_t=\rho\,dt.$$

La simulation à maturité est exacte pour ce modèle :

$$S_i(T)=S_i(0)\exp\left((r-\sigma_i^2/2)T+\sigma_i\sqrt{T}Z_i\right),$$

avec Z₁=U et Z₂=ρU+√(1−ρ²)V, où U et V sont indépendantes N(0,1).
Chaque paire utilise (Z₁,Z₂) et (−Z₁,−Z₂). Le prix est la moyenne des
payoffs actualisés. L'erreur standard est calculée sur les **moyennes des paires**,
qui sont indépendantes, et non sur les deux trajectoires corrélées de chaque paire.
L'accumulation de Welford évite la soustraction de grands moments proches.
L'IC asymptotique vaut prix ± 1,96 × erreur standard ; il est peu informatif pour
les payoffs très rares ou les petits échantillons. Aucun biais de discrétisation
temporelle n'intervient dans ce Monte-Carlo européen.

## Éléments finis

On résout en temps restant τ=T−t :

$$\partial_\tau u-\frac12\sum_{i,j}C_{ij}s_i s_j\partial_{ij}u
-r\sum_i s_i\partial_i u+ru=0,\qquad u(s,0)=(s_1+s_2-K)^+.$$

Avec Aᵢⱼ=½Cᵢⱼsᵢsⱼ et bᵢ=(Cᵢᵢ+½Cᵢⱼ−r)sᵢ (j≠i),
la forme faible, pour les fonctions test nulles au bord, est

$$(\partial_\tau u,w)+(A\nabla u,\nabla w)+(b\cdot\nabla u,w)+r(u,w)=0.$$

On réutilise les classes `Maillage`, `Point`, `Triangle`, `matrice` et les matrices
élémentaires du projet. Le maillage est un quadrillage régulier coupé en triangles.
Les fonctions de base sont P1 ; la masse locale vaut

$$M_e=\frac{|T_e|}{12}\begin{pmatrix}2&1&1\\1&2&1\\1&1&2\end{pmatrix}.$$

Crank–Nicolson donne (M+ΔτD/2)uⁿ⁺¹=(M−ΔτD/2)uⁿ, avec D=K+B+rM.
Le solveur en bande factorise une seule fois la matrice et réutilise les facteurs.
Il n'utilise pas de pivotage et rejette les pivots quasi nuls : ce n'est pas un
solveur général pour des configurations arbitrairement mal conditionnées.
L'évaluation hors nœuds utilise une interpolation barycentrique P1.

Conditions de bord :

- Sur S₁=0 ou S₂=0, le problème devient un call Black–Scholes à un actif (exact).
- Sur les bords supérieurs, on impose max(S₁+S₂−K exp(−rτ),0), une approximation
  de régime profondément dans la monnaie. Il faut éloigner ces bords du spot et
  du strike et vérifier la stabilité en agrandissant le domaine.

L'IC Monte-Carlo ne couvre **ni l'erreur EF, ni la troncature du domaine**.
Crank–Nicolson avec payoff non lisse peut présenter des oscillations sur des
maillages grossiers ; raffiner et contrôler la solution avant toute utilisation.

## Corrections nécessaires à la comparaison

L'ancienne implémentation comportait une inversion des coefficients diagonaux et
hors diagonale de la masse, un facteur 2 dans la quadrature de convection, un taux
fixé à 5 % dans la convection et une utilisation de jours avec des coefficients
annuels. Ces points sont corrigés, ainsi que l'accès invalide à `Time.end()`, la
troncature des domaines non entiers et l'effacement d'éléments pendant l'itération
lors de la multiplication d'une matrice par zéro. Des conditions de bord explicites
sont maintenant appliquées par le pricer partagé.

## Interface Qt/SFML existante

L'interface reste facultative. Avec Qt5 Widgets, qmake et SFML installés :

```bash
qmake option-europeenne.pro -o Makefile.qt
make -f Makefile.qt
./option-europeenne
```

Elle conserve la saisie de T et dt **en jours**, convertit la maturité en années et
utilise ceil(T/dt) pas uniformes. Elle appelle le même pricer EF que la comparaison
console. Les spots de comparaison sont fixés à S₁=S₂=a/4 ; les prix et l'IC MC
s'affichent dans le terminal. Pour choisir les spots, utiliser `compare_pricers`.
Le graphe représente désormais le **prix à un spot fixe en fonction de la maturité
restante**, et non un mélange de nœuds ou une trajectoire temporelle de marché.
`Maillage.txt` et `Solution.txt` contiennent le maillage et l'historique EF.
La compilation et l'affichage de cette interface n'ont pas été vérifiés dans
l'environnement de développement de cet ajout (Qt/SFML indisponibles).

## Tests et fichiers

`make check` vérifie les intégrales élémentaires, l'orientation des triangles,
les échéances, le cas à maturité, la limite Black–Scholes à un actif, le strike nul,
la reproductibilité, le raffinement EF et les entrées invalides.

| Fichier | Rôle |
|---|---|
| `pricing.hpp` | Paramètres communs et interfaces des pricers |
| `monte_carlo.cpp` | Simulation exacte, antithétiques, IC |
| `fem_pricer.cpp` | Assemblage partagé, bords, résolution en bande, interpolation |
| `compare_pricers.cpp` | CLI, comparaison et CSV |
| `pricing_tests.cpp` | Tests numériques |
| `elementsFinies.cpp` | Matrices élémentaires et covariance |
| `main.cpp`, `utils.cpp` | Interface graphique facultative |

Projet académique d'analyse numérique et de mathématiques financières.
