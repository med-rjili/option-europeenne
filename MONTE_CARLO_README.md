# Monte Carlo Option Pricing - Implementation Guide

## Overview

This document describes the Monte Carlo simulation approach added to the European option pricing project. The implementation prices options on two correlated assets using geometric Brownian motion and calculates various Greeks for risk management.

## Mathematical Background

### Correlated Log-Normal Asset Prices

The two underlying assets follow correlated geometric Brownian motion:

```
dS₁ = μ₁S₁dt + σ₁S₁dW₁
dS₂ = μ₂S₂dt + σ₂S₂dW₂
```

where:
- S₁, S₂: Asset prices
- μ₁, μ₂: Drift rates (equal to r under risk-neutral measure)
- σ₁, σ₂: Volatilities
- dW₁, dW₂: Correlated Wiener processes with correlation ρ

The solution at time T is:

```
S₁(T) = S₁(0) × exp((r - σ₁²/2)T + σ₁√T × Z₁)
S₂(T) = S₂(0) × exp((r - σ₂²/2)T + σ₂√T × Z₂)
```

where Z₁ and Z₂ are correlated standard normal random variables.

### Correlation Using Cholesky Decomposition

To generate correlated normals from independent normals:

```
W₁ = Z₁
W₂ = ρZ₁ + √(1-ρ²)Z₂
```

where Z₁ and Z₂ are independent N(0,1) random variables.

### Option Payoff

For a European call on the sum of two assets:

```
Payoff = max(S₁(T) + S₂(T) - K, 0)
```

### Monte Carlo Price Estimate

```
V₀ = e^(-rT) × (1/N) × Σᵢ₌₁ⁿ Payoff(S₁ⁱ(T), S₂ⁱ(T))
```

with standard error:

```
SE = √(Var(Payoffs)/N) × e^(-rT)
```

## Implementation Details

### Class Structure

#### `OptionParams` Struct
Holds all option parameters:
- `S1_0, S2_0`: Initial asset prices
- `K`: Strike price
- `T`: Time to maturity (years)
- `r`: Risk-free rate
- `sigma1, sigma2`: Volatilities
- `rho`: Correlation

#### `Greeks` Struct
Holds calculated Greeks:
- `delta1, delta2`: Price sensitivities to underlying assets
- `gamma1, gamma2`: Second-order price sensitivities
- `vega1, vega2`: Volatility sensitivities
- `theta`: Time decay
- `rho_greek`: Correlation sensitivity
- `rho_rate`: Interest rate sensitivity

#### `MonteCarloPricer` Class
Main pricing engine with methods:
- `priceOption()`: Standard Monte Carlo pricing
- `priceOptionAntithetic()`: Pricing with variance reduction
- `calculateGreeks()`: Finite difference Greeks calculation
- `priceEvolution()`: Price over multiple time steps
- `convergenceAnalysis()`: Study of convergence properties

### Variance Reduction Techniques

#### Antithetic Variates
For each simulation, we generate two paths:
1. Path with random variables (Z₁, Z₂)
2. Antithetic path with (-Z₁, -Z₂)

This reduces variance by exploiting negative correlation between paths.

Expected variance reduction: ~50% for monotonic payoffs.

### Greeks Calculation

Greeks are calculated using finite differences:

#### Delta (Δ)
```
Δ₁ = (V(S₁+ΔS₁) - V(S₁-ΔS₁)) / (2ΔS₁)
Δ₂ = (V(S₂+ΔS₂) - V(S₂-ΔS₂)) / (2ΔS₂)
```

#### Gamma (Γ)
```
Γ₁ = (V(S₁+ΔS₁) - 2V(S₁) + V(S₁-ΔS₁)) / (ΔS₁)²
Γ₂ = (V(S₂+ΔS₂) - 2V(S₂) + V(S₂-ΔS₂)) / (ΔS₂)²
```

#### Vega (ν)
```
ν₁ = (V(σ₁+Δσ) - V(σ₁)) / Δσ
ν₂ = (V(σ₂+Δσ) - V(σ₂)) / Δσ
```

#### Theta (Θ)
```
Θ = (V(T-Δt) - V(T)) / Δt
```

#### Rho (ρ)
```
ρ_rate = (V(r+Δr) - V(r)) / Δr
ρ_corr = (V(ρ+Δρ) - V(ρ)) / Δρ
```

## Visualization

The implementation provides several visualization types:

### 1. Convergence Plot
Shows how the price estimate converges as the number of simulations increases.
- X-axis: Number of simulations
- Y-axis: Estimated option price
- Purpose: Verify convergence and choose appropriate simulation count

### 2. Price Evolution
Shows option price as time to maturity decreases.
- X-axis: Time to maturity
- Y-axis: Option price
- Purpose: Understand time decay (Theta)

### 3. Greeks Sensitivity
Shows how Greeks vary with spot prices.
- X-axis: Spot price of asset
- Y-axis: Greek value
- Purpose: Risk management and hedging

### 4. Monte Carlo vs Finite Elements
Compares the two pricing methods.
- X-axis: Time
- Y-axis: Option price
- Purpose: Validation and method comparison

## Usage

### Compilation

```bash
# Add the Monte Carlo files to your project
qmake
make

# Or compile manually
g++ -std=c++17 -o montecarlo main_montecarlo.cpp monteCarlo.cpp \
    monteCarloVisualization.cpp -lsfml-graphics -lsfml-window -lsfml-system
```

### Running

```bash
./montecarlo
```

The program will prompt for:
- Initial asset prices
- Strike price
- Time to maturity
- Risk-free rate
- Volatilities
- Correlation
- Number of simulations

### Example Parameters

For comparison with the FEM implementation:
```
S1_0 = 1.0
S2_0 = 1.0
K = 1.0
T = 2.0 years
r = 0.05
sigma1 = 0.2 (from √0.04)
sigma2 = 0.2 (from √0.04)
rho = -0.6 (from cov = -0.024, σ₁σ₂ = 0.04)
simulations = 100000
```

## Performance Considerations

### Recommended Simulation Counts

| Purpose | Simulations | Time (approx.) |
|---------|-------------|----------------|
| Quick estimate | 1,000 - 10,000 | < 1s |
| Production pricing | 50,000 - 100,000 | 1-5s |
| Greeks calculation | 10,000 - 50,000 per Greek | 10-30s |
| High precision | 500,000 - 1,000,000 | 30-60s |

### Accuracy vs Speed Trade-off

Standard error decreases as 1/√N:
- 10,000 simulations → SE ≈ 1%
- 100,000 simulations → SE ≈ 0.3%
- 1,000,000 simulations → SE ≈ 0.1%

## Improvements and Extensions

### Implemented
✓ Antithetic variates for variance reduction
✓ Full Greeks calculation
✓ Convergence analysis
✓ Interactive visualization
✓ Comparison with FEM

### Suggested Improvements

1. **Advanced Variance Reduction**
   - Control variates using known option prices
   - Importance sampling for deep out-of-the-money options
   - Stratified sampling

2. **Quasi-Random Sequences**
   - Sobol sequences
   - Halton sequences
   - Better convergence rate: O(1/N) vs O(1/√N)

3. **Parallel Computing**
   - Multi-threading for independent simulations
   - GPU acceleration (CUDA/OpenCL)
   - Can achieve 10-100x speedup

4. **Advanced Greeks**
   - Pathwise derivatives (more accurate than finite differences)
   - Likelihood ratio method
   - Cross-Greeks (e.g., ∂²V/∂S₁∂S₂)

5. **American Options**
   - Longstaff-Schwartz regression
   - Binomial tree overlay

6. **Exotic Options**
   - Barrier options
   - Asian options (path-dependent)
   - Lookback options

7. **Model Extensions**
   - Stochastic volatility (Heston model)
   - Jump-diffusion processes
   - Local volatility surfaces

8. **Risk Management**
   - Value at Risk (VaR) calculation
   - Conditional Value at Risk (CVaR)
   - Stress testing scenarios

9. **Calibration**
   - Calibrate model parameters to market data
   - Implied volatility surface
   - Correlation matrix estimation

10. **Production Features**
    - Configuration files
    - Database integration
    - RESTful API
    - Real-time market data feeds
    - Automated hedging recommendations

## Validation

### Cross-Validation with FEM
The Monte Carlo implementation can be validated against the finite elements method:
1. Use same parameters
2. Compare prices (should agree within error bounds)
3. Check Greeks (directional agreement)

### Convergence Tests
1. Run with increasing simulation counts
2. Verify error decreases as 1/√N
3. Check confidence intervals

### Greeks Validation
1. Verify put-call parity (if applicable)
2. Check delta bounds: 0 ≤ Δ ≤ 1 for calls
3. Verify gamma is non-negative
4. Check theta is typically negative for long positions

## References

### Mathematical Finance
- Hull, J. C. (2018). *Options, Futures, and Other Derivatives*
- Glasserman, P. (2003). *Monte Carlo Methods in Financial Engineering*
- Shreve, S. E. (2004). *Stochastic Calculus for Finance II*

### Numerical Methods
- Boyle, P. (1977). "Options: A Monte Carlo Approach"
- Broadie, M., & Glasserman, P. (1996). "Estimating Security Price Derivatives Using Simulation"
- Longstaff, F. A., & Schwartz, E. S. (2001). "Valuing American Options by Simulation"

### Variance Reduction
- Hammersley, J. M., & Morton, K. W. (1956). "A New Monte Carlo Technique: Antithetic Variates"
- Lavenberg, S. S., & Welch, P. D. (1981). "A Perspective on the Use of Control Variables"

## Author Notes

This implementation demonstrates:
- Professional-grade Monte Carlo engine
- Proper correlation handling for multi-asset options
- Comprehensive Greeks calculation
- Variance reduction techniques
- Interactive visualization
- Well-documented code

The implementation is suitable for:
- Academic projects in quantitative finance
- Learning Monte Carlo methods
- Prototyping trading strategies
- Risk management analysis
- Algorithm comparison studies

For production use, consider:
- Performance optimization (parallelization)
- More sophisticated variance reduction
- Calibration to market data
- Error handling and logging
- Integration with trading systems
