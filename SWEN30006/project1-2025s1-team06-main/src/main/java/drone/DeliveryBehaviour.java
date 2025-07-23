package drone;

public interface DeliveryBehaviour {
    int getTransitDuration(int initialTransDuration);
    boolean canMove();
    void onDelivered(); // Called when parcel is delivered
}
