package drone;

public class FragileDeliveryBehaviour extends DefaultDeliveryBehaviour {
    private final Drone drone;
    private final int TICKS_PER_MOVE = 2;
    private int currentTicksUntilMove = TICKS_PER_MOVE;

    public FragileDeliveryBehaviour(Drone drone) {
        this.drone = drone;
    }

    @Override
    public boolean canMove() {
        // Incremenet the tick down needed before moving.
        currentTicksUntilMove--;
        if (currentTicksUntilMove == 0) {
            currentTicksUntilMove = TICKS_PER_MOVE;
            return true;
        } else {
            return false;
        }

    }

    @Override
    public void onDelivered() {
        // Revert to default behaviour once parcel is delivered
        drone.setDeliveryBehaviour(new DefaultDeliveryBehaviour());
    }
}