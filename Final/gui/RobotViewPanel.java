import javax.swing.*;
import java.awt.Color;
import java.awt.Graphics;
import java.util.ArrayList;

public class RobotViewPanel extends JPanel {

    private ArrayList<FoundObject> objects;
    private Graphics g;

    public RobotViewPanel(ArrayList<FoundObject> objects) {
        setBorder(BorderFactory.createLineBorder(Color.black));
        this.objects = objects;
    }

    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        this.g = g;

        renderRings();
        renderObjects();
    }

    public void repaint(ArrayList<FoundObject> newObjects) {
        objects = newObjects;
        super.repaint();
    }

    private void renderRings() {
        g.setColor (Color.blue);
        g.drawOval (200, 150, 100, 100);
        g.drawOval (175, 125, 150, 150);
        g.drawOval (150, 100, 200, 200);
        g.drawOval (125, 75, 250, 250);
        g.drawOval (100, 50, 300, 300);
        g.drawOval (75, 25, 350, 350);
        g.drawOval (50, 0, 400, 400);
    }

    private void renderObjects() {
        for(FoundObject object: objects) {
            renderObject(object);
        }
    }

    private void renderObject(FoundObject object) {
        int[] coords = polarToRect(object.distance, object.location);

        int diameter = (int)((object.width*5)/2.0 + 25);
        int x = (int)(coords[0] - diameter/2.0);
        int y = (int)(coords[1] - diameter/2.0);

        g.drawOval(x, y, diameter, diameter);
    }

    private int[] polarToRect(double r, int theta) {
        int[] coords = new int[2];
        r = (r*5)/2.0 + 25;
        double thetaRadians = degreesToRadians(theta);
        double x = r*Math.cos(thetaRadians);
        double y = r*Math.sin(thetaRadians);

        x = x+250;
        y= 200-y;

        coords[0] = (int)x;
        coords[1] = (int)y;

        return coords;
    }



    private double degreesToRadians(int degrees) {
        return degrees*(Math.PI/180);
    }
}
