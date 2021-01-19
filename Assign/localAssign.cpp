#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <climits>
#include <assert.h>
#include <sys/time.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <numeric>
#include <set>
#include <array>
#include <chrono>
#include <boost/dynamic_bitset.hpp>
#include "../include/tqdm.h"

using namespace std;

typedef vector<int> vec;

void readGraph(string file, vec*& G, int N) {
    ifstream hash_File(file.c_str());
    string currLine;
    while(getline(hash_File, currLine)) {
        stringstream linestream(currLine);
        string ids;
        getline(linestream,ids,' ');
        int idx = stoi(ids);
        string ns;
        getline(linestream,ns,' ');
        int nid = stoi(ns);
        G[idx].push_back(nid);
        G[nid].push_back(idx);
    }
    for (int i=0; i<N; i++) {
        sort(G[i].begin(), G[i].end());
        vec::iterator uniqEnd = unique(G[i].begin(), G[i].end());
        G[i].erase(uniqEnd, G[i].end());
    }
}

template <typename T>
void sort_indexes(T*& v, int*& ids, int N) {
    int* idx = new int[N];
    iota(idx, idx+N, 0);
    stable_sort(idx, idx+N, [v](int i1, int i2) {return v[i1] < v[i2];});

    T* newV = new T[N];
    for (int i=0; i<N; i++) {
        ids[idx[i]] = i;
        newV[i] = v[idx[i]];
    }
    delete[] v;
    v = newV;
}

inline int binarySearch(int _key, const vec& _list, int _left, int _right) {
	int _mid;
	while (_left < _right) {
		_mid = (_right + _left) / 2;
		if (_list[_mid] < _key)
            _left = _mid+1;
        else
            _right = _mid;
	}
	return _left;
}

inline void commonNeighbor(vec& vec1, vec& vec2, int& cn, int id1, int ed1, int id2, int ed2) {
    while (id1 != ed1 && id2 != ed2) {
        if (vec1[id1]==vec2[id2]) {
            cn++;
            id1++;
            id2++;
        }
        else if (vec1[id1]<vec2[id2])
            id1++;
        else
            id2++;
    }
}

inline void updateMinMaxNB(vec*& G, int node, int h, unordered_map<int, unsigned>*& hashesNums, queue<int>& candidates, unordered_set<int>& candidatesUniq, int*& hashesMin, int*& hashesMax) {
    for (const auto& n : G[node]) {
        if (h<hashesMin[n])
            hashesMin[n] = h;
        if (h>hashesMax[n])
            hashesMax[n] = h;
        if (candidatesUniq.find(n) == candidatesUniq.end()) {
            candidates.push(n);
            candidatesUniq.insert(n);
        }
    }
    for (const auto& n : G[node]) {
        for (const auto& n2 : G[n]) {
            if (h<hashesMin[n2] || h>hashesMax[n2]) {
                unordered_map<int, unsigned>& hn2 = hashesNums[n2];
                if (hn2.find(h) != hn2.end())
                    hn2[h]++;
                else
                    hn2[h] = 1;
            }
        }
    }
}

void hashMergeDiffNBEx(vec*& G, int*& except, int*& hashes, int*& hashesMin, int* hashesMax, int maxDegreeNode, int N) {
    unordered_map<int, unsigned>* hashesNums = new unordered_map<int, unsigned>[N]();
    boost::dynamic_bitset<> adjVec(N);
    boost::dynamic_bitset<> marked(N);
    marked.set(maxDegreeNode);
    hashes[maxDegreeNode] = 1;
    int upper = 2;
    int lower = 0;
    // vector<unsigned> currNeighbors = adjlist[maxDegreeNode];
    queue<int> candidates;
    unordered_set<int> candidatesUniq;
    updateMinMaxNB(G, maxDegreeNode, 1, hashesNums, candidates, candidatesUniq, hashesMin, hashesMax);
    tqdm bar;
    bar.progress(1, N);
    int markedNum = 1;
    maxDegreeNode = 1;
    while (markedNum != N) {
        int n;
        if (!candidates.empty()) {
            n = candidates.front();
            candidates.pop();
            candidatesUniq.erase(n);
            if (n == maxDegreeNode)
                maxDegreeNode++;
        }
        else {
            n = maxDegreeNode;
            maxDegreeNode++;
        }
        if (!marked.test(n)) {
            if (except[n]==0) {
                marked.set(n);
                markedNum++;
                bar.progress(markedNum, N);
                continue;
            }
            vec& gn = G[n];
            for (const auto& adj : gn)
                adjVec.set(adj);
            int prev = 0;
            int minus = 0;
            int hnum = upper-lower+1;
            int* currNum = new int[hnum];
            fill_n(currNum, hnum, 0);
            unsigned triNum = 0;
            for (const int adj : gn) {
                if (hashesMin[adj]!=INT_MAX) {
                    unordered_map<int, unsigned>& hn = hashesNums[adj];
                    for (int uh=lower; uh<=hashesMin[adj]-1; uh++) {
                        if (hn.find(uh) != hn.end())
                            prev += hn[uh];
                    }
                }
                for (const int adj2 : G[adj]) {
                    if (adjVec.test(adj2))
                        triNum++;
                    else if (hashesMin[adj2]!=INT_MAX) {
                        currNum[hashesMin[adj2]-lower]++;
                        currNum[hashesMax[adj2]+1-lower]--;
                    }
                }
            }
            for (int i=1; i<hnum; i++)
                currNum[i] = currNum[i-1]+currNum[i];
            int minScore = INT_MAX;
            int hstar = upper;
            for (int h=lower; h<=upper; h++) {
                int score = triNum + currNum[h-lower]-minus;
                for (const auto& adj : G[n]) {
                    unordered_map<int, unsigned>& hn = hashesNums[adj];
                    if (hn.find(h) != hn.end()) {
                        if (h<hashesMin[adj])
                            minus = minus + hn[h];
                        if (h>hashesMax[adj])
                            prev = prev + hn[h];
                    }
                }
                score = prev + score;
                if (h == lower || (h != upper && score <= minScore) || (h == upper && score < minScore)) {
                    minScore = score;
                    hstar = h;
                }
            }
            // cout << hstar << endl;
            hashes[n] = hstar;
            if (hstar == upper)
                upper++;
            else if (hstar == lower)
                lower--;
            updateMinMaxNB(G, n, hstar, hashesNums, candidates, candidatesUniq, hashesMin, hashesMax);
            delete[] currNum;
            adjVec.reset();
            marked.set(n);
            markedNum++;
            bar.progress(markedNum, N);
        }
    }
    delete[] hashesNums;
}

inline void updateMinMaxIndexNB(vec*& G, int node, int h, unordered_map<int, pair<unsigned, unsigned>>*& hashesNums, unordered_set<int>*& hashEntries, queue<int>& candidates, unordered_set<int>& candidatesUniq, int*& hashesMin, int*& hashesMax) {
    for (const auto& n : G[node]) {
        unordered_set<int>& hashEnts = hashEntries[n];
        bool found = (hashEnts.find(h) != hashEnts.end());
        for (const auto& n2 : G[n]) {
            unordered_map<int, pair<unsigned, unsigned>>& hn = hashesNums[n2];
            if (hn.find(h) != hn.end()) {
                hn[h].first++;
                if (!found)
                    hn[h].second++;
            }
            else
                hn[h] = {1,1};
        }
        if (h<hashesMin[n])
            hashesMin[n] = h;
        if (h>hashesMax[n])
            hashesMax[n] = h;
        if (!found)
            hashEnts.insert(h);
        if (candidatesUniq.find(n) == candidatesUniq.end()) {
            candidates.push(n);
            candidatesUniq.insert(n);
        }
    }
}

void hashMergeDiffIndexNBEx(vec*& G, int*& except, int*& hashes, int maxDegreeNode, int N) {
    unordered_map<int, pair<unsigned, unsigned>>* hashesNums = new unordered_map<int, pair<unsigned, unsigned>>[N]();
    unordered_set<int>* hashEntries = new unordered_set<int>[N]();
    boost::dynamic_bitset<> marked(N);
    marked.set(maxDegreeNode);
    int* hashesMin = new int[N];
    int* hashesMax = new int[N];
    fill_n(hashesMin, N, INT_MAX);
    fill_n(hashesMax, N, INT_MIN);
    boost::dynamic_bitset<> adjVec(N);
    hashes[maxDegreeNode] = 1;
    int upper = 2;
    int lower = 0;
    // vector<unsigned> currNeighbors = adjlist[maxDegreeNode];
    queue<int> candidates;
    unordered_set<int> candidatesUniq;
    updateMinMaxIndexNB(G, maxDegreeNode, 1, hashesNums, hashEntries, candidates, candidatesUniq, hashesMin, hashesMax);
    tqdm bar;
    bar.progress(1, N);
    int markedNum = 1;
    maxDegreeNode = 1;
    while (markedNum != N) {
        int n;
        if (!candidates.empty()) {
            n = candidates.front();
            candidates.pop();
            candidatesUniq.erase(n);
            if (n == maxDegreeNode)
                maxDegreeNode++;
        }
        else
            n = maxDegreeNode++;
        if (!marked.test(n)) {
            if (except[n]==0) {
                marked.set(n);
                markedNum++;
                bar.progress(markedNum, N);
                continue;
            }
            const vec& gn = G[n];
            for (const auto& adj : gn)
                adjVec.set(adj);
            int d = gn.size();
            int* prev = new int[d];
            fill_n(prev, d, 0);
            int* minus = new int[d];
            fill_n(minus, d, 0);
            int hnum = upper-lower+1;
            unsigned* adj2num = new unsigned[d];
            fill_n(adj2num, d, 0);
            vector<unsigned*> currNum(d);
            for (int i=0;i!=d;i++) {
                int adj = gn[i];
                unordered_map<int, pair<unsigned, unsigned>>& hn = hashesNums[adj];
                if (hashesMin[adj]!=INT_MAX) {
                    for (int uh=lower; uh<=hashesMin[adj]-1; uh++) {
                        if (hn.find(uh) != hn.end())
                            prev[i] += hn[uh].second;
                    }
                }
                unsigned* currNumVec = new unsigned[hnum];
                fill_n(currNumVec, hnum, 0);
                for (const auto& adj2 : G[adj]) {
                    if (adjVec.test(adj2)) {
                        for (const int& h:hashEntries[adj2])
                            currNumVec[h-lower]++;
                        adj2num[i]++;
                    }
                }
                currNum[i] = currNumVec;
            }
            int minScore = INT_MAX;
            int hstar = upper;
            for (int h=lower; h<=upper; h++) {
                int score = 0;
                for (int i=0;i<d;i++) {
                    int adj = gn[i];
                    int currMinus = minus[i];
                    unsigned a2n = adj2num[i];
                    unordered_map<int, pair<unsigned, unsigned>>& hn = hashesNums[adj];
                    bool fd = hn.find(h) != hn.end();
                    int hnf = 0;
                    int hns = 0;
                    if (fd) {
                        hnf = hn[h].first;
                        hns = hn[h].second;
                    }
                    if (fd) {
                        if (hashesMin[adj] == INT_MAX || (hashesMin[adj] <= h && h <= hashesMax[adj])) {
                            minus[i] += hns;
                            prev[i] += hns;
                        }
                        else if (h<hashesMin[adj])
                            minus[i] += hns;
                        else
                            prev[i] += hns;
                    }
                    if (hashEntries[adj].find(h) != hashEntries[adj].end())
                        score += (a2n - currNum[i][h-lower]) + hns;
                    else
                        score = score + a2n + (a2n - currNum[i][h-lower]) - currMinus + prev[i] + hnf;
                }
                if (h==lower || (h!=upper && score <= minScore) || (h==upper && score < minScore)) {
                    minScore = score;
                    hstar = h;
                }
            }
            hashes[n] = hstar;
            if (hstar == upper)
                upper++;
            else if (hstar == lower)
                lower--;
            updateMinMaxIndexNB(G, n, hstar, hashesNums, hashEntries, candidates, candidatesUniq, hashesMin, hashesMax);
            adjVec.reset();
            delete[] prev;
            delete[] minus;
            delete[] adj2num;
            for (const auto& hn:currNum)
                delete[] hn;
            marked.set(n);
            markedNum++;
            bar.progress(markedNum, N);
        }
    }
    delete[] hashesMin;
    delete[] hashesMax;
    delete[] hashesNums;
    delete[] hashEntries;
}

inline int binarySearch(int _key, const vec& _list, int*& _hashes) {
	int _left = 0;
    int _right = _list.size();
    int _mid;
	while (_left < _right) {
		_mid = (_right + _left) / 2;
		if (_hashes[_list[_mid]] < _key)
            _left = _mid+1;
        else
            _right = _mid;
	}
	return _left;
}

int main(int argc,char** argv) {
    string directname = "../data/";
    string outdirect = "../hash_data/";
    string name = argv[1];
    int N = stoi(argv[2]);
    bool R = stoi(argv[3]);
    outdirect += name + "/";
    string filename = directname + name + ".txt";
    vec* G = new vec[N];
    readGraph(filename, G, N);
    int* hashes = new int[N];
    fill_n(hashes, N, INT_MAX);
    int* degrees = new int[N];
    fill_n(degrees, N, 0);
    for (int i=0; i<N; i++) {
        if (G[i].size()==1) {
            degrees[G[i][0]] = degrees[G[i][0]] - 1;
            degrees[i] = 0;
        }
        else
            degrees[i] = degrees[i] + G[i].size();
    }
    int* except = new int[N];
    for (int i=0; i<N; i++) {
        if (degrees[i] <= 1)
            except[i] = 0;
        else {
            int CN = 0;
            for (int j=0; j!=G[i].size(); j++) {
                int n = G[i][j];
                int id2 = binarySearch(n, G[n], 0, G[n].size());
                commonNeighbor(G[i], G[n], CN, j+1, G[i].size(), id2, G[n].size());
            }
            except[i] = CN;
        }
    }
    if (R) {
        int* hashesMin = new int[N];
        int* hashesMax = new int[N];
        fill_n(hashesMin, N, INT_MAX);
        fill_n(hashesMax, N, INT_MIN);
        auto start_time = chrono::high_resolution_clock::now();
        hashMergeDiffNBEx(G, except, hashes, hashesMin, hashesMax, 0, N);
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> diff = end_time-start_time;
        cout << diff.count() << " s" << endl;
        delete[] hashesMin;
        delete[] hashesMax;
    }
    else {
        auto start_time = chrono::high_resolution_clock::now();
        hashMergeDiffIndexNBEx(G, except, hashes, 0, N);
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> diff = end_time-start_time;
        cout << diff.count() << " s" << endl;
    }
    int* ids = new int[N];
    sort_indexes(hashes, ids, N);
    string outname = name;
    if (R)
        outname += "_range_nb";
    else
        outname += "_index_nb";
    ofstream idfile;
    idfile.open(outdirect+outname+"/"+outname+"_id.csv");
    idfile << ":ID" << endl;
    for (int i=0; i<N; i++)
        idfile << ids[i] << endl;
    ofstream nodefile;
    nodefile.open(outdirect+outname+"/"+outname+"_node.csv");
    nodefile << ":ID,h:int" << endl;
    for (int i=0; i<N; i++)
        nodefile << i << "," << hashes[i] << endl;
    delete[] ids;
    delete[] hashes;
    delete[] except;
    delete[] G;
}