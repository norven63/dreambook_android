package com.retech.richbook;

import android.app.Application;
import android.util.DisplayMetrics;

import com.retech.richbook.reader.BookReader;

public class MyApplication extends Application {
	@Override
	public void onCreate() {
		super.onCreate();

		int screenWidth = getResources().getDisplayMetrics().widthPixels;
		int screenHeight = getResources().getDisplayMetrics().heightPixels;

		DisplayMetrics dm = new DisplayMetrics();
		dm = getResources().getDisplayMetrics();
		int screenDensityDPI = dm.densityDpi;

		BookReader.getInstance.initProperty(screenWidth, screenHeight, screenDensityDPI);
	}
}
