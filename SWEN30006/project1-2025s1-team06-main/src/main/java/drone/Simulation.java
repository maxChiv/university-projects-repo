package drone;

// Check that maxweight (of parcel) is less than or equal to the maxcapacity of drone.

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.concurrent.TimeUnit;

public class Simulation {
    static Logger logger;
    interface Tickable {
        void tick();
    }
    static final List<Tickable> entities = new ArrayList<>();
    static void register(Tickable entity) { entities.add(entity); }

    static final Map<Integer, List<Parcel>> waitingToArrive = new HashMap<>();
    static int time = 0;
    final int endArrival;
    final DispatchCentre dispatchCentre;
    static int timeout;

    static int deliveredCount = 0;
    static int deliveredTotalTime = 0;

    public static void deliver(Parcel parcel) {
        String s = "Delivered: " + parcel;
        System.out.println(s);
        logger.logEvent("%5d: %s\n", Simulation.now(), s);;
        deliveredCount++;
        deliveredTotalTime += now() - parcel.myArrival();
    }

    void addToArrivals(int arrivalTime, Parcel item) {
        // System.out.println(item.toString());
        if (waitingToArrive.containsKey(arrivalTime)) {
            waitingToArrive.get(arrivalTime).add(item);
        } else {
            LinkedList<Parcel> items = new LinkedList<>();
            items.add(item);
            waitingToArrive.put(arrivalTime, items);
        }
    }

    public Simulation(Properties properties) {
        int        seed = Integer.parseInt(properties.getProperty("seed", "30006"));
        this.endArrival = Integer.parseInt(properties.getProperty("parcel.endarrival", "5"));
        int  numParcels = Integer.parseInt(properties.getProperty("parcel.parcels", "4"));
        int   maxWeight = Integer.parseInt(properties.getProperty("parcel.maxweight", "15"));
    int weightThreshold = Integer.parseInt(properties.getProperty("parcel.weightthreshold", "12"));
        int  fragile1in = Integer.parseInt(properties.getProperty("parcel.fragile1in", "1000"));
       int timeToSuburb = Integer.parseInt(properties.getProperty("parcel.timetosuburb", "4"));
        int  numStreets = Integer.parseInt(properties.getProperty("suburb.streets", "2"));
        int   numHouses = Integer.parseInt(properties.getProperty("suburb.housesperstreet", "3"));
        int   numdrones = Integer.parseInt(properties.getProperty("drone.number", "1"));
                timeout = Integer.parseInt(properties.getProperty("timeout", "600"));
                          String logfile = properties.getProperty("logfile", "logfile.txt");

        logger = new Logger(logfile);
        LocalDateTime myDateObj = LocalDateTime.now();
        DateTimeFormatter myFormatObj = DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm:ss");
        String formattedDate = myDateObj.format(myFormatObj);
        logger.logEvent("%5d: Simulation parameters - time=%s  seed=%d\n", now(), formattedDate, seed);
        Random random = new Random(seed);

        Suburb suburb = new Suburb(numStreets, numHouses);
        new SuburbView(numStreets, numHouses); // Registers itself as a Location observer

        dispatchCentre = new DispatchCentre(suburb, timeToSuburb, numdrones, weightThreshold);
        for (int i = 0; i < numParcels; i++) { // Generate parcels
            int arrivalTime = random.nextInt(endArrival)+1;
            int street = random.nextInt(suburb.NUMSTREETS)+1;
            int house = random.nextInt(suburb.NUMHOUSES)+1;
            int weight = random.nextInt(maxWeight)+1;
            boolean fragile = random.nextInt(fragile1in) == 0;
            addToArrivals(arrivalTime, new Parcel(street, house, arrivalTime, weight, fragile));
        }
    }

    public static int now() { return time; }

    void step() {
        // External events
        if (waitingToArrive.containsKey(time))
            dispatchCentre.arrive(waitingToArrive.get(time));
        // Internal events
        for (Tickable entity: entities) {
            entity.tick();
        }
    }

    public void run() {
        while (time++ <= endArrival || dispatchCentre.someItems() || !dispatchCentre.allDronesBack()) {
            step();
            try {
                TimeUnit.MILLISECONDS.sleep(timeout);
            } catch (InterruptedException e) {
                // System.out.printf("Sleep interrupted!\n");
            }
        }
        logger.logEvent("%5d Finished: Items delivered = %d; Average time for delivery = %.2f%n",
                now(), deliveredCount, (float) deliveredTotalTime/deliveredCount);
    }

}
