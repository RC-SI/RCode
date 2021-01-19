#include "indexGraph.h"

void indexGraph::processHGIndex(const vec* OG) {
    for (int i=0; i<N; i++) {
        int newid = ids[i];
        G[newid].reserve(OG[i].size());   
        for (const int& adj : OG[i]) 
            G[newid].push_back(ids[adj]);
        sort(G[newid].begin(), G[newid].end());
        int prev = INT_MIN;
        int j=0;
        while (j!=G[newid].size()) {
            int adj = G[newid][j];
            if (hashes[adj]!=prev) {
                prev = hashes[adj];
                if (prev==INT_MAX)
                    break;
                hashEntries[newid].push_back(prev);
                hashIds[newid].push_back(j);
            }
            j++;
        }
        if (!hashEntries[newid].empty()) {
            hashesMin[newid] = hashEntries[newid].front();
            hashesMax[newid] = hashEntries[newid].back();
        }
        else {
            hashesMin[newid] = INT_MAX;
            hashesMax[newid] = INT_MIN;
        }
        degrees[newid] = hashEntries[newid].size();
        hashEntries[newid].push_back(INT_MAX);
        hashIds[newid].push_back(j);
    }
}

#if CNV == 1
void indexGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    int m1 = hashesMin[i];
    int m2 = hashesMin[n];
    int M1 = hashesMax[i];
    int M2 = hashesMax[n];
    if (s1==1 && l1 == 0) {
        l2 = hashIds[n][l2];
        int v = vec1[0];
        if (s2==1 && v==vec2[0])
            result++;
        else if (l2<s2 && v>=vec2[l2] && v<=vec2[s2-1]) {                    
            int lb = utils::binarySearch(v, vec2, l2, s2);
            if (lb!=s2 && vec2[lb]==v)
                result++;
        }
    }
    else if (s2==1 && l2 == 0 && l1<s1) {
        l1 = hashIds[i][l1];
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0) {
        vec& hashEnts1 = hashEntries[i];
        vec& hashEnts2 = hashEntries[n];
        vec& hashId1 = hashIds[i];
        vec& hashId2 = hashIds[n];
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
            if (v>=vec1[0] && v<=vec1[s1-1]) {
                int lb = utils::binarySearch(v, vec1, 0, l1);
                if (lb!=l1 && vec1[lb]==v)
                    result++;
            }
        }
        else if (max(m1, m2)<=min(M1, M2)) {
            int b1 = 0;
            int f1 = l1;
            int b2 = 0;
            int f2 = l2;
            if (m1 < m2)
                b1 = utils::binarySearch(m2, hashEnts1, b1, f1);
            else if (m1 > m2)
                b2 = utils::binarySearch(m1, hashEnts2, b2, f2);
            if (M1 > M2)
                f1 = utils::binarySearch(M2+1, hashEnts1, b1, f1);
            else if (M1 < M2)
                f2 = utils::binarySearch(M1+1, hashEnts2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (hashEnts1[b1] == hashEnts2[b2]) {
                    int vit1 = hashId1[b1];
                    int vit2 = hashId2[b2];
                    int ved1 = hashId1[b1+1];
                    int ved2 = hashId2[b2+1];
                    while (vit1 != ved1 && vit2 != ved2) {
                        if (vec1[vit1] == vec2[vit2]) {
                            result++;
                            vit1++;
                            vit2++;
                        }
                        else if (vec1[vit1] < vec2[vit2])
                            vit1++;
                        else
                            vit2++;
                    }
                    b1++;
                    b2++;
                }
                else if (hashEnts1[b1] < hashEnts2[b2])
                    b1++;
                else
                    b2++;
            }
        }
    }
}
#elif CNV == 2
void indexGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    int m1 = hashesMin[i];
    int m2 = hashesMin[n];
    int M1 = hashesMax[i];
    int M2 = hashesMax[n];
    if (l1 != 0 && l2 != 0) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
        vec& hashEnts1 = hashEntries[i];
        vec& hashEnts2 = hashEntries[n];
        vec& hashId1 = hashIds[i];
        vec& hashId2 = hashIds[n];
        int s1 = hashId1[l1];
        int s2 = hashId2[l2];
        if (s1==1) {
            if (s2==1 && vec1[0]==vec2[0])
                result++;
            else {
                int v = vec1[0];
                if (v>=vec2[0] && v<=vec2[s2-1]) {
                    int lb = utils::binarySearch(v, vec2, 0, s2);
                    if (lb!=s2 && vec2[lb]==v)
                        result++;
                }
            }
        }
        else if (s2==1) {
            int v = vec2[0];
            if (v>=vec1[0] && v<=vec1[s1-1]) {
                int lb = utils::binarySearch(v, vec1, 0, s1);
                if (lb!=s1 && vec1[lb]==v)
                    result++;
            }
        }
        else if (max(m1, m2)<=min(M1, M2)) {
            int b1 = 0;
            int f1 = l1;
            int b2 = 0;
            int f2 = l2;
            if (m1 < m2)
                b1 = utils::binarySearch(m2, hashEnts1, b1, f1);
            else if (m1 > m2)
                b2 = utils::binarySearch(m1, hashEnts2, b2, f2);
            if (M1 > M2)
                f1 = utils::binarySearch(M2+1, hashEnts1, b1, f1);
            else if (M1 < M2)
                f2 = utils::binarySearch(M1+1, hashEnts2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (hashEnts1[b1] == hashEnts2[b2]) {
                    int vit1 = hashId1[b1];
                    int vit2 = hashId2[b2];
                    int ved1 = hashId1[b1+1];
                    int ved2 = hashId2[b2+1];
                    while (vit1 != ved1 && vit2 != ved2) {
                        if (vec1[vit1] == vec2[vit2]) {
                            result++;
                            vit1++;
                            vit2++;
                        }
                        else if (vec1[vit1] < vec2[vit2])
                            vit1++;
                        else
                            vit2++;
                    }
                    b1++;
                    b2++;
                }
                else if (hashEnts1[b1] < hashEnts2[b2])
                    b1++;
                else
                    b2++;
            }
        }
    }
}
#elif CNV == 3
void indexGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    int m1 = hashesMin[i];
    int m2 = hashesMin[n];
    int M1 = hashesMax[i];
    int M2 = hashesMax[n];
    if (s1==1 && l1 == 0) {
        l2 = hashIds[n][l2];
        int v = vec1[0];
        if (s2==1 && v==vec2[0])
            result++;
        else if (l2<s2 && v>=vec2[l2] && v<=vec2[s2-1]) {                    
            int lb = utils::binarySearch(v, vec2, l2, s2);
            if (lb!=s2 && vec2[lb]==v)
                result++;
        }
    }
    else if (s2==1 && l2 == 0 && l1<s1) {
        l1 = hashIds[i][l1];
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0 && max(m1, m2)<=min(M1, M2)) {
        vec& hashEnts1 = hashEntries[i];
        vec& hashEnts2 = hashEntries[n];
        vec& hashId1 = hashIds[i];
        vec& hashId2 = hashIds[n];
        int b1 = 0;
        int f1 = l1;
        int b2 = 0;
        int f2 = l2;
        if (m1 < m2)
            b1 = utils::binarySearch(m2, hashEnts1, b1, f1);
        else if (m1 > m2)
            b2 = utils::binarySearch(m1, hashEnts2, b2, f2);
        if (M1 > M2)
            f1 = utils::binarySearch(M2+1, hashEnts1, b1, f1);
        else if (M1 < M2)
            f2 = utils::binarySearch(M1+1, hashEnts2, b2, f2);
        while (b1 != f1 && b2 != f2) {
            if (hashEnts1[b1] == hashEnts2[b2]) {
                int vit1 = hashId1[b1];
                int vit2 = hashId2[b2];
                int ved1 = hashId1[b1+1];
                int ved2 = hashId2[b2+1];
                while (vit1 != ved1 && vit2 != ved2) {
                    if (vec1[vit1] == vec2[vit2]) {
                        result++;
                        vit1++;
                        vit2++;
                    }
                    else if (vec1[vit1] < vec2[vit2])
                        vit1++;
                    else
                        vit2++;
                }
                b1++;
                b2++;
            }
            else if (hashEnts1[b1] < hashEnts2[b2])
                b1++;
            else
                b2++;
        }
    }
}
#endif

#if CNNBV == 1
void indexGraph::commonNeighborNB(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    if (l1 != 0 && l2 != 0) {
        int m1 = hashesMin[i];
        int m2 = hashesMin[n];
        int M1 = hashesMax[i];
        int M2 = hashesMax[n];
        vec& vec1 = G[i];
        vec& vec2 = G[n];
        vec& hashEnts1 = hashEntries[i];
        vec& hashEnts2 = hashEntries[n];
        vec& hashId1 = hashIds[i];
        vec& hashId2 = hashIds[n];
        int s1 = vec1.size();
        int s2 = vec2.size();
        if (s1==1) {
            if (s2==1 && vec1[0]==vec2[0])
                result++;
            else {
                int v = vec1[0];
                if (v>=vec2[0] && v<=vec2[s2-1]) {
                    int lb = utils::binarySearch(v, vec2, 0, s2);
                    if (lb!=s2 && vec2[lb]==v)
                        result++;
                }
            }
        }
        else if (s2==1) {
            int v = vec2[0];
            if (v>=vec1[0] && v<=vec1[s1-1]) {
                int lb = utils::binarySearch(v, vec1, 0, s1);
                if (lb!=s1 && vec1[lb]==v)
                    result++;
            }
        }
        else if (max(m1, m2)<=min(M1, M2)) {
            int b1 = 0;
            int f1 = l1;
            int b2 = 0;
            int f2 = l2;
            if (m1 < m2)
                b1 = utils::binarySearch(m2, hashEnts1, b1, f1);
            else if (m1 > m2)
                b2 = utils::binarySearch(m1, hashEnts2, b2, f2);
            if (M1 > M2)
                f1 = utils::binarySearch(M2+1, hashEnts1, b1, f1);
            else if (M1 < M2)
                f2 = utils::binarySearch(M1+1, hashEnts2, b2, f2);
            while (b1 != f1 && b2 != f2) {
                if (hashEnts1[b1] == hashEnts2[b2]) {
                    int vit1 = hashId1[b1];
                    int vit2 = hashId2[b2];
                    int ved1 = hashId1[b1+1];
                    int ved2 = hashId2[b2+1];
                    while (vit1 != ved1 && vit2 != ved2) {
                        if (vec1[vit1] == vec2[vit2]) {
                            result++;
                            vit1++;
                            vit2++;
                        }
                        else if (vec1[vit1] < vec2[vit2])
                            vit1++;
                        else
                            vit2++;
                    }
                    b1++;
                    b2++;
                }
                else if (hashEnts1[b1] < hashEnts2[b2])
                    b1++;
                else
                    b2++;
            }
        }
    }
}
#elif CNNBV == 2
void indexGraph::commonNeighborNB(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    int m1 = hashesMin[i];
    int m2 = hashesMin[n];
    int M1 = hashesMax[i];
    int M2 = hashesMax[n];
    if (l1 != 0 && l2 != 0 && max(m1, m2)<=min(M1, M2)) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
        vec& hashEnts1 = hashEntries[i];
        vec& hashEnts2 = hashEntries[n];
        vec& hashId1 = hashIds[i];
        vec& hashId2 = hashIds[n];
        int b1 = 0;
        int f1 = l1;
        int b2 = 0;
        int f2 = l2;
        if (m1 < m2)
            b1 = utils::binarySearch(m2, hashEnts1, b1, f1);
        else if (m1 > m2)
            b2 = utils::binarySearch(m1, hashEnts2, b2, f2);
        if (M1 > M2)
            f1 = utils::binarySearch(M2+1, hashEnts1, b1, f1);
        else if (M1 < M2)
            f2 = utils::binarySearch(M1+1, hashEnts2, b2, f2);
        while (b1 != f1 && b2 != f2) {
            if (hashEnts1[b1] == hashEnts2[b2]) {
                int vit1 = hashId1[b1];
                int vit2 = hashId2[b2];
                int ved1 = hashId1[b1+1];
                int ved2 = hashId2[b2+1];
                while (vit1 != ved1 && vit2 != ved2) {
                    if (vec1[vit1] == vec2[vit2]) {
                        result++;
                        vit1++;
                        vit2++;
                    }
                    else if (vec1[vit1] < vec2[vit2])
                        vit1++;
                    else
                        vit2++;
                }
                b1++;
                b2++;
            }
            else if (hashEnts1[b1] < hashEnts2[b2])
                b1++;
            else
                b2++;
        }
    }
}
#endif

pair<int, int> indexGraph::calCP2(const int i, const int n, const int m, const int M) {
    int result = 0;
    vec& hashEnts1 = hashEntries[i];
    vec& hashEnts2 = hashEntries[n];
    vec& hashId1 = hashIds[i];
    vec& hashId2 = hashIds[n];
    int l1 = degrees[i];
    int l2 = degrees[n];
    int b1 = 0;
    int f1 = l1;
    int b2 = 0;
    int f2 = l2;
    if (hashesMin[i] < hashesMin[n])
        b1 = utils::binarySearch(hashesMin[n], hashEnts1, b1, f1);
    else if (hashesMin[i] > hashesMin[n])
        b2 = utils::binarySearch(hashesMin[i], hashEnts2, b2, f2);
    if (hashesMax[i] > hashesMax[n])
        f1 = utils::binarySearch(hashesMax[n]+1, hashEnts1, b1, f1);
    else if (hashesMax[i] < hashesMax[n])
        f2 = utils::binarySearch(hashesMax[i]+1, hashEnts2, b2, f2);
    int add = f1 - b1 + f2 - b2;
    while (b1 != f1 && b2 != f2) {
        if (hashEnts1[b1] == hashEnts2[b2]) {
            result += hashId1[b1+1] - hashId1[b1] + hashId2[b2+1] - hashId2[b2];
            b1++;
            b2++;
        }
        else if (hashEnts1[b1] < hashEnts2[b2])
            b1++;
        else
            b2++;
    }
    return make_pair(result, result + add);
}

double indexGraph::calF(const pvec& nodePairs, const bool NB) {
    tqdm bar;
    int marked = 0;
    long double CNDP = 0.0;
    for (const pair<int, int>& p:nodePairs) {
        int i = ids[p.first];
        int n = ids[p.second];
        unsigned long CN = 0;
        int CP = 0;
        Graph::commonNeighbor(i, n, CN);
        if (G[i].size() == 1 || G[n].size() == 1 || hashIds[i][degrees[i]]==1 || hashIds[n][degrees[n]]==1)
            CP = 2;
        else if (degrees[i] != 0 && degrees[n] != 0) {
            int m = max(hashesMin[i], hashesMin[n]);
            int M = min(hashesMax[i], hashesMax[n]);
            if (m<=M)
                CP = calCP2(i, n, m, M).second;
        }
        CNDP = CNDP + (double) (2*CN+1)/(CP+1);
        bar.progress(++marked, N);
    }
    CNDP = CNDP/nodePairs.size();
    return CNDP;
}

double indexGraph::calF2(const pvec& nodePairs, const bool NB) {
    tqdm bar;
    int marked = 0;
    long double CNDP = 0.0;
    for (const pair<int, int>& p:nodePairs) {
        int i = ids[p.first];
        int n = ids[p.second];
        unsigned long CN = 0;
        int CP = 0;
        Graph::commonNeighbor(i, n, CN);
        if (G[i].size() == 1 || G[n].size() == 1 || hashIds[i][degrees[i]]==1 || hashIds[n][degrees[n]]==1)
            CP = 2;
        else if (degrees[i] != 0 && degrees[n] != 0) {
            int m = max(hashesMin[i], hashesMin[n]);
            int M = min(hashesMax[i], hashesMax[n]);
            if (m<=M)
                CP = calCP2(i, n, m, M).first;
        }
        CNDP = CNDP + (double) (2*CN+1)/(CP+1);
        bar.progress(++marked, N);
    }
    CNDP = CNDP/nodePairs.size();
    return CNDP;
}

void indexGraph::calRate() {
    double r = 0.0;
    for (int i=0; i<N; i++) {
        r = r + (double)(hashEntries[i].size()-1)/G[i].size();
    }
    r = r/N;
    cout << "R: " << r << endl;
}