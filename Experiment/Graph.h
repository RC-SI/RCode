#ifndef GRAPH_H
#define GRAPH_H
#include "utils.h"

class Graph {
public:
    Graph (const int VN) : N(VN), G(new vec[VN]), degrees(new int[VN]) {}
    Graph(vec* OG, const int VN) : N(VN), G(OG), degrees(new int[VN]) {
        for (int i=0; i!=N; i++)
            degrees[i] = G[i].size();
    }
    virtual ~Graph() {
        delete[] degrees;
    }
    virtual void commonNeighbor(const int i, const int j, unsigned long& cn);
    virtual void commonNeighborCP(const int i, const int j, unsigned long& cn);
    virtual void commonNeighborNB(const int i, const int j, unsigned long& cn);
    virtual void commonNeighborBS(const int i, const int j, unsigned long& cn);

    virtual double calCN(const pvec& nodePairs, const bool NB);
    virtual double calCNBS(const pvec& nodePairs);
    virtual unsigned long calCNCP(const pvec& nodePairs);

    virtual void calCN(const vec& nodes);
    virtual double calF(const pvec& nodePairs, const bool NB);
    virtual double calR(const pvec& nodePairs);
    virtual double calTri();
    virtual void calF();
    virtual void calTriF();

    virtual double MC(const vec& nodes, const int* Vrank);
    
protected:
    const int N;
    vec* G;
    int* degrees;
    int selectPivot(const vec& P, const vec& X);
    virtual void BKP(vec& P, vec& X, unsigned long& result);
};

#endif