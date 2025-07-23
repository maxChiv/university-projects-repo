package drone;

class Drone implements Simulation.Tickable {
    private DeliveryBehaviour behaviour;

    static int count = 1;
    final String id;
    final DispatchCentre dispatchCentre;
    final Suburb suburb;
    Location location;
    Parcel parcel = null;
    enum State {
        WaitingForDispatch,TransitToSuburb, TransitToDelivery, Delivering, TransitToExit, TransitToCentre, Recharge}
    State state = State.WaitingForDispatch;
    int transDuration; // Elapsed time before next transition occurs

    Drone(DispatchCentre dispatchCentre, Suburb suburb) {
        this.dispatchCentre = dispatchCentre;
        this.suburb = suburb;
        this.id = "D" + count++;
        location = null;
        Simulation.register(this);
    }

    public void setDeliveryBehaviour(DeliveryBehaviour behaviour) {
        this.behaviour = behaviour;
    }

    // Check if the drone is carrying a heavy parcel
    public boolean isCarryingHeavy() {
        if (this.parcel == null) return false;
        return this.dispatchCentre.isHeavy(this.parcel);
    }

    // Check if the drone is approaching the next location from the south
    public boolean isApproachingFromSouth() {
        Location entry = suburb.getEntry();
        // Find entry point south of entry
        Location fromSouth = entry.getRoad(Suburb.Direction.SOUTH);
        // On the space south of entry, meaning we are approaching from the south
        return fromSouth != null && this.location == fromSouth;
    }

    public void tick() {
        Location nextLocation;
        switch (state) {
            case WaitingForDispatch:
                dispatchCentre.requestDispatch(this);
                break;
            case TransitToSuburb:
                if (transDuration > 0) {
                    if (behaviour.canMove()) {
                        // Can move so decrease its transition duration.
                        transDuration--;
                        return;
                    }
                } else {
                    dispatchCentre.requestAccess(this, suburb.getEntry());
                }
                break;
            case TransitToDelivery:
                if (!behaviour.canMove()) return;

                // Counterclockwise: south to parcel street then east to parcel house
                nextLocation = location.getRoad(Suburb.Direction.EAST);
                // East - looking for delivery location - must be a street
                if (nextLocation == null || !((Suburb.StreetId) nextLocation.id).sameStreet(parcel.myStreet())) {
                    // not currently next to parcel address street
                    nextLocation = location.getRoad(Suburb.Direction.SOUTH);
                }
                assert nextLocation != null :
                    "Reached " + location.id + " without finding street:" + parcel.myStreet();
                dispatchCentre.requestAccess(this, nextLocation);
                break;
            case Delivering:
                if (parcel == null) {
                    location.endDelivery();
                    behaviour.onDelivered(); // Hook to allow behaviour change
                    state = State.TransitToExit;

                } else {
                    location.startDelivery();
                    Simulation.deliver(parcel);
                    parcel = null;
                }
                break;
            case TransitToExit:
                if (!behaviour.canMove()) return;

                // Counterclockwise: east to Back Ave then north to exit
                nextLocation = location.getRoad(Suburb.Direction.EAST);
                if (nextLocation == null) {
                    nextLocation = location.getRoad(Suburb.Direction.NORTH);
                }
                assert nextLocation != null : "Can't go east or north from " + location.id;
                dispatchCentre.requestAccess(this, nextLocation);
                break;
            case TransitToCentre:
                if (location != null) location.departDrone();
                if (transDuration > 0) {
                    if (behaviour.canMove()) {
                        // Can move so decrease its transition duration.
                        transDuration--;
                        return;
                    }
                } else {
                        state = State.Recharge;
                }
                break;
            case Recharge:
                state = State.WaitingForDispatch;
                break;
        }
    }

    void dispatch(Parcel parcel) {
        assert state == State.WaitingForDispatch : id + " dispatched when not waiting for dispatch";
        this.parcel = parcel;

        // Assign the behaviour
        if (parcel.isFragile()) {
            this.behaviour = new FragileDeliveryBehaviour(this);
        } else {
            this.behaviour = new DefaultDeliveryBehaviour();
        }

        transDuration = behaviour.getTransitDuration(dispatchCentre.timeToSuburb);
        state = State.TransitToSuburb;
    }

    void grantAccess(Location location) {
        switch (state) {
            case TransitToSuburb:
                if (this.location != null) this.location.departDrone();
                location.arriveDrone(this);
                state = State.TransitToDelivery;
                break;
            case TransitToDelivery:
                if (this.location != null) this.location.departDrone();
                location.arriveDrone(this);
                if (location.id.deliveryAddress(parcel.myStreet(), parcel.myHouse())) {
                    state = State.Delivering;
                }
                break;
            case TransitToExit:
                if (this.location != null) this.location.departDrone();
                location.arriveDrone(this);
                if (location == suburb.getExit()) {
                    state = State.TransitToCentre;
                    transDuration = dispatchCentre.timeToSuburb;
                }
                break;
            default:
                assert false : id + " access granted to " + location.id + " in non-requesting state " + state;
        }
    }

    public String toString() { return id; }

    void setLocation(Location location) { this.location = location; }

    Location getLocation() { return location; }

    public void add(Parcel item) {
        parcel = item;
    }

}