import java.io.*;
import java.net.*;
import java.util.*;

public class Main {
    public static void main(String[] args) throws Exception {
        try (ServerSocket server = new ServerSocket(8080)) {
            for (;;) {
                Socket socket = server.accept();
                System.out.println("Connected");
                // ServerThread serverThread = new ServerThread(socket);
                // Thread thread = new Thread(serverThread);
                // thread.start();
            }
        }
    }
}