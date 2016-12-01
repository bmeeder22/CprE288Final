import java.io.*;
import java.net.Socket;
import java.util.ArrayList;

public class ListClient {

    Socket serverSocket;
    String serverHostName = "192.168.1.1";
    int serverPortNumber = 42880;
    ServerListener serverListener;
    private ArrayList<FoundObject> objects = new ArrayList<>();

    private RobotViewPanel robotViewPanel;
    boolean error;

    ListClient(RobotViewPanel robotViewPanel) {
        try {
            serverSocket = new Socket(serverHostName, serverPortNumber);
        } catch (Exception e) {
            e.printStackTrace();
        }

        error = false;
        serverListener = new ServerListener(this, serverSocket);
        this.robotViewPanel = robotViewPanel;
        new Thread(serverListener).start();
    }

    public ArrayList<FoundObject> getObjects() {
        return objects;
    }

    public int sendMessage(String s) {
        if(s.startsWith("f") || s.startsWith("b") || s.startsWith("l") || s.startsWith("r")) {
            error = false;
            robotViewPanel.addText("Moving...");
        }

        PrintWriter out=null;
        try {
            out = new PrintWriter(new BufferedOutputStream(serverSocket.getOutputStream()));
        } catch (IOException e) {
            e.printStackTrace();
            return 1;
        }

        out.print(s);
        out.flush();
        return 0;
    }

    public void handleMessage(String s) {
        s = s.replace(new StringBuilder().append((char)0), "");

        System.out.println(s);
        if(s.contains("WIDTH")) {
            handleObjectFound(s);
        }
        else if(s.equals("start_sweep")) {
            objects.clear();
            robotViewPanel.addText("Sweeping...");
        }
        else if(s.equals("end_sweep")) {
            sendSweepData();
            robotViewPanel.removeText();
        }
        else if(s.contains("CLIFF")) {
            robotViewPanel.addText("Cliff found!");
            error = true;
        }
        else if(s.contains("OBSTACLE")) {
            robotViewPanel.addText("Obstacle found!");
            error = true;
        }
        else if(s.contains("LINELEFT")) {
            robotViewPanel.addText("Line found! Left of robot");
            error = true;
        }
        else if(s.contains("LINERIGHT")) {
            robotViewPanel.addText("Line found! Right of robot");
            error = true;
        }
        else if(s.contains("EndMove") && !error) {
            robotViewPanel.removeText();
        }
    }

    private void sendSweepData() {
        robotViewPanel.repaint(objects);
        robotViewPanel.revalidate();
    }
    
    private void handleObjectFound(String s) {
        String[] object = s.split(";");
        int width = Integer.parseInt(object[0].split(":")[1]);
        int location = Integer.parseInt(object[1].split(":")[1]);
        int distance = Integer.parseInt(object[2].split(":")[1]);
        objects.add(new FoundObject(width,location,distance));
    }
}

class ServerListener implements Runnable {
    ListClient lc;
    BufferedReader in;

    ServerListener(ListClient lc, Socket s) {
        try {
            this.lc = lc;
            in = new BufferedReader(new InputStreamReader(s.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        System.out.println("Client - waiting to read");
        while (true) {
            String s = null;
            try {
                s = in.readLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            lc.handleMessage(s);
        }
    }
}
