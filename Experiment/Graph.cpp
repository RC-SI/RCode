#include "Graph.h"

void Graph::commonNeighbor(const int i, const int j, unsigned long& cn) {
    utils::commonNeighbor(G[i], G[j], cn, 0, degrees[i], 0, degrees[j]);
}

void Graph::commonNeighborNB(const int i, const int j, unsigned long& cn) {
    utils::commonNeighbor(G[i], G[j], cn, 0, degrees[i], 0, degrees[j]);
}

void Graph::commonNeighborCP(const int i, const int j, unsigned long& cn) {
    vec& vec1 = G[i];
    vec& vec2 = G[j];
    int id1 = 0;
    int ed1 = degrees[i];
    int id2 = 0;
    int ed2 = degrees[j];
    while (id1 != ed1 && id2 != ed2) {
        if (vec1[id1]==vec2[id2]) {
            id1++;
            id2++;
        }
        else if (vec1[id1]<vec2[id2])
            id1++;
        else
            id2++;
        cn++;
    }
}

void Graph::commonNeighborBS(const int i, const int j, unsigned long& cn) {
    int d1 = degrees[i];
    int d2 = degrees[j];
    if (d1<d2)
        utils::commonNeighborBS(G[i], G[j], cn, 0, d1, 0, d2);
    else
        utils::commonNeighborBS(G[j], G[i], cn, 0, d2, 0, d1);
}

double Graph::calCN(const pvec& nodePairs, const bool NB) {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    for (const pair<int, int>& p:nodePairs)
        commonNeighbor(p.first, p.second, result);
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total CNs:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

double Graph::calCNBS(const pvec& nodePairs) {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    for (const pair<int, int>& p:nodePairs)
        commonNeighborBS(p.first, p.second, result);
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total CNs:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

unsigned long Graph::calCNCP(const pvec& nodePairs) {
    unsigned long result = 0;
    for (const pair<int, int>& p:nodePairs)
        commonNeighborCP(p.first, p.second, result);
    return result;
}

void Graph::calCN(const vec& nodes) {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    int s = nodes.size();
    for (int m=0; m!=s-1; m++) {
        int i = nodes[m];
        for (int j=m+1; j!=s; j++) {
            int n = nodes[j];
            commonNeighbor(i, n, result);
        }
    }
    cout << endl << "Total CNs:" << result << endl;
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end_time-start_time;
    cout << diff.count() * 1000 << " ms " << endl;
}

double Graph::calTri() {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    for (int i=0; i!=N; i++) {
        int s = degrees[i];
        vec& adj = G[i];
        for (int j=0; j!=s; j++) {
            int n = adj[j];
            commonNeighborNB(i, n, result);
        }
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total Triangles:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

double Graph::calF(const pvec& nodePairs, const bool NB) {
    tqdm bar;
    long double CNDP = 0.0;
    int marked = 0;
    int psize = nodePairs.size();
    for (const pair<int, int>& p:nodePairs) {
        int i = p.first;
        int n = p.second;
        unsigned long CN = 0;
        if (NB)
            commonNeighborNB(i, n, CN);
        else
            commonNeighbor(i, n, CN);
        CNDP = CNDP + (double)(2*CN+1)/(degrees[i]+degrees[n]+1);
        bar.progress(++marked, psize);
    }
    CNDP = CNDP / psize;
    return CNDP;
}

double Graph::calR(const pvec& nodePairs) {
    tqdm bar;
    long double R = 0.0;
    int marked = 0;
    int psize = nodePairs.size();
    for (const pair<int, int>& p:nodePairs) {
        int i = p.first;
        int n = p.second;
        R += degrees[i]<degrees[n]?(double)degrees[i]/degrees[n]:(double)degrees[n]/degrees[i];
        bar.progress(++marked, psize);
    }
    R /= psize;
    return R;
}

void Graph::calF() {
    long double CND = 0.0;
    long double CNDP = 0.0;
    unsigned long total = 0;
    tqdm bar;
    int marked = 0;
    for (int i=0; i<N-1; i++) {
        int len = N-i-1;
        int* CN = new int[len];
        fill_n(CN, len, 0);
        int d = degrees[i];
        for (const int& adj : G[i]) {
            for (const int& adj2 : G[adj]) {
                if (adj2 > i)
                    CN[adj2-i-1] = CN[adj2-i-1] + 1;
            }
        }
        for (int j=0; j<len; j++) {
            int CP = d + degrees[j+i+1];
            CNDP = CNDP + (double)(2*CN[j]+1)/(CP+1);
            if (CN[j] != 0) {
                CND = CND + (double)(2*CN[j])/CP;
                total++;
            }
        }
        delete[] CN;
        marked++;
        bar.progress(marked, N);
    }
    CND = CND/total;
    CNDP = (2*CNDP)/N/(N-1);
    cout << endl << "NF2: " << CND << endl;
    cout << "NPF2: " << CNDP << endl;
}

void Graph::calTriF() {
    tqdm bar;
    long double CND = 0.0;
    long double CNDP = 0.0;
    unsigned long total = 0;
    unsigned long edges = 0;
    int marked = 0;
    for (int i=0; i!=N; i++) {
        for (const int& n: G[i]) {
            unsigned long CN;
            commonNeighbor(i, n, CN);
            if (CN!=0) {
                CND = CND + (double)(2*CN)/(degrees[i]+degrees[n]);
                total++;
            }
            CNDP = CNDP + (double)(2*CN+1)/(degrees[i]+degrees[n]+1);
            edges++;
        }
        bar.progress(++marked, N);
    }
    cout << endl << "Edges:" << edges << ", Total:" << total << endl;
    CND = CND/total;
    CNDP = CNDP / edges;
    cout << "TriNF2: " << CND << endl;
    cout << "TriDF2: " << CNDP << endl;
}

int Graph::selectPivot(const vec& P, const vec& X) {
    int p = P[0];
    int si = 0;
    int ps = P.size();
    for (const int& v : P) {
        unsigned long cn = 0;
        utils::commonNeighbor(G[v], P, cn, 0, degrees[v], 0, ps);
        if (cn>si) {
            si = cn;
            p = v;
        }
    }
    for (const auto& v : X) {
        unsigned long cn = 0;
        utils::commonNeighbor(G[v], P, cn, 0, degrees[v], 0, ps);
        if (cn>si) {
            si = cn;
            p = v;
        }
    }
    return p;
}

void Graph::BKP(vec& P, vec& X, unsigned long& result) {
    if (P.empty()) {
        if (X.empty())
            result++;
        return;
    }
    int u = selectPivot(P, X);
    int vit1 = 0;
    int vit2 = 0;
    int ud = G[u].size();
    int ps = P.size();
    int xs = X.size();
    while (vit1 != ps) {
        if (vit2 == ud || P[vit1] < G[u][vit2]) {
            int v = P[vit1];
            int gvs = G[v].size();
            vec NP;
            NP.reserve(min(ps, gvs));
            vec NX;
            NX.reserve(min(xs, gvs));
            utils::commonNeighbor(P, G[v], NP, 0, ps, 0, gvs);
            utils::commonNeighbor(X, G[v], NX, 0, xs, 0, gvs);
            BKP(NP, NX, result);
            P.erase(P.begin()+vit1);
            X.insert(X.begin()+utils::binarySearch(v, X, 0, xs), v);
            ps--;
            xs++;
        }
        else if (P[vit1] == G[u][vit2]) {
            vit1++;
            vit2++;
        }
        else
            vit2++;
    }
}

double Graph::MC(const vec& nodes, const int* Vrank) {
    unsigned long result = 0;
    auto start_time = chrono::high_resolution_clock::now();
    for (const int& n:nodes) {
        int i = Vrank[n];
        int gns = degrees[n];
        vec P;
        P.reserve(gns);
        vec X;
        X.reserve(gns);
        for (const int& adj : G[n]) {
            if (Vrank[adj] > i)
                P.push_back(adj);
            else
                X.push_back(adj);
        }
        BKP(P, X, result);
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total MC:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}
