import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class ListClient {

    Socket serverSocket;
    String serverHostName = "192.168.1.1";
    int serverPortNumber = 42880;
    ServerListener sl;
    public ArrayList<FoundObject> objects = new ArrayList<>();

    ListClient() {
        try {
            serverSocket = new Socket(serverHostName, serverPortNumber);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        sl = new ServerListener(this, serverSocket);
        new Thread(sl).start();
    }

    public int sendMessage(String s) {
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
        System.out.println(s);
        if(s.contains("WIDTH")) {
            handleObjectFound(s);
        }
    }
    
    public void handleObjectFound(String s) {
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
