package drone;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

class Location {
    public interface Id {
        Id makeId(int position);
        String toString();
        boolean deliveryAddress(int street, int house); // Does it match?
    }

    public enum DroneEvent {arrive, startDelivery, endDelivery, depart}
    public interface Observer {
        void notifyEvent(Id id, String s, DroneEvent e);
    }
    static final List<Observer> observers = new LinkedList<>();
    static void addObserver(Location.Observer observer) {
        observers.add(observer);
    }
    static private void changeEvent(Location.Id id, Drone d, DroneEvent droneEvent) {
        for (Location.Observer o: observers)
            o.notifyEvent(id, d.toString(), droneEvent); // Notify SuburbView and logging
    }

    final Id id;
    final Map<Suburb.Direction, Location> roads = new HashMap<>();
    Drone drone = null;
    boolean delivering = false;

    Location(Id id) {
        this.id = id;
    }

    public String toString() {
        return id.toString();
    }

    void setRoad(Suburb.Direction direction, Location location) {
        assert !(roads.containsKey(direction)) :
                "Attempt to overwrite " + this + " road " + direction;  // Road reconstruction not supported
        roads.put(direction, location);
    }

    Location getRoad(Suburb.Direction direction) {
        return roads.get(direction);
    }

    public Location end(Suburb.Direction direction) {
        Location road = this;
        while (road.getRoad(direction) != null) {
            // System.out.println(road.id);
            road = road.getRoad(direction);
        }
        return road;
    }

    public void arriveDrone(Drone d) {
        assert drone == null : "Second drone arrives at " + this; // Safety violation on assertion failure!
        drone = d;
        d.setLocation(this);
        changeEvent(id, drone, DroneEvent.arrive);
    }

    public void departDrone() {
        assert drone != null : "No drone to depart " + this; // Must be a drone here
        assert !delivering : "Attempt to depart while delivering " + this;  // Must not be delivering
        changeEvent(id, drone, DroneEvent.depart);
        drone.setLocation(null);
        drone = null;
    }

    public void departDrone(Suburb.Direction direction) {
        Drone drone = this.drone;
        departDrone();
        // System.out.println(roads);
        // System.out.println(direction + " -> " + roads.containsKey(direction));
        assert roads.containsKey(direction) :   // Must be a road in that direction
                "Attempting to depart " + this + " towards " + direction;
        // System.out.println(direction + " -> " + getRoad(direction));
        getRoad(direction).arriveDrone(drone);
    }

    public void startDelivery() {
        assert id.getClass() == Suburb.StreetId.class : "Attempt to deliver in non-street " + this;
        assert this.drone != null : "Attempt to deliver without drone " + this;
        assert !delivering : "Attempt to deliver when already delivering " + this;
        delivering = true;
        changeEvent(id, drone, DroneEvent.startDelivery);
    }

    public void endDelivery() {
        assert id.getClass() == Suburb.StreetId.class : "Attempt to end delivery in non-street " + this;
        assert this.drone != null : "Attempt to end delivery without drone " + this;
        assert delivering : "Attempt to end delivery when not delivering at " + this;
        delivering = false;
        changeEvent(id, drone, DroneEvent.endDelivery);
    }
}