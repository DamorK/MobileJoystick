package damork.mobilejoystick;

import java.util.Arrays;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

public class SettingsActivity extends Activity {
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        initListeners();
    }

    private void initListeners() 
    {
        ListView lvSettings = (ListView) findViewById(R.id.listViewSettings);
        if (lvSettings == null)
        	return;

    	lvSettings.setOnItemClickListener(new AdapterView.OnItemClickListener() 
        {
            public void onItemClick(AdapterView<?> parent, View view, int position, long i) 
            {
            	if (position == 0)
				{
					SettingsActivity.this.selectNumberOfButtons();
				}
				else if (position == 1)
				{
					SettingsActivity.this.goToCalibration();
				}        
            }
        });
    }
    
    private void selectNumberOfButtons()
    {
    	int buttons = preferences().getInt("BUTTONS", 2);
    	int sel = Arrays.binarySearch(
    			getResources().getIntArray(R.array.items_numofbuttons_int),
    			buttons);
    	
    	AlertDialog.Builder ad = new AlertDialog.Builder(this);
    	ad.setTitle(R.string.title_numofbuttons);
    	ad.setSingleChoiceItems(R.array.items_numofbuttons, sel, new DialogInterface.OnClickListener() 
    	{			
			public void onClick(DialogInterface dialog, int which) 
			{
				int buttons = getResources().getIntArray(R.array.items_numofbuttons_int)[which];
				Editor edit = preferences().edit();
				edit.putInt("BUTTONS", buttons);
				edit.commit();
				dialog.dismiss();
			}
		});
    	ad.create();
    	ad.show();
    }
    
    private void goToCalibration()
    {
    	startActivity(new Intent(this, CalibrationActivity.class));
    }
    
    private SharedPreferences preferences()
    {
    	return getSharedPreferences("damork.mobilejoystick.cfg", MODE_PRIVATE);
    }
}
