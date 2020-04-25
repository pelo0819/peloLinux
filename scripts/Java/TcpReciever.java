import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.*;
import java.util.Arrays;

public class TcpReciever{
    public static void main(String[] args){
        try{
            ServerSocket svSock=new ServerSocket(8080);
            System.out.println("waiting connect....");
            Socket sock=svSock.accept();

            byte[] data=new byte[1024];
            InputStream in=sock.getInputStream();

            int readSize=in.read(data);
            data=Arrays.copyOf(data, readSize);

            System.out.println("receive content : " + new String(data, "UTF-8"));

            // in.close();
            
            svSock.close();

        }catch(IOException e){
            e.printStackTrace();
        }
    }

}