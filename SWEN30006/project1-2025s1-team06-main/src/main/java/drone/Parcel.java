package drone;

class Parcel implements Comparable<Parcel> {
    final int street;
    final int house;
    final int arrival; // Time of arrival at DispatchCentre
    final int weight;
    final boolean isFragile;

    @Override public int compareTo(Parcel i) {
        int streetDiff = this.street - i.street;  // Don't really need this as only deliver to one street at a time
        return (streetDiff == 0) ? this.house - i.house : streetDiff;
    }

    Parcel(int street, int house, int arrival, int weight, boolean isFragile) {
        this.street = street;
        this.house = house;
        this.arrival = arrival;
        this.weight = weight;
        this.isFragile = isFragile;
    }

    public String toString() {
        return "Parcel from time " + arrival +
                " addressed to " + house + " " + Suburb.StreetName.values[street-1] +
                " Street; Weight: " + myWeight() + " Fragile?: " + isFragile;
    }

    int myStreet() { return street; }
    int myHouse() { return house; }
    int myArrival() { return arrival; }
    int myWeight() {return weight; }
    boolean isFragile() { return isFragile; }

}
