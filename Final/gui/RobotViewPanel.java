import javax.swing.*;
import java.awt.Color;
import java.awt.Graphics;
import java.util.ArrayList;

/**
 * JPanel to show the objects on the screen
 */
public class RobotViewPanel extends JPanel {

    private ArrayList<FoundObject> objects;
    private Graphics g;
    String message;

    public RobotViewPanel(ArrayList<FoundObject> objects) {
        setBorder(BorderFactory.createLineBorder(Color.black));
        this.objects = objects;
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        this.g = g;

        renderMessage();
        renderRings();
        renderObjects();
    }

    /**
     * Rerenders the Panel with an updated list of FoundObjects
     *
     * @param newObjects
     */
    public void repaint(ArrayList<FoundObject> newObjects) {
        objects = newObjects;
        super.repaint();
        revalidate();
    }

    /**
     * Add String s to the Panel
     * @param s
     */
    public void addText(String s) {
        message = s;
        super.repaint();
        revalidate();
    }

    /**
     * Remove all currently printed text
     */
    public void removeText() {
        message = null;
        super.repaint();
        revalidate();
    }

    /**
     * Render the message to the GUI
     */
    private void renderMessage() {
        if(message != null) {
            g.drawString(message, 10, 10);
        }
        else {
            g.setColor(new Color(238, 238, 238));
            g.fillRect(0,0,150,30);
        }
    }

    /**
     * Render the rings to define the map lines
     */
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

    /**
     * Render the FoundObjects
     */
    private void renderObjects() {
        for(FoundObject object: objects) {
            renderObject(object);
        }
    }

    /**
     * Render one object
     *
     * @param object
     */
    private void renderObject(FoundObject object) {
        int[] coords = polarToRect(object.distance, object.location);

        int diameter = (int)((object.width*5)/2.0 + 25);
        int x = (int)(coords[0] - diameter/2.0);
        int y = (int)(coords[1] - diameter/2.0);

        g.drawOval(x, y, diameter, diameter);
    }

    /**
     * Converts polar coords into rectangular coords
     *
     * @param r
     * @param theta
     * @return int[] coordinates
     */
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

    /**
     * Convert angle in degrees to radians
     *
     * @param degrees
     * @return
     */
    private double degreesToRadians(int degrees) {
        return degrees*(Math.PI/180);
    }
}
