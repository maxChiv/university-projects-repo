package drone;

class DefaultPriorityEvaluator implements PriorityEvaluator {
    // Get the priority of the drone by return a higher number for drones with higher priority
    public int getPriority(Drone drone) {
        if (drone.parcel == null) return 0;
        if (drone.parcel.isFragile()) return 1;
        if (drone.isCarryingHeavy()) return 2;
        if (drone.isApproachingFromSouth()) return 3;
        return 4;
    }
}
