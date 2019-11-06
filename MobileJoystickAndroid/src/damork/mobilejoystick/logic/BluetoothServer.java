package damork.mobilejoystick.logic;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;

public class BluetoothServer 
{
	public static enum State
	{
		DISCONNECTED,
		LISTENING,
		CONNECTED
	}
	
	private static BluetoothServer instance = new BluetoothServer();
	private BluetoothServerSocket serverSocket;
	private BluetoothSocket socket;
	private IOTask ioTask;
	
	public static BluetoothServer getInstance()
	{
		return instance;
	}
	
	// ==================================================================
	// listening
	// ==================================================================	
	
	public synchronized boolean startListening(BluetoothListener listener)
	{
		if (serverSocket != null || socket != null)
			return false;
		
		try
		{
			BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
			serverSocket = adapter.listenUsingInsecureRfcommWithServiceRecord(
					Const.BTNAME,
					Const.BTUUID);
			
			new AcceptConnection(serverSocket, listener).start();			
			return true;
		}
		catch (IOException e)
		{
			return false;
		}		
	}
	
	public synchronized void stopListening()
	{
		if (serverSocket != null)
		{
			try
			{
				serverSocket.close();
			}
			catch (IOException e) {}
			serverSocket = null;
		}
	}
	
	// ==================================================================
	// connection
	// ==================================================================
	
	private synchronized void assignSocket(BluetoothSocket s)
	{
		socket = s;
	}
	
	public synchronized boolean connected()
	{
		return socket != null;
	}
	
	public synchronized void disconnect()
	{
		if (socket != null)
		{
			try
			{
				socket.close();
			}
			catch (IOException e) {}
			socket = null;
			ioTask = null;
		}
	}
	
	public synchronized void startIO(BluetoothListener l, JoystickReport jr)
	{
		ioTask = new IOTask(socket, l, jr);
		ioTask.start();		
	}
	
	public synchronized void stopIO()
	{
		if (ioTask != null)
		{
			ioTask.cancelTask();
			ioTask = null;
		}
	}
	
	// ==========================================

	private class AcceptConnection extends Thread
	{
		private BluetoothServerSocket btServerSocket;
		private BluetoothListener listener;
		
		public AcceptConnection(BluetoothServerSocket s, BluetoothListener l)
		{
			btServerSocket = s;
			listener = l;
		}
		
		@Override
		public void run()
		{
			BluetoothSocket s = accept();	
			stopListening();
			assignSocket(s);
			
			if (s != null)	listener.connectionAccepted(s);
			else			listener.listeningFailed();
		}
		
		private BluetoothSocket accept()
		{
			try {
				return btServerSocket.accept();
			}	catch (IOException e) {}
			return null;
		}
	}
	
	// ==========================================
	
	private class IOTask extends Thread
	{
		private static final byte MSG_SET_REPORT = 1;
		private static final byte MSG_GET_REPORT = 2;
		
		private BluetoothSocket btSocket;
		private BluetoothListener listener;
		private JoystickReport joyReport;
		private volatile boolean canceled;
		
		public IOTask(BluetoothSocket s, BluetoothListener l, JoystickReport jr)
		{
			btSocket = s;
			listener = l;
			joyReport = jr;
		}
		
		public void cancelTask()	
		{
			canceled = true;
		}
		
		@Override 
		public void run()
		{
			InputStream istream = null;
			OutputStream ostream = null;
			
			try
			{
				istream = btSocket.getInputStream();
				ostream = btSocket.getOutputStream();
				
				byte[] inMsg = new byte[1];
				byte[] outMsg = new byte[4];
				
				while (!canceled)
				{
					istream.read(inMsg);
					
					if (inMsg[0] == MSG_GET_REPORT)
					{
						byte[] jr = joyReport.prepareReport();
						outMsg[0] = MSG_SET_REPORT;
						outMsg[1] = jr[0];
						outMsg[2] = jr[1];
						outMsg[3] = jr[2];
						ostream.write(outMsg);
						ostream.flush();
					}
				}
			}
			catch (IOException e)
			{
				disconnect();
				listener.connectionInterrupted();
				return;
			}
			
						
		}		
	}

}
