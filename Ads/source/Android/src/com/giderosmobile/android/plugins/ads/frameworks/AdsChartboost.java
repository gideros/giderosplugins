package com.giderosmobile.android.plugins.ads.frameworks;

import java.lang.ref.WeakReference;
import android.app.Activity;
import android.util.SparseArray;
import android.view.KeyEvent;

import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.Chartboost.CBAgeGateConfirmation;
import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Model.CBError.CBClickError;
import com.chartboost.sdk.Model.CBError.CBImpressionError;
import com.giderosmobile.android.plugins.ads.*;

public class AdsChartboost implements AdsInterface, ChartboostDelegate{
	
	private WeakReference<Activity> sActivity;
	private String adsID;
	private AdsManager mngr;
	private Chartboost cb;
	static AdsChartboost me;
	
	public void onCreate(WeakReference<Activity> activity)
	{
		me = this;
		sActivity = activity;
		mngr = new AdsManager();
		cb = Chartboost.sharedChartboost();
	}
	
	//on destroy event
	public void onDestroy()
	{	
		this.cb.onDestroy(sActivity.get());
	}
	
	public void onStart()
	{
		this.cb.onStart(sActivity.get());
		this.cb.startSession();
	}

	public void onStop()
	{
		this.cb.onStop(sActivity.get());
	}
	
	public void onPause(){}
		
	public void onResume(){}
	
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (this.cb.onBackPressed())
			// If a Chartboost view exists, close it and return
			return true;
		else
			// If no Chartboost view exists, continue on as normal
			return false;
	}
	
	public void setKey(final Object parameters){
		SparseArray<String> param = (SparseArray<String>)parameters;
		adsID = param.get(0);
		this.cb.onCreate(sActivity.get(), adsID, param.get(1), this);
		cb.getPreferences().setImpressionsUseActivities(true);
		onStart();
	}
	
	//load an Ad
	public void loadAd(final Object parameters)
	{
		SparseArray<String> param = (SparseArray<String>)parameters;
		final String type = param.get(0);
		final String tag = param.get(1);
		if(type.equals("interstitial")){
			if(tag != null)
				Chartboost.sharedChartboost().cacheInterstitial(tag);
			else
				Chartboost.sharedChartboost().cacheInterstitial();
				mngr.set(Chartboost.sharedChartboost(), type, new AdsStateChangeListener(){

					@Override
					public void onShow() {
						Ads.adDisplayed(me, type);
						if(tag != null)
							Chartboost.sharedChartboost().showInterstitial(tag);
						else
							Chartboost.sharedChartboost().showInterstitial();
					}

					@Override
					public void onDestroy() {
					}	
					@Override
					public void onHide() {
					}	
				});
				mngr.setAutoKill(type, false);
		}
		else if(type.equals("moreapps")){
			Chartboost.sharedChartboost().cacheMoreApps();
			mngr.set(Chartboost.sharedChartboost(), type, new AdsStateChangeListener(){

				@Override
				public void onShow() {
					Ads.adDisplayed(me, type);
					Chartboost.sharedChartboost().showMoreApps();
				}

				@Override
				public void onDestroy() {}	
				@Override
				public void onHide() {}	
			});
		}
		else
		{
			Ads.adError(this, "Unknown type: " + type);
		}
	}
	
	public void showAd(final Object parameters)
	{
		SparseArray<String> param = (SparseArray<String>)parameters;
		String type = param.get(0);
		if(mngr.get(type) == null)
		{
			loadAd(parameters);
		}
		mngr.show(type);
	}
	
	//remove ad
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
	public void didDismissInterstitial(String arg0) {
		Ads.adDismissed(this, "interstitial");
	}

	@Override
	public void didDismissMoreApps() {
		Ads.adDismissed(this, "moreapps");
	}

	@Override
	public void didShowInterstitial(String arg0) {
	}

	@Override
	public void didShowMoreApps() {}
	
	@Override
	public void didCloseMoreApps() {
		Ads.adActionEnd(this, "moreapps");
	}

	
	@Override
	public void didCloseInterstitial(String arg0) {
		Ads.adActionEnd(this, "interstitial");
	}
	
	@Override
	public void didCacheInterstitial(String arg0) {
		mngr.load("interstitial");
		Ads.adReceived(this, "interstitial");
	}

	@Override
	public void didCacheMoreApps() {
		mngr.load("moreapps");
		Ads.adReceived(this, "moreapps");
	}

	@Override
	public void didClickInterstitial(String arg0) {
		Ads.adActionBegin(this, "interstitial");
	}

	@Override
	public void didClickMoreApps() {
		Ads.adActionBegin(this, "moreapps");
	}

	@Override
	public boolean shouldDisplayInterstitial(String arg0) {
		return true;
	}

	@Override
	public boolean shouldDisplayLoadingViewForMoreApps() {
		return true;
	}

	@Override
	public boolean shouldDisplayMoreApps() {
		return true;
	}

	@Override
	public boolean shouldRequestInterstitial(String arg0) {
		return true;
	}

	@Override
	public boolean shouldRequestInterstitialsInFirstSession() {
		return true;
	}

	@Override
	public boolean shouldRequestMoreApps() {
		return true;
	}

	@Override
	public void enableTesting() {
		//should be enabled inside chartboost account
		//String android_id = Secure.getString(sActivity.get().getBaseContext().getContentResolver(),Secure.ANDROID_ID);
		//Log.e("Chartboost", android_id);
	}

	@Override
	public void didFailToLoadInterstitial(String arg0, CBImpressionError error) {
		Ads.adFailed(this, "interstitial", error.name());
		mngr.reset("interstitial");
	}

	@Override
	public void didFailToLoadMoreApps(CBImpressionError arg0) {
		Ads.adFailed(this, "moreapps", "Error");
		mngr.reset("moreapps");
	}

	@Override
	public void didFailToRecordClick(String arg0, CBClickError arg1) {
	}

	@Override
	public boolean shouldPauseClickForConfirmation(CBAgeGateConfirmation arg0) {
		return false;
	}
}
