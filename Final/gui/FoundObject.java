public class FoundObject {
    public int distance;
    public int width;
    public int location;

    public FoundObject(int width,int location,int distance) {
        this.width = width;
        this.distance = distance;
        this.location = location;
    }

    public String toString() {
        return "Dist: " + distance + "Width: " + width + "Location: " + location;
    }
}
