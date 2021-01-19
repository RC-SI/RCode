#include "indexMCGraph.h"

inline int binarySearch(const int _key, const adjList& _list, int _left, int _right) {
    int _mid;
    while (_left < _right) {
        _mid = (_right + _left) / 2;
        if (_list[_mid].first < _key)
            _left = _mid+1;
        else
            _right = _mid;
    }
    return _left;
}

inline void commonNeighbor(const vec& vec1, const vec& vec2, unsigned long& cn) {
    int id1 = 0;
    int ed1 = vec1.size();
    int id2 = 0;
    int ed2 = vec2.size();
    while (id1 != ed1 && id2 != ed2) {
        int v1 = vec1[id1];
        int v2 = vec2[id2];
        if (v1==v2) {
            if (v1!=-1)
                cn++;
            id1++;
            id2++;
        }
        else if (v1<v2)
            id1++;
        else
            id2++;
    }
}

inline void commonNeighbor(const vec& vec1, const vec& vec2, vec& cn) {
    int id1 = 0;
    int ed1 = vec1.size();
    int id2 = 0;
    int ed2 = vec2.size();
    while (id1 != ed1 && id2 != ed2) {
        int v1 = vec1[id1];
        int v2 = vec2[id2];
        if (v1==v2) {
            if (v1!=-1)
                cn.push_back(v1);
            id1++;
            id2++;
        }
        else if (v1<v2)
            id1++;
        else
            id2++;
    }
}

void indexVecGraph::processHGIndex(const vec* OG) {
    for (int i=0; i!=N; i++) {
        int newid = ids[i];
        adjList& neighbors = G[newid];
        vec& adjSort = GO[newid];
        adjSort.reserve(OG[i].size());
        for (const int& adj : OG[i])
            adjSort.push_back(ids[adj]);
        sort(adjSort.begin(), adjSort.end());
        degreesh[newid] = 0;
        int prev = INT_MIN;
        for (const int adj : adjSort) {
            int adjh = hashes[adj];
            if (adjh!=prev) {
                if (prev!=INT_MIN)
                    degreesh[newid] += neighbors.back().second->size();
                prev = adjh;
                vec* nv = new vec();
                neighbors.push_back({adjh, nv});
            }
            neighbors.back().second->push_back(adj);
        }
        if (neighbors.back().first!=INT_MAX) {
            hashesMin[newid] = neighbors.front().first;
            hashesMax[newid] = neighbors.back().first;
            degreesh[newid] += neighbors.back().second->size();
            degrees[newid] = neighbors.size();
        }
        else {
            if (neighbors.size()!=1) {
                hashesMin[newid] = neighbors.front().first;
                hashesMax[newid] = (*(neighbors.end()-2)).first;
            }
            else {
                hashesMin[newid] = INT_MAX;
                hashesMax[newid] = INT_MIN;
            }
            degrees[newid] = neighbors.size()-1;
        }
    }
}

void commonNeighborNB(const adjList& n1, const int d1, const adjList& n2, const int d2, unsigned long& result) {
    if (d1==1) {
        int h = n1[0].first;
        if (d2==1) {
            if(h==n2[0].first)
                utils::commonNeighbor(*(n1[0].second), *(n2[0].second), result);
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb!=d2 && n2[lb].first==h)
                utils::commonNeighbor(*(n1[0].second), *(n2[lb].second), result);
        }
    }
    else if (d2==1) {
        int h = n2[0].first;
        int lb = binarySearch(h, n1, 0, d1);
        if (lb!=d1 && n1[lb].first==h)
            utils::commonNeighbor(*(n2[0].second), *(n1[lb].second), result);
    }
    else {
        int b1 = 0;
        int f1 = d1;
        int b2 = 0;
        int f2 = d2;
        int m1 = n1[0].first;
        int m2 = n2[0].first;
        int M1 = n1[d1-1].first;
        int M2 = n2[d2-1].first;
        if (max(m1,m2)<=min(M1, M2)) {
            if (m1 < m2)
                b1 = binarySearch(m2, n1, b1, f1);
            else if (m2 < m1)
                b2 = binarySearch(m1, n2, b2, f2);
            if (M1 > M2)
                f1 = binarySearch(M2+1, n1, b1, f1);
            else if (M1 < M2)
                f2 = binarySearch(M1+1, n2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (n1[b1].first==n2[b2].first)
                    utils::commonNeighbor(*(n1[b1++].second), *(n2[b2++].second), result);
                else if (n1[b1].first<n2[b2].first)
                    b1++;
                else
                    b2++;
            }
        }
    }
}

void commonNeighborNB(const adjList& n1, const int d1, const adjList& n2, const int d2, adjList& resultn) {
    if (d1==1) {
        int h = n1[0].first;
        if (d2==1) {
            if(h==n2[0].first) {
                vec result;
                result.reserve(min(n1[0].second->size(), n2[0].second->size()));
                utils::commonNeighbor(*(n1[0].second), *(n2[0].second), result);
                if (!result.empty()) {
                    vec* nv = new vec(move(result));
                    resultn.push_back({h, nv});
                }
            }
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb!=d2 && n2[lb].first==h) {
                vec result;
                result.reserve(min(n1[0].second->size(), n2[lb].second->size()));
                utils::commonNeighbor(*(n1[0].second), *(n2[lb].second), result);
                if (!result.empty()) {
                    vec* nv = new vec(move(result));
                    resultn.push_back({h, nv});
                }
            }
        }
    }
    else if (d2==1) {
        int h = n2[0].first;
        int lb = binarySearch(h, n1, 0, d1);
        if (lb!=d1 && n1[lb].first==h) {
            vec result;
            result.reserve(min(n2[0].second->size(), n1[lb].second->size()));
            utils::commonNeighbor(*(n2[0].second), *(n1[lb].second), result);
            if (!result.empty()) {
                vec* nv = new vec(move(result));
                resultn.push_back({h, nv});
            }
        }
    }
    else {
        int b1 = 0;
        int f1 = d1;
        int b2 = 0;
        int f2 = d2;
        int m1 = n1[0].first;
        int m2 = n2[0].first;
        int M1 = n1[d1-1].first;
        int M2 = n2[d2-1].first;
        if (max(m1,m2)<=min(M1, M2)) {
            if (m1 < m2)
                b1 = binarySearch(m2, n1, b1, f1);
            else if (m2 < m1)
                b2 = binarySearch(m1, n2, b2, f2);
            if (M1 > M2)
                f1 = binarySearch(M2+1, n1, b1, f1);
            else if (M1 < M2)
                f2 = binarySearch(M1+1, n2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                int h = n1[b1].first;
                if (h==n2[b2].first) {
                    vec result;
                    result.reserve(min(n1[b1].second->size(), n2[b2].second->size()));
                    utils::commonNeighbor(*(n1[b1++].second), *(n2[b2++].second), result);
                    if (!result.empty()) {
                        vec* nv = new vec(move(result));
                        resultn.push_back({h, nv});
                    }
                }
                else if (h<n2[b2].first)
                    b1++;
                else
                    b2++;
            }
        }
    }
}

void commonNeighborNB(const adjList& n1, const int d1, const adjList& n2, const int d2, vec& result) {
    if (d1==1) {
        int h = n1[0].first;
        if (d2==1) {
            if(h==n2[0].first)
                utils::commonNeighbor(*(n1[0].second), *(n2[0].second), result);
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb!=d2 && n2[lb].first==h)
                utils::commonNeighbor(*(n1[0].second), *(n2[lb].second), result);
        }
    }
    else if (d2==1) {
        int h = n2[0].first;
        int lb = binarySearch(h, n1, 0, d1);
        if (lb!=d1 && n1[lb].first==h)
            utils::commonNeighbor(*(n2[0].second), *(n1[lb].second), result);
    }
    else {
        int b1 = 0;
        int f1 = d1;
        int b2 = 0;
        int f2 = d2;
        int m1 = n1[0].first;
        int m2 = n2[0].first;
        int M1 = n1[d1-1].first;
        int M2 = n2[d2-1].first;
        if (max(m1,m2)<=min(M1, M2)) {
            if (m1 < m2)
                b1 = binarySearch(m2, n1, b1, f1);
            else if (m2 < m1)
                b2 = binarySearch(m1, n2, b2, f2);
            if (M1 > M2)
                f1 = binarySearch(M2+1, n1, b1, f1);
            else if (M1 < M2)
                f2 = binarySearch(M1+1, n2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                int h = n1[b1].first;
                if (h==n2[b2].first)
                    utils::commonNeighbor(*(n1[b1++].second), *(n2[b2++].second), result);
                else if (h<n2[b2].first)
                    b1++;
                else
                    b2++;
            }
        }
    }
}

void commonNeighborNBP(const vec& n1, const int h, const adjList& n2, const int d2, unsigned long& result) {
    if (n1.size()==1) {
        int v = n1[0];
        if (d2==1) {
            if(h==n2[0].first) {
                vec& l2 = *(n2[0].second);
                int s2 = l2.size();
                if (s2==1) {
                    if (v==l2[0])
                        result++;
                }
                else {
                    int lbv = utils::binarySearch(v, l2, 0, s2);
                    if (lbv!=s2 && l2[lbv]==v)
                        result++;
                }
            }
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb!=d2 && n2[lb].first==h) {
                vec& l2 = *(n2[lb].second);
                int s2 = l2.size();
                int lbv = utils::binarySearch(v, l2, 0, s2);
                if (lbv!=s2 && l2[lbv]==v)
                    result++;
            }
        }
    }
    else {
        int b2 = 0;
        int f2 = d2;
        int b11 = 0;
        int b21 = 0;
        vec* vec2 = n2[0].second;
        int f11 = n1.size();
        int f21 = vec2->size();
        bool c2 = false;
        while (b11 != f11 && b2 != f2) {
            if (c2) {
                b21 = 0;
                vec2 = n2[b2].second;
                f21 = vec2->size();
                c2 = false;
            }
            if (n1[b11]==vec2->operator[](b21)) {
                result++;
                b11++;
                if (b21==f21-1) {
                    b2++;
                    c2=true;
                }
                else
                    b21++;
            }
            else if (n1[b11]<vec2->operator[](b21))
                b11++;
            else {
                if (b21==f21-1) {
                    b2++;
                    c2=true;
                }
                else
                    b21++;
            }
        }
    }
}

void commonNeighborNBP(const vec& n1, const int h, const adjList& n2, const int d2, vec& result) {
    if (n1.size()==1) {
        int v = n1[0];
        if (d2==1) {
            if(h==n2[0].first) {
                vec& l2 = *(n2[0].second);
                int s2 = l2.size();
                if (s2==1) {
                    if (v==l2[0])
                        result.push_back(v);
                }
                else {
                    int lbv = utils::binarySearch(v, l2, 0, s2);
                    if (lbv!=s2 && l2[lbv]==v)
                        result.push_back(v);
                }
            }
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb!=d2 && n2[lb].first==h) {
                vec& l2 = *(n2[lb].second);
                int s2 = l2.size();
                int lbv = utils::binarySearch(v, l2, 0, s2);
                if (lbv!=s2 && l2[lbv]==v)
                    result.push_back(v);
            }
        }
    }
    else {
        int b2 = 0;
        int f2 = d2;
        int b11 = 0;
        int b21 = 0;
        vec* vec2 = n2[0].second;
        int f11 = n1.size();
        int f21 = vec2->size();
        bool c2 = false;
        while (b11 != f11 && b2 != f2) {
            if (c2) {
                b21 = 0;
                vec2 = n2[b2].second;
                f21 = vec2->size();
                c2 = false;
            }
            if (n1[b11]==vec2->operator[](b21)) {
                result.push_back(n1[b11]);
                b11++;
                if (b21==f21-1) {
                    b2++;
                    c2=true;
                }
                else
                    b21++;
            }
            else if (n1[b11]<vec2->operator[](b21))
                b11++;
            else {
                if (b21==f21-1) {
                    b2++;
                    c2=true;
                }
                else
                    b21++;
            }
        }
    }
}

void commonNeighborNB(const vec& vec1, const vec& vec2, unsigned long& result, const int l1, const int l2, const int m, const int M) {
    if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            if (l2==1 && vec1[0]==vec2[0])
                result++;
            else {
                int v = vec1[0];
                if (v>=vec2[0] && v<=vec2[l2-1]) {
                    int lb = utils::binarySearch(v, vec2, 0, l2);
                    if (lb!=l2 && vec2[lb]==v)
                        result++;
                }
            }
        }
        else if (l2==1) {
            int v = vec2[0];
            if (v>=vec1[0] && v<=vec1[l1-1]) {
                int lb = utils::binarySearch(v, vec1, 0, l1);
                if (lb!=l1 && vec1[lb]==v)
                    result++;
            }
        }
        else if (m<=M) {
            int b1 = 0;
            int f1 = l1;
            int b2 = 0;
            int f2 = l2;
            if (vec1[b1]<vec2[b2])
                b1 = utils::binarySearch(vec2[b2], vec1, b1, f1);
            else if (vec1[b1]>vec2[b2])
                b2 = utils::binarySearch(vec1[b1], vec2, b2, f2);
            if (vec1[f1-1]>vec2[f2-1])
                f1 = utils::binarySearch(vec2[f2-1]+1, vec1, b1, f1);
            else if (vec1[f1-1]<vec2[f2-1])
                f2 = utils::binarySearch(vec1[f1-1]+1, vec2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (vec1[b1] == vec2[b2]) {
                    result++;
                    b1++;
                    b2++;
                }
                else if (vec1[b1] < vec2[b2])
                    b1++;
                else
                    b2++;
            }
        }
    }
}

void commonNeighborNB(const vec& vec1, const vec& vec2, vec& result, const int l1, const int l2, const int m, const int M) {
    if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            if (l2==1 && vec1[0]==vec2[0])
                result.push_back(vec1[0]);
            else {
                int v = vec1[0];
                if (v>=vec2[0] && v<=vec2[l2-1]) {
                    int lb = utils::binarySearch(v, vec2, 0, l2);
                    if (lb!=l2 && vec2[lb]==v)
                        result.push_back(v);
                }
            }
        }
        else if (l2==1) {
            int v = vec2[0];
            if (v>=vec1[0] && v<=vec1[l1-1]) {
                int lb = utils::binarySearch(v, vec1, 0, l1);
                if (lb!=l1 && vec1[lb]==v)
                    result.push_back(v);
            }
        }
        else if (m<=M) {
            int b1 = 0;
            int f1 = l1;
            int b2 = 0;
            int f2 = l2;
            if (vec1[b1]<vec2[b2])
                b1 = utils::binarySearch(vec2[b2], vec1, b1, f1);
            else if (vec1[b1]>vec2[b2])
                b2 = utils::binarySearch(vec1[b1], vec2, b2, f2);
            if (vec1[f1-1]>vec2[f2-1])
                f1 = utils::binarySearch(vec2[f2-1]+1, vec1, b1, f1);
            else if (vec1[f1-1]<vec2[f2-1])
                f2 = utils::binarySearch(vec1[f1-1]+1, vec2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (vec1[b1] == vec2[b2]) {
                    result.push_back(vec1[b1]);
                    b1++;
                    b2++;
                }
                else if (vec1[b1] < vec2[b2])
                    b1++;
                else
                    b2++;
            }
        }
    }
}


void setMinus(const adjList& n1, const int d1, const adjList& n2, const int d2, vec& resultn) {
    if (d1==1) {
        int h = n1[0].first;
        vec* l = n1[0].second;
        if (d2==1) {
            if(h!=n2[0].first)
                resultn.insert(resultn.end(), l->begin(), l->end());
            else
                utils::setMinus(*l, *(n2[0].second), resultn);
        }
        else {
            int lb = binarySearch(h, n2, 0, d2);
            if (lb==d2 || n2[lb].first!=h)
                resultn.insert(resultn.end(), l->begin(), l->end());
            else
                utils::setMinus(*l, *(n2[lb].second), resultn);
        }
    }
    else {
        int b1 = 0;
        int f1 = d1;
        int b2 = 0;
        int f2 = d2;
        int m1 = n1[0].first;
        int m2 = n2[0].first;
        int M1 = n1[d1-1].first;
        int M2 = n2[d2-1].first;
        b1 = binarySearch(m2, n1, b1, f1);
        f1 = binarySearch(M2+1, n1, b1, f1);
        for (int i=0;i!=b1;i++) {
            vec* l = n1[i].second;
            resultn.insert(resultn.end(), l->begin(), l->end());
        }
        while (b1 != f1 && b2 != f2) {
            int h = n1[b1].first;
            vec* l = n1[b1].second;
            if (h<n2[b2].first) {
                resultn.insert(resultn.end(), l->begin(), l->end());
                b1++;
            }
            else if (h==n2[b2].first) {
                utils::setMinus(*l, *(n2[b2].second), resultn);
                b1++;
                b2++;
            }
            else
                b2++;
        }
        for (int i=f1;i!=d1;i++) {
            vec* l = n1[i].second;
            resultn.insert(resultn.end(), l->begin(), l->end());
        }
    }
}

int indexVecGraph::selectPivot(const vec& P, const vec& X, const int mp, const int Mp) {
    int p = P[0];
    int si = 0;
    int ps = P.size();
    for (const int& v:P) {
        int gvs = degreesh[v];
        vec& gv = GO[v];
        if (gvs!=0) {
            unsigned long cn = 0;
            commonNeighborNB(P, gv, cn, ps, gvs, max(mp, hashes[gv[0]]), min(Mp, hashes[gv[gvs-1]]));
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    for (const int& v:X) {
        int gvs = degreesh[v];
        vec& gv = GO[v];
        if (gvs!=0) {
            unsigned long cn = 0;
            commonNeighborNB(P, gv, cn, ps, gvs, max(mp, hashes[gv[0]]), min(Mp, hashes[gv[gvs-1]]));
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    return p;
}

void indexVecGraph::BKP(vec& P, vec& X, unsigned long& result) {
    if (P.empty()) {
        if (X.empty())
            result++;
        return;
    }
    int ps = P.size();
    int xs = X.size();
    int u = selectPivot(P, X, hashes[P[0]], hashes[P[ps-1]]);
    int vit1 = 0;
    int vit2 = 0;
    int ud = degreesh[u];
    while (vit1 != ps) {
        if (vit2==ud || P[vit1] < GO[u][vit2]) {
            int v = P[vit1];
            int gvs = degreesh[v];
            vec& gv = GO[v];
            vec NP;
            vec NX;
            if (gvs!=0) {
                NP.reserve(min(ps, gvs));
                commonNeighborNB(P, gv, NP, ps, gvs, max(hashes[P[0]], hashes[gv[0]]), min(hashes[P[ps-1]], hashes[gv[gvs-1]]));
                if (xs!=0) {
                    NX.reserve(min(xs, gvs));
                    commonNeighborNB(X, GO[v], NX, xs, gvs, max(hashes[X[0]], hashes[gv[0]]), min(hashes[X[xs-1]], hashes[gv[gvs-1]]));
                }
            }
            BKP(NP, NX, result);
            P.erase(P.begin()+vit1);
            X.insert(X.begin()+utils::binarySearch(v, X, 0, xs), v);
            ps--;
            xs++;
        }
        else if (P[vit1] == GO[u][vit2]) {
            vit1++;
            vit2++;
        }
        else
            vit2++;
    }
}

int indexVecGraph::selectPivot(const adjList& PN, const int ps, const adjList& XN, const int mp, const int Mp) {
    int p = PN.front().second->front();
    int si = INT_MIN;
    for (const entry& adjl : PN) {
        for (const int v:*(adjl.second)) {
            unsigned long cn = 0;
            if (max(mp, hashesMin[v])<=min(Mp, hashesMax[v])) {
                if (ps==1)
                    commonNeighborNBP(*(PN[0].second), PN[0].first, G[v], degrees[v], cn);
                else
                    commonNeighborNB(PN, ps, G[v], degrees[v], cn);
            }
                // commonNeighborNBP(PN, ps, G[v], degrees[v], cn);
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    for (const entry& adjl : XN) {
        for (const int v:*(adjl.second)) {
            unsigned long cn = 0;
            if (max(mp, hashesMin[v])<=min(Mp, hashesMax[v])) {
                if (ps==1)
                    commonNeighborNBP(*(PN[0].second), PN[0].first, G[v], degrees[v], cn);
                else
                    commonNeighborNB(PN, ps, G[v], degrees[v], cn);
            }
                // commonNeighborNBP(PN, ps, G[v], degrees[v], cn);
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    return p;
}

void indexVecGraph::BKP(adjList& PN, adjList& XN, unsigned long& result) {
    if (PN.empty()) {
        if (XN.empty())
            result++;
        else {
            for (entry& x:XN)
                delete x.second;
        }
        return;
    }
    int ps = PN.size();
    int xs = XN.size();
    int u = selectPivot(PN, ps, XN, PN[0].first, PN[ps-1].first);
    int ud = degrees[u];
    vec PMu;
    PMu.reserve(min(ps, ud));
    setMinus(PN, ps, G[u], ud, PMu);
    for (const int v:PMu) {
        adjList& gv = G[v];
        int gvs = degrees[v];
        // adjList NP;
        // adjList NX;
        vec NP;
        vec NX;
        if (gvs!=0) {
            if (max(PN[0].first, hashesMin[v])<=min(PN[ps-1].first, hashesMax[v])) {
                NP.reserve(min(ps, gvs));
                if (ps==1)
                    commonNeighborNBP(*(PN[0].second), PN[0].first, G[v], degrees[v], NP);
                else
                    commonNeighborNB(PN, ps, gv, gvs, NP);
                // commonNeighborNBP(PN, ps, gv, gvs, NP);
            }
            if (xs!=0 && max(XN[0].first, hashesMin[v])<=min(XN[xs-1].first, hashesMax[v])) {
                NX.reserve(min(xs, gvs));
                if (xs==1)
                    commonNeighborNBP(*(XN[0].second), XN[0].first, G[v], degrees[v], NX);
                else
                    commonNeighborNB(XN, xs, gv, gvs, NX);
            }
        }
        BKP(NP, NX, result);
        int h = hashes[v];
        int hpit = binarySearch(h, PN, 0, ps);
        vec* lp = PN[hpit].second;
        lp->erase(lp->begin()+utils::binarySearch(v, *lp, 0, lp->size()));
        if (lp->empty()) {
            delete lp;
            PN.erase(PN.begin()+hpit);
            ps--;
        }
        int hxit = binarySearch(h, XN, 0, xs);
        if (hxit!=xs && XN[hxit].first==h) {
            vec& lx = *(XN[hxit].second);
            lx.insert(lx.begin()+utils::binarySearch(v, lx, 0, lx.size()), v);
        }
        else {
            vec* nv = new vec(1, v);
            XN.insert(XN.begin()+hxit, {h, nv});
            xs++;
        }
    }
    for (entry& p:PN)
        delete p.second;
    for (entry& x:XN)
        delete x.second;
}

double indexVecGraph::MC(const vec& nodes, const int* Vrank) {
    unsigned long result = 0;
    auto start_time = chrono::high_resolution_clock::now();
    for (const int& n:nodes) {
        int r = Vrank[n];
        int gs = degrees[n];
        adjList PN;
        PN.reserve(gs);
        adjList XN;
        XN.reserve(gs);
        adjList& GN = G[n];
        for (int i=0;i!=GN.size();i++) {
            int h = GN[i].first;
            vec& gn = *(GN[i].second);
            int gns = gn.size();
            bool pf = true;
            bool xf = true;
            bool hM = h==INT_MAX;
            for (const int& adj:gn) {
                if (Vrank[adj] > r) {
                    if (hM)
                        result++;
                    else if (pf) {
                        vec* nv = new vec();
                        PN.push_back({h, nv});
                        PN.back().second->reserve(gns);
                        PN.back().second->push_back(adj);
                        pf = false;
                    }
                    else
                        PN.back().second->push_back(adj);
                }
                else if (!hM) {
                    if (xf) {
                        vec* nv = new vec();
                        XN.push_back({h, nv});
                        XN.back().second->reserve(gns);
                        xf = false;
                    }
                    XN.back().second->push_back(adj);
                }
            }
        }
        if (PN.empty() && XN.empty())
            continue;
        BKP(PN, XN, result);
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total MC:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}
