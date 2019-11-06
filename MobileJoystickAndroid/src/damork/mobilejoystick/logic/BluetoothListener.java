package damork.mobilejoystick.logic;

import android.bluetooth.BluetoothSocket;

public interface BluetoothListener 
{
	public void connectionAccepted(BluetoothSocket socket);
	public void listeningFailed();
	public void connectionInterrupted();
}
