# Call européen à deux actifs : éléments finis et Monte-Carlo

> Deux approches indépendantes pour une même option : résoudre une EDP sur un
> maillage et estimer une espérance par simulation. Le projet permet de comparer
> les prix, de mesurer l'incertitude Monte-Carlo et d'étudier la convergence numérique.

## Sommaire

- [Comprendre le produit](#comprendre-le-produit)
- [Compilation et comparaison sans interface graphique](#compilation-et-comparaison-sans-interface-graphique)
- [Résultats exécutés](#résultats-exécutés)
- [Modèle et Monte-Carlo](#modèle-et-monte-carlo)
- [Éléments finis](#éléments-finis)
- [Comment interpréter la comparaison](#comment-interpréter-la-comparaison)
- [Corrections nécessaires à la comparaison](#corrections-nécessaires-à-la-comparaison)
- [Interface Qt/SFML existante](#interface-qtsfml-existante)
- [Tests et fichiers](#tests-et-fichiers)
- [Dépannage](#dépannage)

## Comprendre le produit

Il s'agit d'un **call européen sur un panier composé d'une unité de chacun des deux
actifs**. Il ne peut être exercé qu'à maturité. Son paiement dépend de la somme des
prix des actifs, et non du meilleur des deux ou de leur moyenne.

Par exemple, pour K=1 :

| S₁ à maturité | S₂ à maturité | Somme | Payoff |
|---:|---:|---:|---:|
| 0,70 | 0,50 | 1,20 | 0,20 |
| 0,40 | 0,50 | 0,90 | 0 |
| 0,60 | 0,40 | 1,00 | 0 |

Ces paiements futurs ne sont pas le prix aujourd'hui : il faut prendre leur
espérance sous la probabilité risque-neutre et les actualiser.

$$V_0=e^{-rT}\mathbb{E}^{\mathbb{Q}}[(S_1(T)+S_2(T)-K)^+].$$

Hypothèses : taux et volatilités constants, actifs lognormaux corrélés, absence de
dividendes et absence d'exercice anticipé. Les deux méthodes calculent cette même
quantité. La somme de deux lognormales n'étant généralement pas lognormale, on ne
peut pas simplement appliquer Black–Scholes à la somme avec une volatilité moyenne.

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

### Utilisation dans VS Code

Ouvrir le dossier du dépôt dans VS Code, puis **Terminal → Nouveau terminal**.
L'extension C/C++ ne remplace pas le compilateur : `g++ --version` doit fonctionner.
Les commandes précédentes sont à exécuter dans le dossier contenant le Makefile.

Pour récupérer la version fusionnée dans un clone existant, après avoir sauvegardé
ou commité ses éventuelles modifications locales :

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

Sous Linux/WSL, compiler avec `make compare check`, puis lancer `./compare_pricers`.
Sous Windows/PowerShell, utiliser la commande `g++` ci-dessus et lancer
`.\compare_pricers.exe`. Les sources graphiques `main.cpp` et `utils.cpp` ne doivent
pas être ajoutées à cette commande console : elles nécessitent Qt/SFML et un autre
point d'entrée.

### Paramètres de la ligne de commande

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

Les spots, le strike et la maturité doivent être positifs ou nuls ; le domaine doit
contenir les deux spots. Prévoir au moins deux paires MC, un pas temporel et entre
2 et 500 subdivisions EF. Une grande grille peut consommer beaucoup de mémoire :
commencer avec 40 ou 80 subdivisions.

**Attention aux unités :** écrire `--rate 0.05` pour 5 % et `--maturity 2` pour deux
ans. Les volatilités et la corrélation sont fixes dans `mat_cov()` ; aucune option
`--sigma` ou `--rho` n'est implémentée. Pour modifier la covariance dans le code,
conserver une matrice symétrique positive semi-définie, avec variances strictement
positives pour le MC actuel, puis recompiler et revalider.

Exemple avec d'autres spots et une échéance d'un an :

```bash
./compare_pricers --s1 0.6 --s2 0.4 --strike 1 --rate 0.03 --maturity 1 --mesh 80
```

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

### Algorithme Monte-Carlo, étape par étape

1. Initialiser `std::mt19937_64` avec la graine choisie.
2. Tirer deux normales indépendantes et construire les deux chocs corrélés.
3. Calculer directement S₁(T) et S₂(T), puis le payoff actualisé.
4. Refaire le calcul avec les chocs opposés et prendre la moyenne des deux payoffs.
5. Mettre à jour la moyenne et la variance des observations avec Welford.
6. Renvoyer le prix, l'erreur standard et les bornes de l'intervalle de confiance.

Si Yₖ est le payoff actualisé et Yₖ⁻ celui obtenu avec les chocs opposés, chaque
observation indépendante est Xₖ=(Yₖ+Yₖ⁻)/2. Pour N paires :

$$\widehat V_N=\frac1N\sum_{k=1}^N X_k,\qquad
s_X^2=\frac1{N-1}\sum_{k=1}^N(X_k-\widehat V_N)^2,\qquad
\mathrm{SE}=\frac{s_X}{\sqrt N}.$$

Les antithétiques cherchent à réduire la variance en compensant les chocs opposés.
Leur efficacité dépend du payoff et des paramètres. Multiplier N par quatre réduit
asymptotiquement l'erreur standard d'un facteur deux. Le coût est linéaire en N et
le pricer ne stocke pas les trajectoires : sa mémoire de travail reste constante.

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

### Du maillage au prix

Le calcul EF suit une autre logique :

1. Découper [0,a]² en n² carrés, puis chaque carré en deux triangles :
   **(n+1)² nœuds et 2n² triangles**.
2. Représenter u par une combinaison de fonctions P1, linéaires sur chaque triangle.
   Les inconnues sont les valeurs de l'option aux nœuds.
3. Calculer puis assembler les matrices locales de masse, diffusion et convection.
4. Initialiser chaque nœud au payoff : τ=0 correspond à l'échéance.
5. Avancer en temps restant jusqu'à τ=T avec Crank–Nicolson, en imposant les bords
   à chaque pas. À τ=T, on obtient la valeur aujourd'hui pour cette maturité.
6. Lire la valeur au spot demandé, ou interpoler dans le triangle qui le contient.

Avec la numérotation du maillage, les interactions locales donnent une matrice en
bande de demi-largeur n+1. On réutilise sa factorisation LU car les coefficients
et le pas de temps sont constants. Seul le second membre, notamment les valeurs
aux bords, évolue à chaque pas. Contrairement au MC qui cible un spot donné, une
résolution EF produit une surface de prix sur tout le domaine.

## Comment interpréter la comparaison

| Aspect | Monte-Carlo | Éléments finis |
|---|---|---|
| Quantité calculée | Espérance de payoffs actualisés | Solution d'une EDP |
| Erreur principale | Fluctuation statistique | Maillage, temps et domaine tronqué |
| Réglage de précision | Nombre de paires | Subdivisions, pas temporels, domaine |
| Mesure disponible | Erreur standard et IC approximatif | Étude de raffinement |
| Sortie du pricer | Prix au spot choisi | Prix sur le maillage, puis interpolation |
| Aléatoire | Oui, reproductible dans un même environnement | Non |

Le fichier CSV permet de lire les résultats sans recopier le terminal :

| Colonnes | Lecture |
|---|---|
| `s1` à `seed` | Paramètres complets de l'expérience |
| `fem`, `mc` | Prix calculés par les deux méthodes |
| `mc_se` | Erreur standard de la moyenne MC |
| `ci_low`, `ci_high` | Bornes de l'IC MC approximatif à 95 % |
| `fem_minus_mc` | Écart signé : positif si EF > MC |
| `absolute_gap` | Valeur absolue de cet écart |
| `relative_gap` | Écart absolu divisé par le prix MC, hors cas nul |
| `inside_mc_ci` | 1 si le prix EF est dans l'IC MC, sinon 0 |
| `fem_seconds`, `mc_seconds` | Durées des calculs, hors écriture du CSV |

Le fichier est réécrit à chaque exécution : utiliser `--output` pour conserver
plusieurs expériences. Les dossiers parents du chemin de sortie doivent exister.

Pour vérifier un nouveau scénario, augmenter séparément le nombre de paires,
le nombre de subdivisions et le nombre de pas temporels. Puis agrandir le domaine
**à pas spatial comparable**, donc en augmentant aussi le nombre de subdivisions.
Sinon, on mélange l'effet du domaine avec celui d'un maillage devenu plus grossier.

Un prix EF dans l'IC MC indique une compatibilité numérique pour ce scénario ;
ce n'est pas une certification du solveur. Un écart hors de l'IC ne suffit pas non
plus à conclure à un bug : l'EF possède ses propres erreurs de discrétisation.

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

### Organisation C++

`PricingParameters` centralise les données financières et leur validation.
`MonteCarloResult` regroupe le prix et les statistiques ; `FemResult` possède le
maillage et les valeurs nodales et expose `priceAt()` pour l'interpolation.
Les fonctions `priceMonteCarlo()` et `priceFiniteElements()` sont séparées de
l'interface utilisateur : on peut les appeler depuis un autre programme C++.

Les éléments géométriques sont représentés par `Point`, `Triangle` et `Maillage`.
Le template `Vecteur<T>` fournit les opérations vectorielles ; la classe `matrice`
sert notamment aux matrices élémentaires. Le solveur global utilise une classe
interne `BandMatrix` adaptée à la structure du maillage. Les conteneurs standard
possèdent leurs données ; le paramètre optionnel `history` du pricer EF est un
pointeur non propriétaire permettant à l'interface de récupérer l'évolution.

Lancer les tests avec `make check`. Le message attendu est
`All pricing tests passed`. La compilation console et les tests numériques ont
été exécutés ; les contrôles AddressSanitizer et UndefinedBehaviorSanitizer ont
également passé avec la détection des fuites désactivée, car LeakSanitizer ne
fonctionnait pas dans l'environnement utilisé. Aucun contrôle de fuite réussi
n'est donc revendiqué.

## Dépannage

| Problème | Solution |
|---|---|
| `g++` non reconnu | Installer/configurer un compilateur C++ puis rouvrir le terminal. |
| `make` non reconnu | Utiliser la commande directe `g++` ou un environnement avec Make. |
| Erreur Qt ou SFML | Pour la version console, utiliser `make compare`, pas `make` seul. |
| Fichier source introuvable | Se placer dans le dossier racine du dépôt. |
| `Invalid FEM domain/grid/time steps` | Vérifier le domaine, les spots et les tailles de discrétisation. |
| `Cannot write ...` | Choisir un chemin accessible et créer son dossier parent. |
| Prix différents des chiffres du README | Vérifier les paramètres, la graine et la bibliothèque C++ ; comparer aussi l'incertitude MC. |
| Écart EF/MC important | Raffiner séparément le maillage, le temps et le domaine, puis augmenter les simulations. |

Projet académique d'analyse numérique et de mathématiques financières.
