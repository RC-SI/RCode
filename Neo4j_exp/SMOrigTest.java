import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

import org.neo4j.dbms.api.DatabaseManagementService;
import org.neo4j.dbms.api.DatabaseManagementServiceBuilder;
import org.neo4j.graphdb.*;
import org.neo4j.kernel.internal.GraphDatabaseAPI;
import org.neo4j.internal.helpers.collection.Iterables;
import scala.Int;

import static org.neo4j.configuration.GraphDatabaseSettings.DEFAULT_DATABASE_NAME;

/**
 * Unit test for simple App.
 */
public class SMOrigTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final int maxi = Integer.MAX_VALUE;
    private String name;
    private String subname;
    private int N;
    private long[] ids;
    private int[] Vrank;
    private long[] nodes;
    private long maxId;
    private QG q;


    // tag::vars[]

    GraphDatabaseAPI graphDb;
    private DatabaseManagementService managementService;
    // end::vars[]

    // tag::shutdownHook[]
    private static void registerShutdownHook( final DatabaseManagementService managementService )
    {
        // Registers a shutdown hook for the Neo4j instance so that it
        // shuts down nicely when the VM exits (even if you "Ctrl-C" the
        // running application).
        Runtime.getRuntime().addShutdownHook( new Thread()
        {
            @Override
            public void run()
            {
                managementService.shutdown();
            }
        } );
    }
    // end::shutdownHook[]

    void shutdown() {
        this.managementService.shutdown();
    }

    SMOrigTest(String name, String index, int SN, String qfilename) throws IOException {
        File databaseDirectory;
        this.name = name;
        this.subname = name + "_" + index + "_nb_f";
        String final_file = directory + this.name + "/" + this.subname;
        databaseDirectory = new File(final_file);
        this.managementService = new DatabaseManagementServiceBuilder( databaseDirectory ).build();
        this.graphDb = (GraphDatabaseAPI) managementService.database(DEFAULT_DATABASE_NAME);
        registerShutdownHook( managementService );
        try ( Transaction tx = graphDb.beginTx() ) {
            this.N = (int) Iterables.count(tx.getAllNodes());
        }
        this.maxId = 0;
        this.ids = new long[N];
        try ( Transaction tx = graphDb.beginTx() ) {
            for (int i=0; i<N; i++) {
                Node node = tx.findNode(Label.label("N"), "UID", i);
                ids[i] = node.getId();
                if (ids[i]>this.maxId)
                    this.maxId = ids[i];
            }
        }
        this.nodes = new long[SN];
        for (int i=0; i<SN; i++)
            nodes[i] = ids[(N/SN)*i];
        q = new QG(qfilename);
    }
    
    public static void main( final String[] args ) throws IOException {
        SMOrigTest hello2 = new SMOrigTest(args[0], args[1], Integer.parseInt(args[2]), args[3]);
        long startTime2 = System.nanoTime();
        int t2 = hello2.subgraph_matching();
        long endTime2   = System.nanoTime();
        long totalTime2 = (endTime2 - startTime2)/1000000;
        System.out.println("Total SMs: " + t2 + ", time: " + totalTime2 + " ms");
        hello2.shutdown();
    }

    public int subgraph_matching() {
        List<List<Long> > res = new ArrayList<>();
        boolean[] visited = new boolean[q.vnum];
        List<Integer> join_order = new ArrayList<>();
        gen_join_order(q.first_vertex, q, visited, join_order);
        for (long i : nodes)
            res.add(new ArrayList<>(List.of(i)));
        for (int i = 1; i < q.vnum; ++i) {
            int sg_u = join_order.get(i);
            List<List<Long> > next_res = new ArrayList<>();
            List<Integer> cur_join_idx = new ArrayList<>();
            for (int j = 0; j < i; ++j) {
                int from = Math.min(join_order.get(j), sg_u);
                int to = Math.max(join_order.get(j), sg_u);
                if (q.getAdj(from).contains(to))
                    cur_join_idx.add(j);
            }
            Set<Long> temp_join_res = new HashSet<>();
            for (List<Long> rec : res) {
                long g_v = rec.get(cur_join_idx.get(0));
                try ( Transaction tx = this.graphDb.beginTx() ) {
                    Node node = tx.getNodeById(g_v);
                    for (Relationship rel : node.getRelationships(Direction.OUTGOING))
                        temp_join_res.add(rel.getEndNodeId());
                }
                for (int j = 1; j < cur_join_idx.size(); ++j) {
                    Set<Long> temp_join_res2 = new HashSet<>();
                    g_v = rec.get(cur_join_idx.get(j));
                    try ( Transaction tx = this.graphDb.beginTx() ) {
                        Node node = tx.getNodeById(g_v);
                        for (Relationship rel : node.getRelationships(Direction.OUTGOING)) {
                            long adj = rel.getEndNodeId();
                            if (temp_join_res.contains(adj))
                                temp_join_res2.add(adj);
                        }
                    }
                    temp_join_res = temp_join_res2;
                }
                for (long join_v : temp_join_res) {
                    Boolean is_legal = true;
                    for (long joined_u : rec) {
                        if (joined_u == join_v) {
                            is_legal = false;
                            break;
                        }
                    }
                    if (is_legal) {
                        List<Long> new_rec = new ArrayList<>(rec);
                        new_rec.add(join_v);
                        next_res.add(new_rec);
                    }
                }
            }
            // res = next_res;
            res = next_res;
        }        
        return res.size();
    }

    private void gen_join_order(int u, QG q_edge_list, boolean[] visited, List<Integer> join_order) {
        visited[u] = true;
        join_order.add(u);
        for (int v : q_edge_list.getAdj(u)) {
            if (!visited[v])
                gen_join_order(v, q_edge_list, visited, join_order);
        }
    }
}

class QG {
    private List<Set<Integer> > adjs;
    public int vnum;
    public int first_vertex;
    QG(String qfilename) throws IOException {
        try (BufferedReader br = new BufferedReader(new FileReader(qfilename))) {
            String line = br.readLine();
            vnum = Integer.parseInt(line);
            adjs = new ArrayList<>(vnum);
            for (int i=0; i<vnum; i++)
                adjs.add(new HashSet<Integer>());
            while ((line = br.readLine()) != null) {
                String[] hs = line.split(",");
                int from = Integer.parseInt(hs[0]);
                int to = Integer.parseInt(hs[1]);
                adjs.get(from).add(to);
                adjs.get(to).add(from);
            }
        }
        System.out.println(adjs);
        first_vertex = 0;
        int dg = this.getDegree(first_vertex);
        for (int i = 1; i < vnum; ++i) {        
            if (this.getDegree(i) < dg)
                first_vertex = i;
        }
    }

    public int getDegree(int n) {
        return adjs.get(n).size();
    }

    public Set<Integer> getAdj(int n) {
        return adjs.get(n);
    }
}
