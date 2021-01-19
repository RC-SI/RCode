#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <climits>
#include <assert.h>
#include <chrono>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <numeric>
#include <set>
#include <array>
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>
#include "../include/tqdm.h"

#define CNV 1
#define CNNBV 1

using namespace std;

typedef vector<int> vec;
typedef vector<pair<int, int> > pvec;
typedef vector<string> stringvec;

namespace utils {
    template <typename T>
    inline void sort_indexes(T*& v, T*& result, int*& ids, int N) {
        int* idx = new int[N];
        iota(idx, idx+N, 0);
        stable_sort(idx, idx+N, [v](int i1, int i2) {return v[i1] < v[i2];});

        for (int i=0; i<N; i++) {
            ids[idx[i]] = i;
            result[i] = v[idx[i]];
        }
        delete[] idx;
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

    inline int binarySearch(const int _key, const vec _list, const int* _hashes) {
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

    inline void commonNeighbor(const vec& vec1, const vec& vec2, unsigned long& cn) {
        int id1 = 0;
        int ed1 = vec1.size();
        int id2 = 0;
        int ed2 = vec2.size();
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

    inline void commonNeighborBS(const vec& vec1, const vec& vec2, unsigned long& cn, int id1, int ed1, int id2, int ed2) {
        while (id1 != ed1) {
            id2 = binarySearch(vec1[id1], vec2, id2, ed2);
            if (id2==ed2)
                return;
            if (vec1[id1]==vec2[id2])
                ++cn;
            ++id1;
        }
    }

    inline void commonNeighbor(const vec& vec1, const vec& vec2, unsigned long& cn, int id1, int ed1, int id2, int ed2) {
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

    inline void commonNeighbor(const vec& vec1, const vec& vec2, vec& cn) {
        int id1 = 0;
        int ed1 = vec1.size();
        int id2 = 0;
        int ed2 = vec2.size();
        while (id1 != ed1 && id2 != ed2) {
            if (vec1[id1]==vec2[id2]) {
                cn.push_back(vec1[id1]);
                id1++;
                id2++;
            }
            else if (vec1[id1]<vec2[id2])
                id1++;
            else
                id2++;
        }
    }

    inline void commonNeighbor(const vec& vec1, const vec& vec2, vec& cn, int id1, int ed1, int id2, int ed2) {
        while (id1 != ed1 && id2 != ed2) {
            if (vec1[id1]==vec2[id2]) {
                cn.push_back(vec1[id1]);
                id1++;
                id2++;
            }
            else if (vec1[id1]<vec2[id2])
                id1++;
            else
                id2++;
        }
    }

    inline void setMinus(const vec& vec1, const vec& vec2, vec& cn) {
        int id1 = 0;
        int ed1 = vec1.size();
        int id2 = 0;
        int ed2 = vec2.size();
        while (id1 != ed1) {
            if (id2 == ed2 || vec1[id1] < vec2[id2]) {
                cn.push_back(vec1[id1]);
                id1++;
            }
            else if (vec1[id1] == vec2[id2]) {
                id1++;
                id2++;
            }
            else
                id2++;
        }
    }

    static void readGraph(const string file, vec* G, const int N) {
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

    static void readCSV(string file, int* hashes, int* hashesMin, int* hashesMax) {
        ifstream hash_File(file.c_str());
        string currLine;
        getline(hash_File, currLine);
        while(getline(hash_File, currLine)) {
            stringstream linestream(currLine);
            string ids;
            getline(linestream,ids,',');
            int idx = stoi(ids);
            string hs;
            getline(linestream,hs,',');
            string ms;
            getline(linestream,ms,',');
            string Ms;
            getline(linestream,Ms,',');
            hashes[idx] = stoi(hs);
            hashesMin[idx] = stoi(ms);
            hashesMax[idx] = stoi(Ms);
        }
    }

    static void readCSV(const string file, int*& hashes, bool withHeader=true, char delim=',') {
        ifstream hash_File(file.c_str());
        string currLine;
        if (withHeader)
            getline(hash_File, currLine);
        while(getline(hash_File, currLine)) {
            stringstream linestream(currLine);
            string ids;
            getline(linestream,ids,delim);
            int idx = stoi(ids);
            string hs;
            getline(linestream,hs,delim);
            hashes[idx] = stoi(hs);
        }
    }

    static void readId(const string file, int*& ids) {
        ifstream hash_File(file.c_str());
        string currLine;
        getline(hash_File, currLine);
        int i = 0;
        while(getline(hash_File, currLine)) {
            stringstream linestream(currLine);
            string id;
            getline(linestream,id,',');
            ids[i++] = stoi(id);
        }
    }

    static void readPair(const string file, pvec& pairs) {
        ifstream hash_File(file.c_str());
        string currLine;
        while(getline(hash_File, currLine)) {
            stringstream linestream(currLine);
            string ids1;
            getline(linestream,ids1,' ');
            int id1 = stoi(ids1);
            string ids2;
            getline(linestream,ids2,' ');
            int id2 = stoi(ids2);
            pairs.push_back(make_pair(id1, id2));
        }
    }

    inline bool search(vec& v, int h) {
        if (v.empty())
            return false;
        vec::iterator lb = lower_bound(v.begin(), v.end(), h);
        return (lb != v.end() && *lb == h);
    }

    static void read_directory(const string& name, stringvec& v) {
        boost::filesystem::path p(name);
        boost::filesystem::directory_iterator start(p);
        boost::filesystem::directory_iterator end;
        while (start != end) {
            if (!boost::filesystem::is_directory((*start).path()))
                v.push_back((*start).path().string());
            start++;
        }
    }
}

#endif