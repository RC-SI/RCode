import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

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
public class MCTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final RelationshipType rtn = RelationshipType.withName(String.valueOf(Integer.MAX_VALUE));
    private String name;
    private String subname;
    private int N;
    private long[] ids;
    private int[] Vrank;
    private long[] nodes;
    private long maxId;

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

    MCTest(String name, String index, int SN) throws IOException {
        File databaseDirectory;
        this.name = name;
        this.subname = name + "_" + index + "_nb";
        String final_file = directory + this.name + "/" + this.subname + "_f";
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
            nodes[i] = ids[(N/2/SN)*i];
        this.Vrank = new int[(int) this.maxId+1];
        try (BufferedReader br = new BufferedReader(new FileReader(rdirectory + this.name + "/" + this.subname + "_Vrank.csv"))) {
            int idx = 0;
            String line;
            while ((line = br.readLine()) != null)
                Vrank[(int) ids[idx++]] = Integer.parseInt(line);
        }
        System.out.println("Read mM Done");
    }
    
    public static void main( final String[] args ) throws IOException {
        MCTest hello2 = new MCTest(args[0], args[1], Integer.parseInt(args[2]));
        long startTime2 = System.nanoTime();
        long t2 = hello2.MCSelect();
        long endTime2   = System.nanoTime();
        long totalTime2 = (endTime2 - startTime2)/1000000;
        System.out.println("Total MCs: " + t2 + ", time: " + totalTime2 + " ms");
        hello2.shutdown();
    }

    long selectPivot(Set<Long> P, Set<Long> X, RelationshipType[] PRTS) {
        long u = P.iterator().next();
        int cn = 0;
        for (long p : P) {
            int currCN = 0;
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node = tx.getNodeById(p);
                for (Relationship rel : node.getRelationships(Direction.OUTGOING, PRTS))
                    currCN += P.contains(rel.getEndNodeId())?1:0;                    
            }   
            if (currCN > cn) {
                u = p;
                cn = currCN;
            }
        }
        for (long p : X) {
            int currCN = 0;
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node = tx.getNodeById(p);
                for (Relationship rel : node.getRelationships(Direction.OUTGOING, PRTS))
                    currCN += P.contains(rel.getEndNodeId())?1:0;                    
            }   
            if (currCN > cn) {
                u = p;
                cn = currCN;
            }
        }
        return u;
    }

    long BKP(Set<Long> P, Set<Long> X, RelationshipType[] PH, RelationshipType[] PXH) {
        long result = 0;
        if (P.isEmpty()) {
            if (X.isEmpty())
                return 1;
            return 0;
        }
        long u = selectPivot(P, X, PH);
        Set<Long> V = new HashSet<>(P);
        try ( Transaction tx = this.graphDb.beginTx() ) {
            Node node = tx.getNodeById(u);
            for (Relationship rel : node.getRelationships(Direction.OUTGOING, PH))
                V.remove(rel.getEndNodeId());
        }
        for (long v:V) {
            Set<Long> NP = new HashSet<>();
            Set<String> NPH = new HashSet<>();
            Set<Long> NX = new HashSet<>();
            Set<String> NPXH = new HashSet<>();
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node = tx.getNodeById(v);
                for (Relationship rel : node.getRelationships(Direction.OUTGOING, PXH)) {
                    long adj = rel.getEndNodeId();
                    String t = rel.getType().name();
                    if (P.contains(adj)) {
                        NP.add(adj);
                        NPH.add(t);
                        NPXH.add(t);
                    }
                    else if (X.contains(adj)) {
                        NX.add(adj);
                        NPXH.add(t);
                    }
                }
            }
            List<RelationshipType> NPHL= NPH.stream().map(s -> RelationshipType.withName(s)).collect(Collectors.toList());
            List<RelationshipType> NPXHL= NPXH.stream().map(s -> RelationshipType.withName(s)).collect(Collectors.toList());
            result = result + BKP(NP, NX, NPHL.toArray(new RelationshipType[NPHL.size()]), NPXHL.toArray(new RelationshipType[NPXHL.size()]));
            P.remove(v);
            X.add(v);
        }
        return result;
    }

    long MCSelect() {
        long result = 0;
        for (long n:nodes) {
            int i = Vrank[(int) n];
            Set<Long> P = new HashSet<>();
            Set<String> PH = new HashSet<>();
            Set<Long> X = new HashSet<>();
            Set<String> PXH = new HashSet<>();
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node = tx.getNodeById(n);
                for (Relationship rel : node.getRelationships(Direction.OUTGOING)) {
                    if (!rel.isType(rtn)) {
                        long adj = rel.getEndNodeId();
                        String t = rel.getType().name();
                        PXH.add(t);
                        if (Vrank[(int) adj] > i) {
                            P.add(adj);
                            PH.add(t);
                        }
                        else
                            X.add(adj);
                    }
                    else if (Vrank[(int) rel.getEndNodeId()] > i)
                        result += 1;
                }
            }
            if (!P.isEmpty() || !X.isEmpty()) {
                List<RelationshipType> PHL= PH.stream().map(s -> RelationshipType.withName(s)).collect(Collectors.toList());
                List<RelationshipType> PXHL= PXH.stream().map(s -> RelationshipType.withName(s)).collect(Collectors.toList());
                result = result + BKP(P, X, PHL.toArray(new RelationshipType[PHL.size()]), PXHL.toArray(new RelationshipType[PXHL.size()]));
            }
        }
        return result;
    }
}
