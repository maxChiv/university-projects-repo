package drone;
import java.util.*;

class DispatchCentre implements Simulation.Tickable {
    final int numdrones;
    public final int timeToSuburb;
    public final int weightThreshold;
    final Queue<Parcel> waitingForDelivery;
    final Set<Drone> drones;
    final Map<Location,List<Drone>> requests = new LinkedHashMap<>();
    PriorityEvaluator priorityEvaluator;

    public boolean someItems() {
        return !waitingForDelivery.isEmpty();
    }

    DispatchCentre(Suburb suburb, int timeToSuburb, int numdrones, int weightThreshold) {
        this.timeToSuburb = timeToSuburb; // Distance away suburb is from dispatch centre
        this.weightThreshold = weightThreshold;
        this.priorityEvaluator = new DefaultPriorityEvaluator();
        this.numdrones = numdrones;
        waitingForDelivery = new LinkedList<>();
        drones = new HashSet<>();
        for (int i = 0; i < numdrones; i++) drones.add(new Drone(this, suburb));
        Simulation.register(this);

        // Add "backwards paths" ordering of locations to request map
        for (Location l = suburb.backAvenue; l != null; l = l.getRoad(Suburb.Direction.SOUTH)) {
            requests.put(l, new ArrayList<>());
        }
        for (int i = 0; i < suburb.NUMSTREETS; i++) {
            Location e = suburb.streets[i];
            for (int j = 0; j < suburb.NUMHOUSES; j++) e = e.getRoad(Suburb.Direction.EAST);
            for (int j = 0; j < suburb.NUMHOUSES; j++) {
                e = e.getRoad(Suburb.Direction.WEST);
                requests.put(e, new ArrayList<>());
            }
        }
        Location e = suburb.outAvenue;
        while (e.getRoad(Suburb.Direction.SOUTH) != null) e = e.getRoad(Suburb.Direction.SOUTH);
        for (Location l = e; l != null; l = l.getRoad(Suburb.Direction.NORTH)) {
            requests.put(l, new ArrayList<>());
        }
//         System.out.println(requests.keySet());
    }

    void arrive(List<Parcel> parcels) {
        for (Parcel parcel : parcels) {
            waitingForDelivery.add(parcel);
            String s = "Arrived: " + parcel;
            System.out.println(s);
            Simulation.logger.logEvent("%5d: %s\n", Simulation.now(), s);        }
    }

    boolean isHeavy(Parcel parcel) {
        return parcel.myWeight() > weightThreshold;
    }

    public void requestDispatch(Drone drone) {
        if (!waitingForDelivery.isEmpty()) {
            drone.dispatch(waitingForDelivery.remove());
            drones.remove(drone);
        } else {
            drones.add(drone);  // Track waiting drones
        }
    }

    public boolean allDronesBack() { return drones.size() == numdrones; }

    // This method gets called at each tick to grant access to the drone with the highest priority
    // defined in the priority evaluator. It checks the location cells in opposite order of the flow of traffic
    void processingRequest() {
        for (Location location : requests.keySet()) {
            List<Drone> listOfDrones = requests.get(location);
            if (listOfDrones.isEmpty()) continue;

            Drone highestDronePriority = null;

            // Get the highest priority drone for the location
            for (Drone d : listOfDrones) {
                if (highestDronePriority == null || priorityEvaluator.getPriority(highestDronePriority) < priorityEvaluator.getPriority(d)) {
                    highestDronePriority = d;
                }
            }

            // Move the highest priority drone and remove it from the list of drones requesting to move location
            if (highestDronePriority != null && location.drone == null) {
                highestDronePriority.grantAccess(location);
                requests.get(location).remove(highestDronePriority);
            }
        }
    }

//  Simple access accepting any request for a free location
    void requestAccess(Drone drone, Location location) {
        if (!requests.get(location).contains(drone)) {
            requests.get(location).add(drone);
        }
    }

    // This is 1 time frame unit in the context of the program and will move the drones at this point
    public void tick() {
        processingRequest();
    }
}
