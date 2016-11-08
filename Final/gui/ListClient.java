import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class ListClient {

    Socket serverSocket;
    String serverHostName = "192.168.1.1";
    int serverPortNumber = 42880;
    ServerListener sl;

    ListClient() {
        // 1. CONNECT TO THE SERVER
        try {
            serverSocket = new Socket(serverHostName, serverPortNumber);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        // 2. SPAWN A LISTENER FOR THE SERVER. THIS WILL KEEP RUNNING
        // when a message is received, an appropriate method is called
        sl = new ServerListener(this, serverSocket);
        new Thread(sl).start();

        PrintWriter out;
        try {
            out = new PrintWriter(new BufferedOutputStream(serverSocket.getOutputStream()));

            // 3. SEND THREE WISHES TO SERVER
            out.println("wish 1:  one million bucks ");
            out.flush(); // force the output
            out.println("wish 2: uh oh! ");
            out.flush(); // force the output
            out.println("wish 3: get rid of the bucks ");
            out.flush(); // force the output

        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }



    }

    public void handleMessage(String s) {
        System.out.println(s);
    }

    public static void main(String[] args) {
        ListClient lc = new ListClient();
    } // end of main method

} // end of ListClient

class ServerListener implements Runnable {
    ListClient lc;
    Scanner in; // this is used to read which is a blocking call

    ServerListener(ListClient lc, Socket s) {
        try {
            this.lc = lc;
            in = new Scanner(new BufferedInputStream(s.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        while (true) { // run forever
            System.out.println("Client - waiting to read");
            String s = in.nextLine();
            lc.handleMessage(s);
        }

    }
}
