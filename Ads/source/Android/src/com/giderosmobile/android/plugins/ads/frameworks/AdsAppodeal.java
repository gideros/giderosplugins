package com.giderosmobile.android.plugins.ads.frameworks;

import java.lang.ref.WeakReference;

import android.app.Activity;
import android.util.Log;
import android.util.SparseArray;
import android.view.KeyEvent;

import com.appodeal.ads.Appodeal;
import com.appodeal.ads.AppodealCallbacks;
import com.giderosmobile.android.plugins.ads.*;

public class AdsAppodeal implements AdsInterface{
	
	private WeakReference<Activity> sActivity;
	private String adsID;
	private AdsManager mngr;
	static AdsAppodeal me;
	
	public void onCreate(WeakReference<Activity> activity)
	{
		me = this;
		sActivity = activity;
		mngr = new AdsManager();
	}
	
	//on destroy event
	public void onDestroy()
	{	
		mngr.destroy();
	}
	
	public void onStart(){}

	public void onStop(){}
	
	public void onPause(){}
		
	public void onResume(){}
	
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		return false;
	}
	
	public void setKey(final Object parameters){
		SparseArray<String> param = (SparseArray<String>)parameters;
		adsID = param.get(0);
		final String TAG = "Appodeal";
		Appodeal.initialize(sActivity.get(), adsID, new AppodealCallbacks(){
			 @Override
             public void onAdLoaded() {
                 Log.d(TAG, "onAdLoaded");
             }

             @Override
             public void onAdFailedToLoad() {
            	 Log.d(TAG, "onAdFailedToLoad");
             }

             @Override
             public void onAdShown() {
            	 Log.d(TAG, "onAdShown");
             }

             @Override
             public void onAdClicked() {
            	 Log.d(TAG, "onAdClicked");
             }

             @Override
             public void onAdClosed() {
            	 Log.d(TAG, "onAdClosed");
             }	
		});
	}
	
	public void loadAd(final Object parameters){}
	
	public void showAd(final Object parameters)
	{
		if (Appodeal.isLoaded())
		Appodeal.showBanner(sActivity.get());
	}
	
	public void hideAd(String type)
	{
		mngr.hide(type);
	}
	
	public int getWidth(){
		return 0;
	}
	
	public int getHeight(){
		return 0;
	}

	@Override
	public void enableTesting() {}
	
}
