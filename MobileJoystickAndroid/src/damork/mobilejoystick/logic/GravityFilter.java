package damork.mobilejoystick.logic;

import java.util.LinkedList;

public class GravityFilter 
{
	private LinkedList<float[]> values = new LinkedList<float[]>();
	private float[] factors;
	private int components;
	
	
	public GravityFilter(float[] factors)
	{
		this.factors = factors;
	}
	
	public float[] filter(float[] val)
	{
		if (values.size() == 0)
			components = val.length;
		
		if (val.length == components)
		{
			if (values.size() >= factors.length)
				values.removeLast();
			values.addFirst(val);
		}
		
		return calcFilteredValue();
	}
	
	private float[] calcFilteredValue()
	{
		float[] result = new float[components];
		float 	sum = 0.0f;		
		int 	valueIndex = 0;
		
		for (float[] v : values)
		{
			float factor = factors[valueIndex];
			for (int i = 0; i < components; i++)
			{
				result[i] += v[i] * factor;
			}
			sum += factor;
			valueIndex++;
		}

		divide(result, sum);		
		return result;
	}
	
	private void divide(float[] v, float d)
	{
		float inv = 1.0f / d;
		for (int i = 0; i < v.length; i++)
			v[i] *= inv;
	}
}
