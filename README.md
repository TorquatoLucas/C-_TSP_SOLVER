# Solução para o Problema do Caixeiro Viajante (TSP)

## Descrição
Este programa tem como entrada um arquivo com padrão TSPLIB e resolve o Problema do Caixeiro Viajante (TSP) utilizando uma abordagem baseada em:
- **Árvore Geradora Mínima (AGM)** via algoritmo de Kruskal
- **Busca em Profundidade (DFS)** para gerar um percurso inicial
- **Otimização 2-opt** para refinamento do percurso

Objetivos:  
✅ Minimizar a distância máxima entre pontos consecutivos  
✅ Reduzir o custo total do percurso

## Pré-requisitos
- Compilador C++17 (ex: `g++`)

## Compilação
```bash
g++ -std=c++17 -O3 -o sol_tsp tsp_main.cpp
