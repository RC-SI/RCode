#include "hashGraph.h"

void hashGraph::processHG(const vec* OG) {
    tqdm bar;
    int marked = 0;
    for (int i=0; i<N; i++) {
        int newid = ids[i];
        G[newid].reserve(OG[i].size());
        for (const int& adj : OG[i])
            G[newid].push_back(ids[adj]);
        sort(G[newid].begin(), G[newid].end());
        if (hashes[G[newid].front()] == INT_MAX) {
            degrees[newid] = 0;
            hashesMin[newid] = INT_MAX;
            hashesMax[newid] = INT_MIN;
        }
        else {
            hashesMin[newid] = hashes[G[newid].front()];
            int back = utils::binarySearch(INT_MAX, G[newid], hashes);
            hashesMax[newid] = hashes[G[newid][back-1]];
            degrees[newid] = back;
        }
        bar.progress(++marked, N);
    }
}

#if CNV == 1
void hashGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    if (s1==1 && l1 == 0) {
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
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            int v = vec1[0];
            if (l2==1 && v==vec2[0])
                result++;
            else if (v>=vec2[0] && v<=vec2[l2-1]) {
                int lb = utils::binarySearch(v, vec2, 0, l2);
                if (lb!=l2 && vec2[lb]==v)
                    result++;
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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

void hashGraph::commonNeighborBS(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    if (s1==1 && l1 == 0) {
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
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            int v = vec1[0];
            if (l2==1 && v==vec2[0])
                result++;
            else if (v>=vec2[0] && v<=vec2[l2-1]) {
                int lb = utils::binarySearch(v, vec2, 0, l2);
                if (lb!=l2 && vec2[lb]==v)
                    result++;
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
            if (f1-b1<f2-b2)
                utils::commonNeighborBS(vec1, vec2, result, b1, f1, b2, f2);
            else
                utils::commonNeighborBS(vec2, vec1, result, b2, f2, b1, f1);
        }
    }
}
void hashGraph::commonNeighborCP(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    if (s1==1 && l1 == 0) {
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
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            int v = vec1[0];
            if (l2==1 && v==vec2[0])
                result++;
            else if (v>=vec2[0] && v<=vec2[l2-1]) {
                int lb = utils::binarySearch(v, vec2, 0, l2);
                if (lb!=l2 && vec2[lb]==v)
                    result++;
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
                    b1++;
                    b2++;
                }
                else if (vec1[b1] < vec2[b2])
                    b1++;
                else
                    b2++;
                result++;
            }
        }
    }
}

#elif CNV == 2
void hashGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    if (l1 != 0 && l2 != 0) {
        if (l1==1) {
            int v = vec1[0];
            if (l2==1 && v==vec2[0])
                result++;
            else if (v>=vec2[0] && v<=vec2[l2-1]) {
                int lb = utils::binarySearch(v, vec2, 0, l2);
                if (lb!=l2 && vec2[lb]==v)
                    result++;
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
#elif CNV == 3
void hashGraph::commonNeighbor(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    vec& vec1 = G[i];
    vec& vec2 = G[n];
    int s1 = vec1.size();
    int s2 = vec2.size();
    if (s1==1 && l1 == 0) {
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
        int v = vec2[0];
        if (v>=vec1[l1] && v<=vec1[s1-1]) {
            int lb = utils::binarySearch(v, vec1, l1, s1);
            if (lb!=s1 && vec1[lb]==v)
                result++;
        }
    }
    else if (l1 != 0 && l2 != 0 && max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
#endif

void hashGraph::commonNeighborNB(const vec& vec1, const vec& vec2, unsigned long& result, const int l1, const int l2, const int m, const int M) {
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

void hashGraph::commonNeighborNB(const vec& vec1, const vec& vec2, vec& result, const int l1, const int l2, const int m, const int M) {
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

#if CNNBV == 1
void hashGraph::commonNeighborNB(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    if (l1 != 0 && l2 != 0) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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

void hashGraph::commonNeighborNBCP(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    if (l1 != 0 && l2 != 0) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
                    b1++;
                    b2++;
                }
                else if (vec1[b1] < vec2[b2])
                    b1++;
                else
                    b2++;
                result++;
            }
        }
    }
}

void hashGraph::commonNeighborNBBS(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    if (l1 != 0 && l2 != 0) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
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
        else if (max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
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
            if (f1-b1<f2-b2)
                utils::commonNeighborBS(vec1, vec2, result, b1, f1, b2, f2);
            else
                utils::commonNeighborBS(vec2, vec1, result, b2, f2, b1, f1);
        }
    }
}

#elif CNNBV == 2
void hashGraph::commonNeighborNB(const int i, const int n, unsigned long& result) {
    int l1 = degrees[i];
    int l2 = degrees[n];
    if (l1 != 0 && l2 != 0 && max(hashesMin[i], hashesMin[n])<=min(hashesMax[i], hashesMax[n])) {
        vec& vec1 = G[i];
        vec& vec2 = G[n];
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
#endif

double hashGraph::calCN(const pvec& nodePairs, const bool NB) {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    if (NB) {
        for (const pair<int, int>& p:nodePairs)
            commonNeighborNB(ids[p.first], ids[p.second], result);
    }
    else {
        for (const pair<int, int>& p:nodePairs) 
            commonNeighbor(ids[p.first], ids[p.second], result);
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total CNs:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

double hashGraph::calCNBS(const pvec& nodePairs, const bool NB) {
    auto start_time = chrono::high_resolution_clock::now();
    unsigned long result = 0;
    if (NB) {
        for (const pair<int, int>& p:nodePairs)
            commonNeighborNBBS(ids[p.first], ids[p.second], result);
    }
    else {
        for (const pair<int, int>& p:nodePairs) 
            commonNeighborBS(ids[p.first], ids[p.second], result);
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total CNs:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

unsigned long hashGraph::calCNCP(const pvec& nodePairs, const bool NB) {
    unsigned long result = 0;
    if (NB) {
        for (const pair<int, int>& p:nodePairs)
            commonNeighborNBCP(ids[p.first], ids[p.second], result);
    }
    else {
        for (const pair<int, int>& p:nodePairs) 
            commonNeighborCP(ids[p.first], ids[p.second], result);
    }
    return result;
}


double hashGraph::calF(const pvec& nodePairs, const bool NB) {
    tqdm bar;
    long double CNDP = 0.0;
    int marked = 0;
    int psize = nodePairs.size();
    for (const pair<int, int>& p:nodePairs) {
        int i = ids[p.first];
        int n = ids[p.second];
        unsigned long CN = 0;
        int CP=0;
        Graph::commonNeighbor(i, n, CN);
        if (G[i].size() == 1 || G[n].size() == 1 || degrees[i]==1 || degrees[n]==1)
            CP = 2;
        else if (degrees[i] != 0 && degrees[n] != 0) {
            int m = max(hashesMin[i], hashesMin[n]);
            int M = min(hashesMax[i], hashesMax[n]);
            if (m<=M) {
                int il = degrees[i];
                int jl = degrees[n];
                CP = lower_bound(G[i].begin(), G[i].begin()+il, G[n][jl-1]+1)-lower_bound(G[i].begin(), G[i].begin()+il, G[n].front())
                        +lower_bound(G[n].begin(), G[n].begin()+jl, G[i][il-1]+1)-lower_bound(G[n].begin(), G[n].begin()+jl, G[i].front());
            }
        }
        CNDP = CNDP + (double)(2*CN+1)/(CP+1);
        bar.progress(++marked, psize);
    }
    CNDP = CNDP / psize;
    return CNDP;
}

double hashGraph::calR(const pvec& nodePairs) {
    tqdm bar;
    long double R = 0.0;
    int marked = 0;
    int psize = nodePairs.size();
    for (const pair<int, int>& p:nodePairs) {
        int i = p.first;
        int n = p.second;
        int CP1 = 0;
        int CP2 = 0;
        if (G[i].size() != 1 && G[n].size() != 1 && degrees[i] > 1 && degrees[n] >1) {
            int m = max(hashesMin[i], hashesMin[n]);
            int M = min(hashesMax[i], hashesMax[n]);
            if (m<=M) {
                int il = degrees[i];
                int jl = degrees[n];
                CP1 = lower_bound(G[i].begin(), G[i].begin()+il, G[n][jl-1]+1)-lower_bound(G[i].begin(), G[i].begin()+il, G[n].front());
                CP2 = lower_bound(G[n].begin(), G[n].begin()+jl, G[i][il-1]+1)-lower_bound(G[n].begin(), G[n].begin()+jl, G[i].front());
            }
        }
        R += (CP1 != 0 && CP2 != 0)?(double) min(CP1, CP2)/max(CP1, CP2):1;
        bar.progress(++marked, psize);
    }
    R /= psize;
    return R;
}

int hashGraph::selectPivot(const vec& P, const vec& X, const int mp, const int Mp) {
    int p = P[0];
    int si = 0;
    int ps = P.size();
    for (const int& v:P) {
        int gvs = degrees[v];
        if (gvs!=0) {
            unsigned long cn = 0;
            commonNeighborNB(P, G[v], cn, ps, gvs, max(mp, hashes[G[v][0]]), min(Mp, hashes[G[v][gvs-1]]));
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    for (const int& v:X) {
        int gvs = degrees[v];
        if (gvs!=0) {
            unsigned long cn = 0;
            commonNeighborNB(P, G[v], cn, ps, gvs, max(mp, hashes[G[v][0]]), min(Mp, hashes[G[v][gvs-1]]));
            if (cn>si) {
                si = cn;
                p = v;
            }
        }
    }
    return p;
}

void hashGraph::BKP(vec& P, vec& X, unsigned long& result) {
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
    int ud = degrees[u];
    while (vit1 != ps) {
        if (vit2==ud || P[vit1] < G[u][vit2]) {
            int v = P[vit1];
            int gvs = degrees[v];
            vec NP;
            vec NX;
            if (gvs!=0) {
                NP.reserve(min(ps, gvs));
                NX.reserve(min(xs, gvs));
                commonNeighborNB(P, G[v], NP, ps, gvs, max(hashes[P[0]], hashes[G[v][0]]), min(hashes[P[ps-1]], hashes[G[v][gvs-1]]));
                if (xs!=0)
                    commonNeighborNB(X, G[v], NX, xs, gvs, max(hashes[X[0]], hashes[G[v][0]]), min(hashes[X[xs-1]], hashes[G[v][gvs-1]]));
            }
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

double hashGraph::MC(const vec& nodes, const int* Vrank) {
    unsigned long result = 0;
    auto start_time = chrono::high_resolution_clock::now();
    for (const auto& n:nodes) {
        int i = Vrank[n];
        int gns = degrees[n];
        vec P;
        P.reserve(gns);
        vec X;
        X.reserve(gns);
        for (const int& adj:G[n]) {
            if (Vrank[adj] > i) {
                if (hashes[adj]==INT_MAX)
                    result++;
                else
                    P.push_back(adj);
            }
            else if (hashes[adj]!=INT_MAX)
                X.push_back(adj);
        }
        if (P.empty() && X.empty())
            continue;
        BKP(P, X, result);
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << endl << "Total MC:" << result << endl;
    chrono::duration<double> diff = end_time-start_time;
    return diff.count();
}

void hashGraph::writeHashId(const string hfilename, const string idfilename) {
    ofstream idfile;
    idfile.open(idfilename);
    idfile << ":ID" << endl;
    for (int i=0; i<N; i++)
        idfile << ids[i] << endl;
    ofstream nodefile;
    nodefile.open(hfilename);
    nodefile << ":ID,h:int" << endl;
    for (int i=0; i<N; i++)
        nodefile << i << "," << hashes[i] << endl;
}