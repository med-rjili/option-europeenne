# Valorisation d'Options Européennes par la Méthode des Éléments Finis

## 📋 Table des matières
- [Vue d'ensemble](#vue-densemble)
- [Formulation mathématique](#formulation-mathématique)
- [Méthode des éléments finis](#méthode-des-éléments-finis)
- [Implémentation](#implémentation)
- [Dépendances](#dépendances)
- [Installation et exécution](#installation-et-exécution)
- [Structure du projet](#structure-du-projet)
- [Résultats](#résultats)

## Vue d'ensemble

Ce projet implémente la valorisation d'**options européennes à deux actifs sous-jacents** en utilisant la **méthode des éléments finis** pour résoudre l'équation aux dérivées partielles (EDP) de Black-Scholes. Le projet utilise C++ avec une approche orientée objet et propose une interface graphique interactive développée avec Qt et SFML.

### Qu'est-ce qu'une option européenne?

Une option européenne est un produit dérivé financier qui donne à son détenteur le droit (mais non l'obligation) d'acheter (call) ou de vendre (put) un actif sous-jacent à un prix d'exercice **K** fixé à l'avance, à la date d'échéance **T**.

Dans ce projet, nous considérons une option sur **deux actifs** avec un payoff de type:

$$V(x_1, x_2, T) = \max(x_1 + x_2 - K, 0)$$

où $x_1$ et $x_2$ représentent les prix des deux actifs à maturité.

## Formulation mathématique

### 1. Équation de Black-Scholes pour deux actifs

Le prix de l'option $V(x_1, x_2, t)$ satisfait l'EDP de Black-Scholes suivante:

$$\frac{\partial V}{\partial t} + \frac{1}{2}\sum_{i=1}^{2}\sum_{j=1}^{2} \sigma_{ij} x_i x_j \frac{\partial^2 V}{\partial x_i \partial x_j} + \sum_{i=1}^{2}\left(\sigma_{ii} + \frac{1}{2}\sigma_{ji} - r\right)x_i\frac{\partial V}{\partial x_i} - rV = 0$$

avec la condition terminale:
$$V(x_1, x_2, T) = \max(x_1 + x_2 - K, 0)$$

où:
- $r$ : taux d'intérêt sans risque
- $\sigma_{ij}$ : matrice de covariance des actifs (volatilités et corrélations)
- $K$ : prix d'exercice (strike)
- $T$ : date d'échéance

### 2. Matrice de covariance

Dans notre implémentation, la matrice de covariance utilisée est:

$$C = \begin{pmatrix} 0.04 & -0.024 \\ -0.024 & 0.04 \end{pmatrix}$$

Cette matrice capture:
- La variance de chaque actif ($\sigma_{11} = \sigma_{22} = 0.04$)
- La corrélation négative entre les deux actifs ($\sigma_{12} = \sigma_{21} = -0.024$)

### 3. Forme variationnelle

Pour appliquer la méthode des éléments finis, on réécrit l'EDP sous forme faible. Soit $\Omega = [0, a] \times [0, a]$ le domaine spatial. En multipliant l'EDP par une fonction test $w$ et en intégrant:

$$\int_\Omega \frac{\partial V}{\partial t} w \, dx + \int_\Omega \left[\frac{1}{2}\nabla V \cdot A(x)\nabla w + V(x) \cdot \nabla w\right] dx - r\int_\Omega Vw \, dx = 0$$

où:

$$A(x) = \frac{1}{2}\begin{pmatrix} C_{11}x_1^2 & C_{12}x_1x_2 \\ C_{21}x_1x_2 & C_{22}x_2^2 \end{pmatrix}$$

$$V(x) = \begin{pmatrix} (C_{11} + \frac{1}{2}C_{21} - r)x_1 \\ (C_{22} + \frac{1}{2}C_{12} - r)x_2 \end{pmatrix}$$

## Méthode des éléments finis

### 1. Discrétisation spatiale

Le domaine $\Omega = [0, a] \times [0, a]$ est décomposé en un **maillage triangulaire** avec:
- $m \times n$ nœuds régulièrement espacés
- Triangulation de Delaunay pour assurer la qualité du maillage

Sur chaque triangle $T_k$, on utilise des **fonctions de base linéaires** (éléments finis P1). Pour un triangle avec sommets $S_1, S_2, S_3$, les trois fonctions de base $w_1, w_2, w_3$ sont définies par:

- $w_i(S_j) = \delta_{ij}$ (1 au sommet $i$, 0 aux autres sommets)
- $w_i$ varie linéairement sur le triangle
- $w_1 + w_2 + w_3 = 1$ (partition de l'unité)

Ces fonctions sont appelées **coordonnées barycentriques** du triangle.

### 2. Matrices élémentaires

Pour chaque triangle $T$ avec sommets $S_1, S_2, S_3$, on calcule trois matrices élémentaires:

#### Matrice de masse $M_{el}$:
$$M_{el}^{ij} = \int_T w_i w_j \, dx = \frac{|T|}{12}\begin{pmatrix} 2 & 1 & 1 \\ 1 & 2 & 1 \\ 1 & 1 & 2 \end{pmatrix}$$

où $|T|$ est l'aire du triangle.

#### Matrice de rigidité $K_{el}$:
$$K_{el}^{ij} = \int_T \nabla w_i \cdot A(x) \nabla w_j \, dx$$

Calculée par quadrature de Gauss d'ordre 2 (3 points):

$$K_{el}^{ij} \approx \frac{|T|}{|D|^2}\sum_{k=1}^{3} c_k \left[\nabla w_i \cdot A(\hat{x}_k) \nabla w_j\right]$$

#### Matrice de convection $B_{el}$:
$$B_{el}^{ij} = \int_T V(x) \cdot \nabla w_j \cdot w_i \, dx$$

Calculée par quadrature de Gauss d'ordre 3 (4 points):

$$B_{el}^{ij} \approx \frac{|T|}{|D|}\sum_{k=1}^{4} c_k \left[V(\hat{x}_k) \cdot \nabla w_j \cdot w_i(\hat{x}_k)\right]$$

### 3. Assemblage global

Les matrices globales $M$, $K$, et $B$ sont assemblées en sommant les contributions de chaque triangle:

$$M = \sum_{k=1}^{N_T} M_{el}^{(k)}, \quad K = \sum_{k=1}^{N_T} K_{el}^{(k)}, \quad B = \sum_{k=1}^{N_T} B_{el}^{(k)}$$

### 4. Schéma numérique temporel

Pour la discrétisation temporelle, on utilise le **schéma de Crank-Nicolson** (θ-schéma avec θ = 1/2), qui est:
- Inconditionnellement stable
- Précis à l'ordre 2 en temps

Le schéma s'écrit:

$$\left(M + \frac{\Delta t}{2}D\right)V^{n+1} = \left(M - \frac{\Delta t}{2}D\right)V^n$$

où:
- $D = rM + K + B$ : opérateur spatial complet
- $V^n$ : vecteur des valeurs nodales au temps $t_n = n\Delta t$
- $\Delta t$ : pas de temps

À chaque pas de temps, on résout le système linéaire par **factorisation LU**.

### 5. Quadratures de Gauss

#### Quadrature d'ordre 2 (3 points):
Points de référence sur le triangle: $\hat{P}_1 = (1/6, 1/6)$, $\hat{P}_2 = (2/3, 1/6)$, $\hat{P}_3 = (1/6, 2/3)$

Poids: $c_1 = c_2 = c_3 = 1/6$

*Note: Ces poids incluent l'aire du triangle de référence (1/2). En notation standard, on utilise parfois $\omega_i = 1/3$ avec un facteur d'aire séparé.*

#### Quadrature d'ordre 3 (4 points):
Points de référence: $\hat{P}_1 = (1/3, 1/3)$, $\hat{P}_2 = (0.6, 0.2)$, $\hat{P}_3 = (0.2, 0.6)$, $\hat{P}_4 = (0.2, 0.2)$

Poids: $c_1 = -27/48$, $c_2 = c_3 = c_4 = 25/48$

## Implémentation

### Architecture du code

Le projet est structuré en modules C++ avec une conception orientée objet:

```
├── matrice.cpp/hpp        # Classe matrice creuse avec opérations algébriques
├── vecteur.hpp            # Template de vecteur avec opérations vectorielles
├── point.cpp/hpp          # Classe Point pour la géométrie 2D
├── triangle.cpp/hpp       # Classe Triangle pour le maillage
├── maillage.cpp/hpp       # Génération du maillage triangulaire
├── elementsFinies.cpp/hpp # Calcul des matrices élémentaires et assemblage
├── utils.cpp/hpp          # Utilitaires (interface Qt, visualisation SFML)
└── main.cpp               # Programme principal
```

### Classes principales

#### 1. **Classe `matrice`**
Gère les matrices creuses avec stockage par `std::map` pour optimiser la mémoire:
- Opérations: addition, multiplication, factorisation LU
- Résolution de systèmes linéaires
- Extraction/modification de lignes

#### 2. **Classe `Maillage`**
Génère et gère le maillage triangulaire:
- Construction d'un maillage rectangulaire régulier
- Stockage des points et triangles
- Sauvegarde du maillage dans un fichier

#### 3. **Fonctions éléments finis**
- `matM_el()`: Calcule la matrice de masse élémentaire
- `matK_el()`: Calcule la matrice de rigidité élémentaire
- `matB_el()`: Calcule la matrice de convection élémentaire
- `quad_2()`, `quad_3()`: Quadratures de Gauss

### Algorithme principal

```cpp
1. Initialisation: Créer le maillage (m×n points)
2. Assemblage:
   Pour chaque triangle T:
     - Calculer M_el, K_el, B_el
     - Ajouter aux matrices globales M, K, B
3. Condition initiale: V^0 = max(x_1 + x_2 - K, 0)
4. Boucle temporelle:
   Pour t = dt, 2dt, ..., T:
     - Résoudre (M + dt/2·D)V^{n+1} = (M - dt/2·D)V^n
     - Visualiser la progression
5. Affichage: Tracer l'évolution temporelle de V
```

## Dépendances

### Qt 5
Qt est une bibliothèque multiplateforme pour créer des interfaces graphiques.

**Installation:**
```bash
sudo apt install qt5-default
```

### SFML (Simple and Fast Multimedia Library)
Bibliothèque pour la visualisation graphique et le tracé de courbes.

**Installation:**
```bash
sudo apt-get install libsfml-dev
```

### Compilateur C++
Nécessite un compilateur supportant C++11 ou supérieur (g++, clang++).

## Installation et exécution

### Compilation

```bash
# Générer le Makefile avec qmake
qmake

# Compiler le projet
make
```

### Exécution

```bash
# Lancer l'application
./option-europeenne
```

### Interface utilisateur

Au lancement, une **interface Qt** permet de configurer:
- **a**: Taille du domaine spatial [0, a] × [0, a] (défaut: 2.0)
- **r**: Taux d'intérêt sans risque (défaut: 0.05)
- **K_**: Prix d'exercice (strike) (défaut: 1.0)
- **T**: Date d'échéance en jours (défaut: 730 jours, soit 2 ans)
- **dt**: Pas de temps en jours (défaut: 3 jours)

Après configuration, cliquez sur **"Calculate"** pour lancer la simulation.

### Nettoyage

```bash
# Supprimer les fichiers compilés
make clean
```

## Structure du projet

```
option-europeenne/
├── *.cpp                  # Fichiers sources C++
├── *.hpp                  # Fichiers d'en-tête
├── arial.ttf              # Police pour l'affichage SFML
├── bg.jpg                 # Image de fond pour l'interface Qt
├── option_europeenne.pdf  # Énoncé du projet
├── README.md              # Ce fichier
├── Makefile               # Généré par qmake
├── option-europeenne.pro  # Fichier de configuration Qt
├── bin/                   # Répertoire de l'exécutable compilé
└── obj/                   # Fichiers objets intermédiaires
```

## Résultats

Le programme génère:

1. **Maillage.txt**: Fichier contenant la structure du maillage (points et triangles)

2. **Solution.txt**: Matrice solution V(x, t) pour tous les points du maillage et tous les pas de temps

3. **Visualisation graphique**: 
   - Barre de progression pendant le calcul
   - Graphique animé montrant l'évolution temporelle du logarithme de la valeur de l'option
   - Axes gradués avec échelle automatique

### Interprétation des résultats

- **Valeur initiale** (t = 0): V(x₁, x₂, 0) représente le prix de l'option aujourd'hui
- **Évolution temporelle**: Le graphique montre comment la valeur de l'option évolue vers le payoff à maturité
- **Convergence**: Le schéma de Crank-Nicolson assure une convergence stable et précise

## Références mathématiques

### Équations clés

**Black-Scholes 2D:**
$$\frac{\partial V}{\partial t} + \mathcal{L}V = 0$$

où $\mathcal{L}$ est l'opérateur de Black-Scholes:
$$\mathcal{L}V = \frac{1}{2}\text{Tr}(C \cdot \text{Hess}(V) \cdot \text{diag}(x)) + (C\mathbf{1} + \frac{1}{2}C^T\mathbf{1} - r\mathbf{1}) \cdot \nabla V - rV$$

**Formulation faible:**
$$\langle \frac{\partial V}{\partial t}, w \rangle + a(V, w) = 0, \quad \forall w \in H_0^1(\Omega)$$

avec la forme bilinéaire:
$$a(V, w) = \int_\Omega \left[\frac{1}{2}\nabla V \cdot A\nabla w + V \cdot \nabla w - rVw\right] dx$$

---

## 📝 Licence

Ce projet est réalisé dans un cadre académique.

## 👥 Auteurs

Développé dans le cadre d'un projet d'analyse numérique et de mathématiques financières.
