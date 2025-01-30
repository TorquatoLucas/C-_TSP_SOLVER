#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <ctime>
#include <climits>

using namespace std;

// Nó que armazena o id dele e as coordenadas x e y
struct No {
    int id;
    double x, y;
};

// Lê entrada com padrão TSPLIB
vector<No> ler_nos() {
    string linha;
    vector<No> nos;
    int dimensao = 0;
    bool lendo_coordenadas = false;

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);


    while (getline(cin, linha)) {
        // Recebe o número de nós do problema
        if (linha.find("DIMENSION") != string::npos) {
            istringstream iss(linha);
            string tmp;
            iss >> tmp >> tmp >> dimensao;
        } 
        // Identifica onde começam as coordenadas dos nós
        else if (linha.find("NODE_COORD_SECTION") != string::npos) {
            lendo_coordenadas = true;
            continue;
        } 
        // Quando encontrar "EOF" encerra a leitura
        else if (linha.find("EOF") != string::npos) {
            break;
        }

        // Armazena os nós
        if (lendo_coordenadas) {
            No no;
            istringstream iss(linha);
            iss >> no.id >> no.x >> no.y;
            nos.push_back(no);
        }
    }

    // Verifica se a quantidade de nós igual ao "DIMENSION"
    if (nos.size() != dimensao) {
        cerr << "Erro na leitura: Esperados " << dimensao 
             << ", lidos " << nos.size() << endl;
        exit(1);
    }

    return nos;
}

// Calcula a distância entre dois nós pela fórmula euclidiana
int distancia(const No& a, const No& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return (int)round(sqrt(dx*dx + dy*dy));
}

// Classe UnionFind (DSU) para manter as componentes conectadas do grafo
// Estrutura de dados que evita criação de ciclos e verifica de 2 elementos pertencem ao msm conjunto

class UnionFind {
    vector<int> pai; // Armazena o pai de cada nó
    vector<int> nivel; // Armazena a profundidade das árvores
public:
    // Construtor que inicializa os pais e os niveis
    UnionFind(int n) {
        pai.resize(n);
        nivel.resize(n, 0);
        iota(pai.begin(), pai.end(), 0);
    }

    // Função para encontrar o líder do conjunto
    int find(int u) {
        if (pai[u] != u)
            pai[u] = find(pai[u]);
        return pai[u];
    }

    // Função para unir dois conjuntos
    void unite(int u, int v) {
        u = find(u);
        v = find(v);
        if (u != v) {
            if (nivel[u] < nivel[v])
                swap(u, v);
            pai[v] = u;
            if (nivel[u] == nivel[v])
                nivel[u]++;
        }
    }
};

// Função para aplicar a otimização 2-opt, melhorando o caminho (percurso)
vector<int> otimizar_two_opt(const vector<int>& caminho, const vector<No>& nos, int max_iteracoes = 100) {
    vector<int> novo_caminho = caminho;
    bool melhoria = true;
    int n = novo_caminho.size() - 1; // Ignora o último nó que é igual ao primeiro
    int iteracao = 0;
    
    // Executa enquanto houver melhorias ou o número máximo de iterações (50) não for alcançado
    while (melhoria && iteracao < max_iteracoes) {
        melhoria = false;
        iteracao++;
        
        int melhor_i = -1, melhor_j = -1;
        int melhor_ganho = 0;
        
        // Busca pela melhor troca na iteração
        for (int i = 1; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int a = novo_caminho[i-1];
                int b = novo_caminho[i];
                int c = novo_caminho[j];
                int d = novo_caminho[j+1];
                
                // Calcula o ganho de trocar a parte do caminho entre i e j
                int atual = distancia(nos[a], nos[b]) + distancia(nos[c], nos[d]);
                int potencial = distancia(nos[a], nos[c]) + distancia(nos[b], nos[d]);
                int ganho = atual - potencial;
                
                // Se o ganho for maior que o melhor ganho encontrado faz a troca
                if (ganho > melhor_ganho) {
                    melhor_ganho = ganho;
                    melhor_i = i;
                    melhor_j = j;
                }
            }
        }
        
        // Se encontrou um ganho positivo aplica a troca
        if (melhor_ganho > 0) {
            reverse(novo_caminho.begin() + melhor_i, novo_caminho.begin() + melhor_j + 1);
            melhoria = true;
        }
    }
    
    return novo_caminho;
}

// Função para aplicar o algoritmo de Kruskal e gerar a Árvore Geradora Mínima (AGM)
vector<pair<int, int>> kruskal_agm(const vector<No>& nos, unsigned semente) {
    int n = nos.size();
    vector<tuple<int, int, int>> arestas;

    // Gera todas as arestas possíveis entre os nós
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int d = distancia(nos[i], nos[j]);
            arestas.emplace_back(d, i, j);
        }
    }

    // Ordena as arestas por distância
    sort(arestas.begin(), arestas.end());
    
    // Embaralha as arestas com a mesma distância
    mt19937 rng(semente);
    auto it = arestas.begin();
    while(it != arestas.end()) {
        auto upper = find_if(it, arestas.end(), [&](const auto& e) {
            return get<0>(e) != get<0>(*it);
        });
        
        shuffle(it, upper, rng);
        it = upper;
    }

    UnionFind uf(n);
    vector<pair<int, int>> arestas_agm;

    // Construção da AGM utilizando o algoritmo de Kruskal
    for (auto& aresta : arestas) {
        int d = get<0>(aresta);
        int u = get<1>(aresta);
        int v = get<2>(aresta);
        if (uf.find(u) != uf.find(v)) {
            uf.unite(u, v);
            arestas_agm.emplace_back(u, v);
            if (arestas_agm.size() == n - 1) break;
        }
    }

    return arestas_agm;
}

// Função para construir a lista de adjacência a partir das arestas da AGM
vector<vector<int>> construir_lista_adjacente(const vector<pair<int, int>>& arestas_agm, int n) {
    vector<vector<int>> adj(n);
    // Cria a lista de adjacência
    for (const auto& aresta : arestas_agm) {
        int u = aresta.first;
        int v = aresta.second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return adj;
}

// Função para realizar uma busca em profundidade (DFS) e gerar um caminho (percurso)
void dfs(int u, const vector<vector<int>>& adj, vector<bool>& visitado, vector<int>& caminho) {
    visitado[u] = true;
    caminho.push_back(u);
    for (int v : adj[u]) {
        if (!visitado[v]) {
            dfs(v, adj, visitado, caminho);
        }
    }
}

// Função para gerar o caminho (percurso) a partir de uma DFS
vector<int> dfs_caminho(const vector<vector<int>>& adj, int inicio) {
    int n = adj.size();
    vector<bool> visitado(n, false);
    vector<int> caminho;
    dfs(inicio, adj, visitado, caminho);
    return caminho;
}



// Função main





int main() {
    // Seed
    unsigned semente = static_cast<unsigned>(time(nullptr));

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // Lê os nós usando cin
    vector<No> nos = ler_nos();
    int n = nos.size();

    // Gera a Árvore Geradora Mínima (AGM) utilizando Kruskal
    vector<pair<int, int>> arestas_agm = kruskal_agm(nos, semente);
    vector<vector<int>> adj = construir_lista_adjacente(arestas_agm, n);

    // Gera o caminho inicial com uma busca em profundidade (DFS)
    vector<int> caminho = dfs_caminho(adj, 0);
    
    // Aplica a otimização 2-opt para melhorar o caminho (percurso)
    caminho = otimizar_two_opt(caminho, nos, 50);  // Limita a 50 iterações
    caminho.push_back(caminho[0]);  // Fecha o ciclo

    // Calcula a distância máxima e o custo total do caminho
    int distancia_max = 0;
    int custo_total = 0;

    // "size_t" garante que possa armazenar o tamanho máximo de um objeto
    for (size_t i = 0; i < caminho.size() - 1; ++i) {
        int u = caminho[i];
        int v = caminho[i + 1];
        int d = distancia(nos[u], nos[v]);
        custo_total += d;
        distancia_max = max(distancia_max, d);
    }

    // Exibe os resultados
    cout << "Distancia maxima: " << distancia_max << endl;
    cout << "Custo total: " << custo_total << endl;
    cout << "Semente usada: " << semente << endl;

    // Grava o resultado em um arquivo
    ofstream output("formato.txt");
    if(output.is_open()) {
        // Remove o último elemento se for igual ao primeiro (ciclo)
        if(!caminho.empty() && caminho.back() == caminho.front()) {
            caminho.pop_back();
        }
        
        // Escreve os IDs no arquivo no formato v0 v1 v2 ... vn-1
        for(size_t i = 0; i < caminho.size(); ++i) {
            output << caminho[i];
            if(i != caminho.size() - 1) {
                output << " ";
            }
        }
        output.close();
    } else {
        cerr << "Erro ao criar o arquivo de saída!" << endl;
    }

    return 0;
}
