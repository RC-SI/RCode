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
public class TriOrigTest {
    private static final String directory = "/home/zhengweiguo/yfyang/hash_merge/databases/";
    private static final String rdirectory = "/home/zhengweiguo/yfyang/hash_merge/final_neo4j/";
    private static final String confFile = "./conf";
    private static final int maxi = Integer.MAX_VALUE;
    private String name;
    private String subname;
    private int N;
    private long[] ids;

    // tag::vars[]

    GraphDatabaseAPI graphDb;
    private DatabaseManagementService managementService;
    // end::vars[]

    void setUpDb(String name, String index) throws IOException {
        File databaseDirectory;
        this.name = name;
        this.subname = name + "_" + index + "_nb";
        String final_file = directory + this.name + "/" + this.subname;
        databaseDirectory = new File(final_file);
        this.managementService = new DatabaseManagementServiceBuilder( databaseDirectory ).build();
        this.graphDb = (GraphDatabaseAPI) managementService.database(DEFAULT_DATABASE_NAME);
        registerShutdownHook( managementService );
        try ( Transaction tx = graphDb.beginTx() ) {
            this.N = (int) Iterables.count(tx.getAllNodes());
        }
        this.ids = new long[N];
        try ( Transaction tx = graphDb.beginTx() ) {
            for (int i=0; i<N; i++) {
                Node node = tx.findNode(Label.label("N"), "UID", i);
                ids[i] = node.getId();
            }
        }
    }

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

    TriOrigTest(String name, String index) throws IOException {
        this.setUpDb(name, index);
    }
    
    public static void main( final String[] args ) throws IOException {
        TriOrigTest hello2 = new TriOrigTest(args[0], args[1]);
        long startTime2 = System.nanoTime();
        long t2 = hello2.TriExactCounting();
        long endTime2   = System.nanoTime();
        long totalTime2 = (endTime2 - startTime2)/1000000;
        System.out.println("Total CNs: " + t2 + ", time: " + totalTime2 + " ms");
        hello2.shutdown();
    }

    long TriExactCounting() {
        long result = 0;
        try ( Transaction tx = this.graphDb.beginTx() ) {
            for (int i=0; i<this.N; i++) {
                Node node1 = tx.getNodeById(this.ids[i]);
                Set<Long> adjs = new HashSet<>();
                for (Relationship rel : node1.getRelationships(Direction.OUTGOING)) {
                    long adjid = rel.getEndNodeId();
                    if (adjid > this.ids[i])
                        adjs.add(adjid);
                }
                for (long adj : adjs) {
                    Node node2 = tx.getNodeById(adj);
                    for (Relationship rel : node2.getRelationships(Direction.OUTGOING)) {
                        long adjid = rel.getEndNodeId();
                        result += (adjid > adj && adjs.contains(rel.getEndNodeId()))?1:0;
                    }
                }
            }
        }
        return result;
    }

    long TriCounting() {
        long result = 0;
        for (int i=0; i<this.N; i++) {
            try ( Transaction tx = this.graphDb.beginTx() ) {
                Node node1 = tx.getNodeById(this.ids[i]);
                Set<Long> adjs = new HashSet<>();
                for (Relationship rel : node1.getRelationships(Direction.OUTGOING))
                    adjs.add(rel.getEndNodeId());
                for (long adj : adjs) {
                    Node node2 = tx.getNodeById(adj);
                    for (Relationship rel : node2.getRelationships(Direction.OUTGOING))
                        result += adjs.contains(rel.getEndNodeId())?1:0;
                }
            }
        }
        return result;
    }
}
