package drone;

public class DefaultDeliveryBehaviour implements DeliveryBehaviour {
    @Override
    public int getTransitDuration(int initialTransDuration) {
        return initialTransDuration; // Default: same duration for all
    }

    @Override
    public boolean canMove() {
        return true;
    }

    @Override
    public void onDelivered() {
        // No-op
    }
}