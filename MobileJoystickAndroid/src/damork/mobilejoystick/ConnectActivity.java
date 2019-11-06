package damork.mobilejoystick;

import damork.mobilejoystick.logic.BluetoothListener;
import damork.mobilejoystick.logic.BluetoothServer;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.app.Activity;
import android.bluetooth.BluetoothSocket;

public class ConnectActivity extends Activity implements BluetoothListener
{
	private Handler handler = new Handler();
	private volatile boolean active;
	private volatile boolean visible;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect);
    }

	@Override
	protected void onStart() {
		visible = true;
		if (BluetoothServer.getInstance().connected())
			setGUI(GUIState.CONNECTED);
		else
			setGUI(GUIState.START);
		super.onStart();
	}

	@Override
	protected void onStop() {
		visible = false;
		BluetoothServer.getInstance().stopListening();
		super.onStop();
	}

	@Override
	protected void onResume() {
		active = true;
		super.onResume();
	}
	
	@Override
	protected void onPause() {
		active = false;
		super.onPause();
	}
	
	// ===============================================
	// buttons callbacks
	// ===============================================
	
	public void connectClicked(View v)
	{
		setGUI(GUIState.LISTENING);
		
		if (!BluetoothServer.getInstance().startListening(this))
		{
			setGUI(GUIState.START);
			showToast(R.string.text_connect_failed);
		}
	}
	
	public void disconnectClicked(View v)
	{
		BluetoothServer.getInstance().disconnect();
		setGUI(GUIState.START);
	}

	// ===============================================
	// bluetooth Listener
	// ===============================================

	public void connectionAccepted(BluetoothSocket socket)
	{
		handler.post(new Runnable() 
		{
			public void run() 
			{
				setGUI(GUIState.CONNECTED);
			}
		});
	}
	
	public void listeningFailed()
	{
		handler.post(new Runnable() 
		{
			public void run() 
			{
				setGUI(GUIState.START);
				showToast(R.string.text_connect_failed);
			}
		});
	}
	
	public void connectionInterrupted()
	{}
	

	// ===============================================
	// gui methods
	// ===============================================
	
	private static enum GUIState
	{
		START,
		LISTENING,
		CONNECTED
	}
	
	private void setGUI(GUIState state)
	{
		if (visible)
		{
			TextView txt = (TextView) findViewById(R.id.textConnect);
			Button btnConnect = (Button) findViewById(R.id.btnConnect);
			Button btnDisconnect = (Button) findViewById(R.id.btnDisconnect);
			
			switch (state)
			{
			case START:
				txt.setText(R.string.text_connect_start);
				btnConnect.setEnabled(true);
				btnDisconnect.setEnabled(false);
				break;
			case LISTENING:
				txt.setText(R.string.text_connect_listening);
				btnConnect.setEnabled(false);
				btnDisconnect.setEnabled(false);
				break;
			case CONNECTED:
				txt.setText(R.string.text_connect_ok);
				btnConnect.setEnabled(false);
				btnDisconnect.setEnabled(true);
				break;
			}
		}
	}
	
	private void showToast(int strId)
	{
		if (active)
		{
			Toast toast = Toast.makeText(this, strId, Toast.LENGTH_SHORT);
			toast.show();
		}
	}
}
