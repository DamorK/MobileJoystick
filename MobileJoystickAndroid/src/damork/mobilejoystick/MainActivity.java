package damork.mobilejoystick;

import damork.mobilejoystick.logic.BluetoothListener;
import damork.mobilejoystick.logic.BluetoothServer;
import damork.mobilejoystick.logic.Const;
import damork.mobilejoystick.logic.JoystickButtons;
import damork.mobilejoystick.logic.JoystickPosition;
import damork.mobilejoystick.logic.JoystickReport;
import damork.mobilejoystick.logic.OrientationListener;
import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Rect;
import android.hardware.SensorManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ImageView.ScaleType;
import android.widget.LinearLayout;

public class MainActivity extends Activity implements BluetoothListener
{
	private Handler handler = new Handler();
	private volatile boolean active;
	
	private ImageView[] buttons;
	private JoystickPosition joyPosition;
	private JoystickButtons joyButtons;
	private JoystickReport joyReport;
	private SensorManager sensorManager;
	private OrientationListener listener;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
    }

	@Override
	protected void onStart() {
		SharedPreferences p = getSharedPreferences("damork.mobilejoystick.cfg", MODE_PRIVATE);
		
		float xcenter = p.getFloat("XCENTER", Const.XCENTER);
		float ycenter = p.getFloat("YCENTER", Const.YCENTER);
		int newNumOfButtons = p.getInt("BUTTONS", Const.BUTTONS);
		
		joyPosition = new JoystickPosition(xcenter, ycenter);
		joyButtons = new JoystickButtons();
		joyReport = new JoystickReport(
				joyPosition,
				joyButtons,				
				p.getFloat("XMIN", Const.XMIN),
				p.getFloat("XMAX", Const.XMAX),
				xcenter,
				p.getFloat("YMIN", Const.YMIN),
				p.getFloat("YMAX", Const.YMAX),
				ycenter);
		listener = new OrientationListener(joyPosition);
		
		if (buttons == null || buttons.length != newNumOfButtons)
		{
	        setContentView(R.layout.activity_main);
	        initButtons(newNumOfButtons);
		}
		
		super.onStart();
	}
	
	@Override
	protected void onResume()
	{
		active = true;
		listener.register(sensorManager);
		
		if (BluetoothServer.getInstance().connected())
		{
			Toast.makeText(this, R.string.toast_connected, Toast.LENGTH_SHORT).show();
			BluetoothServer.getInstance().startIO(this, joyReport);
		}
		else
			Toast.makeText(this, R.string.toast_unconnected, Toast.LENGTH_SHORT).show();
			
		super.onResume();
	}
	
    @Override
	protected void onPause() {
    	active = false;
    	BluetoothServer.getInstance().stopIO();
		listener.unregister(sensorManager);
		super.onPause();
	}

	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		switch (item.getItemId())
		{
		case R.id.menu_connect:
			startActivity(new Intent(this, ConnectActivity.class));
			return true;
		case R.id.menu_settings:
			startActivity(new Intent(this, SettingsActivity.class));
			return true;
		default:
			return super.onMenuItemSelected(featureId, item);
		}
	}
	
	// ======================================================
	// GUI methods
	// ======================================================
	
	private class ButtonTouchListener implements OnTouchListener
	{		
		public boolean onTouch(View v, MotionEvent event)
		{
			Rect rect = new Rect();
			int pointerToIgnore = -1;
			byte state = 0;
			
			switch (event.getActionMasked())
			{
			case MotionEvent.ACTION_POINTER_UP:
			case MotionEvent.ACTION_UP:
				pointerToIgnore = event.getActionIndex();
			case MotionEvent.ACTION_DOWN:
			case MotionEvent.ACTION_POINTER_DOWN:
			case MotionEvent.ACTION_MOVE:
				for (int btnNo = 0; btnNo < buttons.length; btnNo++)
				{
					ImageView btn = buttons[btnNo];
					btn.setAlpha(0x30);
					
					if (!btn.getGlobalVisibleRect(rect))
						continue;
					for (int i = 0; i < event.getPointerCount(); i++)
					{
						if (i == pointerToIgnore)
							continue;
						if (rect.contains((int) event.getX(i), (int) event.getY(i)))
						{
							state |= (1 << btnNo);
							btn.setAlpha(0x80);
							break;
						}
					}
				}

				joyButtons.setButtons(state);
				return true;
			}
			return false;			
		}
		
		/* DEBUG
		private String getDesc(int i)
		{
			switch (i) {
				case MotionEvent.ACTION_POINTER_UP: return "PUP";
				case MotionEvent.ACTION_UP: return "UP";
				case MotionEvent.ACTION_DOWN: return "DOWN";
				case MotionEvent.ACTION_POINTER_DOWN: return "PDOWN";
				case MotionEvent.ACTION_MOVE: return "MOVE";
				case MotionEvent.ACTION_CANCEL: return "CANCEL";
			}
			return "OTH" + i;
		}*/		
	}
	private void initButtons(int numOfButtons)
	{
		LinearLayout btnContainer = (LinearLayout) findViewById(R.id.layoutButtons);
		LinearLayout col1 = (LinearLayout) findViewById(R.id.layoutCol1);
		LinearLayout col2 = (LinearLayout) findViewById(R.id.layoutCol2);
		LinearLayout col3 = (LinearLayout) findViewById(R.id.layoutCol3);
		
		if (col1 == null || col2 == null || col3 == null)
			return;
		if (numOfButtons < 2 || numOfButtons > 9)
			return;
		
		buttons = new ImageView[numOfButtons];
		
		if (numOfButtons <= 6)
		{
			int leftCnt = numOfButtons - (numOfButtons / 2);
			initButtonColumn(col1, 0, leftCnt - 1);
			initButtonColumn(col3, leftCnt, numOfButtons - 1);
		}
		else
		{
			int cntrCnt = numOfButtons - 6;
			int rghtBeg = 3 + cntrCnt;
			initButtonColumn(col1, 0, 2);
			initButtonColumn(col2, 3, rghtBeg - 1);
			initButtonColumn(col3, rghtBeg, numOfButtons - 1);
		}
		
		btnContainer.setOnTouchListener(new ButtonTouchListener());
	}
	
	
	
	private void initButtonColumn(LinearLayout lay, int buttonMin, int buttonMax)
	{
		int[] colours = getResources().getIntArray(R.array.btn_colours);
		
		for (int btnNo = buttonMin; btnNo <= buttonMax; btnNo++)
		{			
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
					LayoutParams.FILL_PARENT, 
					LayoutParams.WRAP_CONTENT);
			lp.weight = 1;

			ImageView btn = new ImageView(this);
			btn.setLayoutParams(lp);
			btn.setImageResource(R.drawable.btn);
			btn.setScaleType(ScaleType.FIT_XY);
			btn.setAlpha(0x40);
			btn.setColorFilter(colours[btnNo]);
			lay.addView(btn);
			buttons[btnNo] = btn;
		}		
	}

	// ======================================================
	// Bluetooth listener
	// ======================================================

	public void connectionAccepted(BluetoothSocket socket)	{}
	public void listeningFailed() {}
	
	public void connectionInterrupted()
	{
		if (active)
		{
			handler.post(new Runnable()
			{
				public void run() 
				{
					Toast.makeText(
							MainActivity.this, 
							R.string.toast_interrupted, 
							Toast.LENGTH_SHORT).show();
				}
			});
		}
	}
}
