#include "indexGraph.h"

int main(int argc,char** argv) {
    string directname = "../data/";
    string outdirect = "../hash_data/";
    string groDirect = "../gro_data/";
    string name = argv[1];
    int N = stoi(argv[2]);
    vec* G = new vec[N];
    utils::readGraph(directname+name+".txt", G, N);
    Graph OG(G, N);
    double OGT = OG.calTri();
    {
        int* hashes = new int[N];
        iota(hashes, hashes+N, 0);
        hashGraph HG(G, hashes, N);
        double HGT = HG.calTri();
        cout << "Orig Order Rate: " << OGT/HGT << endl;
        delete[] hashes;
    }
    {
        groDirect += name;
        stringvec groFiles;
        utils::read_directory(groDirect, groFiles);
        for (const string& f:groFiles) {
            hashGraph HG(G, N, f);
            double GROT = HG.calTri();
            cout << f << " Rate: " << OGT/GROT << endl;
        }
    }
    {
        string hdirect = outdirect + name + "/" + name + "_range_nb/" + name + "_range_nb_";
        string hnode = hdirect + "node.csv";
        string hid = hdirect + "id.csv";
        cout << hnode << endl;
        cout << hid << endl;
        hashGraph HG(G, N, hnode, hid);
        double HGT = HG.calTri();
        cout << "HG Rate: " << OGT/HGT << endl;
    }
    {
        string idirect = outdirect + name + "/" + name + "_index_nb/" + name + "_index_nb_";
        string inode = idirect + "node.csv";
        string iid = idirect + "id.csv";
        cout << inode << endl;
        cout << iid << endl;
        indexGraph IG(G, N, inode, iid);
        double IGT = IG.calTri();
        cout << "IG Rate: " << OGT/IGT << endl;
    }
    delete[] G;
}