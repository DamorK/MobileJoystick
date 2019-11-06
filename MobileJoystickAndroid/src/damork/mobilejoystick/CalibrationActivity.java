package damork.mobilejoystick;

import java.util.Timer;
import java.util.TimerTask;

import damork.mobilejoystick.logic.Const;
import damork.mobilejoystick.logic.JoystickPosition;
import damork.mobilejoystick.logic.OrientationListener;

import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.app.Activity;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class CalibrationActivity extends Activity 
{
	private static enum State
	{
		CALIB_START,
		CALIB_SET_CENTER,
		CALIB_SET_BOUNDARIES
	}
	
	private State state = State.CALIB_START;
	
	private JoystickPosition joyCurrentPos = new JoystickPosition(Const.XCENTER, Const.XCENTER);
	private JoystickPosition joyCenter = new JoystickPosition(Const.XCENTER, Const.XCENTER);
	private JoystickPosition joyMin = new JoystickPosition(Const.XCENTER, Const.XCENTER);
	private JoystickPosition joyMax = new JoystickPosition(Const.XCENTER, Const.XCENTER);
	
	private Handler handler = new Handler();
	private Timer timer = new Timer();
	private SensorManager sensorManager;	
	private OrientationListener listener = new OrientationListener(joyCurrentPos);
	
	class UpdatePositionViews implements Runnable
	{
		private JoystickPosition p;
		public UpdatePositionViews(JoystickPosition p) { this.p = p; }
		public void run() 
		{
			TextView txtX = (TextView) findViewById(R.id.textX);
			TextView txtY = (TextView) findViewById(R.id.textY);
			Button btn = (Button) findViewById(R.id.buttonCalib);
			txtX.setText(String.format("X: %d", (int)p.x()) );
			txtY.setText(String.format("Y: %d", (int)p.y()) );
			btn.setEnabled(true);
		}		
	}
	
	class UpdateJoyPositions extends TimerTask
	{
		@Override
		public void run() 
		{
			JoystickPosition p = joyCurrentPos.clone();
			if (state.equals(State.CALIB_SET_CENTER))
			{
				joyCenter = p.clone();
				joyMin = p.clone();
				joyMax = p.clone();
			}
			else if (state.equals(State.CALIB_SET_BOUNDARIES))
			{
				joyMin.min(p);
				joyMax.max(p);
			}
			handler.post(new UpdatePositionViews(p));
		}		
	}

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calibration);
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
    }

	@Override
	protected void onResume() {
		if (!state.equals(State.CALIB_START))
		{
			startOrientationRetrieving();
		}
		super.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		stopOrientationRetrieving();
	}

	public void calibButtonClicked(View view) {
		TextView txt = (TextView) findViewById(R.id.textCalib);
		Button btn = (Button) findViewById(R.id.buttonCalib);
		
		switch (state)
		{
		case CALIB_START:
			txt.setText(R.string.text_calib_centralpos);
			btn.setText(R.string.btn_next);
			btn.setEnabled(false);
			state = State.CALIB_SET_CENTER;
			startOrientationRetrieving();
			break;
		case CALIB_SET_CENTER:
			txt.setText(R.string.text_calib_boundaries);	
			state = State.CALIB_SET_BOUNDARIES;
			break;
		case CALIB_SET_BOUNDARIES:
			txt.setText(R.string.text_calib_finished);
			btn.setText(R.string.btn_calibrate);
			state = State.CALIB_START;
			stopOrientationRetrieving();
			saveCalibrationResults();
			break;
		}
	}    
	
	private void startOrientationRetrieving()
	{		
		if (state.equals(State.CALIB_SET_CENTER))
		{
			joyCurrentPos.set(Const.XCENTER, Const.YCENTER);
		}
		
		listener.register(sensorManager);
		timer.schedule(new UpdateJoyPositions(), 500, 100);
	}
	
	private void stopOrientationRetrieving()
	{
		TextView txtX = (TextView) findViewById(R.id.textX);
		TextView txtY = (TextView) findViewById(R.id.textY);
		txtX.setText("");
		txtY.setText("");
		
		listener.unregister(sensorManager);
		timer.cancel();
		timer.purge();
		timer = new Timer();
	}
	
	private void saveCalibrationResults()
	{
		SharedPreferences p = getSharedPreferences("damork.mobilejoystick.cfg", MODE_PRIVATE);
		Editor ed = p.edit();
		synchronized (joyMin)
		{
			ed.putFloat("XMIN", joyMin.x());
			ed.putFloat("YMIN", joyMin.y());
		}
		synchronized (joyMax)
		{
			ed.putFloat("XMAX", joyMax.x());
			ed.putFloat("YMAX", joyMax.y());
		}
		synchronized (joyCenter)
		{
			ed.putFloat("XCENTER", joyCenter.x());
			ed.putFloat("YCENTER", joyCenter.y());			
		}
		ed.commit();		
	}
}
