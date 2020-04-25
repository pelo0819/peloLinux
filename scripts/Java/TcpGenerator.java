import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;


public class TcpGenerator{
    public static void main(String[] args){
        try
        {
            Socket socket=new Socket(args[0], 8080);
            OutputStream out=socket.getOutputStream();

            String sendData="this is Test.";

            out.write(sendData.getBytes("UTF-8"));

            System.out.println("Send Data : "+sendData);

            out.close();

            socket.close();

        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }
}