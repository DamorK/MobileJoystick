package damork.mobilejoystick.logic;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class OrientationListener implements SensorEventListener
{
	private static final float[] FILTERING_FACTORS = {1.0f, 1.0f, 0.75f, 0.5f};
	private float[] accelerometerValues;
	private GravityFilter gravityFilter = new GravityFilter(FILTERING_FACTORS);
	private JoystickPosition joyPosition;
	
	/*
	 * pos - where to store orientation
	 */
	public OrientationListener(JoystickPosition pos)
	{
		this.joyPosition = pos;
	}
	
	public void clear()
	{
		accelerometerValues = null;
		gravityFilter = new GravityFilter(FILTERING_FACTORS);
	}
	
	public void register(SensorManager sm)
	{
		sm.registerListener(this,
				sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
				SensorManager.SENSOR_DELAY_FASTEST);
		clear();
	}
	
	public void unregister(SensorManager sm)
	{
		sm.unregisterListener(this);
	}

	public void onSensorChanged(SensorEvent event) 
	{
		accelerometerValues = gravityFilter.filter(event.values.clone());
		
		Utils.normalize(accelerometerValues);
		float yAngle = (float) Math.toDegrees(Math.asin(accelerometerValues[0]));
		float xAngle = (float) Math.toDegrees(Math.asin(accelerometerValues[1]));
		if (accelerometerValues[2] < 0.0f)
		{
			float f = yAngle > 0.0f ? 180.0f : -180.0f;
			yAngle = f - yAngle;
		}
		
		joyPosition.set(xAngle, yAngle);
	}
	
	public void onAccuracyChanged(Sensor sensor, int accuracy) 
	{}

}
