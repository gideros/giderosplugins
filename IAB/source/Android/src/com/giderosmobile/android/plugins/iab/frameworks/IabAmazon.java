package com.giderosmobile.android.plugins.iab.frameworks;

import java.lang.ref.WeakReference;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Map;
import java.util.Set;

import com.amazon.inapp.purchasing.Item;
import com.amazon.inapp.purchasing.ItemDataResponse;
import com.amazon.inapp.purchasing.Offset;
import com.amazon.inapp.purchasing.PurchaseResponse;
import com.amazon.inapp.purchasing.PurchaseUpdatesResponse;
import com.amazon.inapp.purchasing.PurchasingManager;
import com.amazon.inapp.purchasing.BasePurchasingObserver;
import com.amazon.inapp.purchasing.Receipt;
import com.giderosmobile.android.plugins.iab.Iab;
import com.giderosmobile.android.plugins.iab.IabInterface;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.SparseArray;

public class IabAmazon implements IabInterface {
	public static WeakReference<Activity> sActivity;
	public static boolean sdkAvailable = false;
	public static boolean wasChecked = false;
	
	public static Boolean isInstalled(){
		if(android.os.Build.MANUFACTURER == "Amazon" || Iab.isPackageInstalled("com.amazon.venezia") || checkLoader())
			return true;
		return false;
	}
	
	public static boolean checkLoader(){
		boolean res = false;
		try {
            ClassLoader localClassLoader = IabAmazon.class.getClassLoader();
            localClassLoader.loadClass("com.amazon.android.Kiwi");
            res = true;
        } catch (Throwable localThrowable) {
        }
		return res;
	}

	@Override
	public void onCreate(WeakReference<Activity> activity) {
		sActivity = activity;
		PurchasingManager.registerObserver(new IabAmazonObserver(this));
	}

	@Override
	public void onDestroy() {}
	
	@Override
	public void onStart() {}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {}

	@Override
	public void init(Object parameters) {
		
	}

	@Override
	public void check() {
		if(sdkAvailable == true){
			Iab.available(this);
		}
		else
		{
			wasChecked = true;
		}
	}
	

	@Override
	public void request(Hashtable<String, String> products) {
		Set<String> skuSet = new HashSet<String>();
    	Enumeration<String> e = products.keys();
		while(e.hasMoreElements())
		{
			String prodName = e.nextElement();
        	skuSet.add(products.get(prodName));
        }
        PurchasingManager.initiateItemDataRequest(skuSet);
	}

	@Override
	public void purchase(String productId) {
		PurchasingManager.initiatePurchaseRequest(productId);
	}

	@Override
	public void restore() {
		PurchasingManager.initiatePurchaseUpdatesRequest(Offset.BEGINNING);
	}
}

class IabAmazonObserver extends BasePurchasingObserver {
	
	private IabAmazon caller;
	
	public IabAmazonObserver(IabAmazon iabAmazon) {
		super(IabAmazon.sActivity.get());
		caller = iabAmazon;
	}
	
	@Override
	public void onSdkAvailable(boolean isSandboxMode){
		IabAmazon.sdkAvailable = true;
		if(IabAmazon.wasChecked)
		{
			Iab.available(caller);
			IabAmazon.wasChecked = false;
		}
	}
	 
	@Override
	public void onItemDataResponse(ItemDataResponse itemDataResponse) {
		switch (itemDataResponse.getItemDataRequestStatus()) { 
         	case SUCCESSFUL_WITH_UNAVAILABLE_SKUS:
         	case SUCCESSFUL:
         		final Map<String, Item> items = itemDataResponse.getItemData();
         		SparseArray<Bundle> arr = new SparseArray<Bundle>();
         		int i = 0; 
         		for (final String key : items.keySet()) {
         			Item item = items.get(key);
         	        Bundle map = new Bundle();
         	        map.putString("productId", item.getSku());
         	        map.putString("title", item.getTitle());
         	        map.putString("description", item.getDescription());
         	        map.putString("price", item.getPrice());
         	        arr.put(i, map);
         	        i++;
         		}
         		Iab.productsComplete(caller, arr);
         		break;
         	case FAILED:
         		Iab.restoreError(caller, "Failed");
         		break;
         }
	}
	 
	@Override
	public void onPurchaseResponse(PurchaseResponse purchaseResponse) {
	 
		switch (purchaseResponse.getPurchaseRequestStatus()) {
			case SUCCESSFUL:
				final Receipt receipt = purchaseResponse.getReceipt();
				Iab.purchaseComplete(caller, receipt.getSku(), purchaseResponse.getUserId()+receipt.getSku());
				break;
			case FAILED:
				Iab.purchaseError(caller, "Purchase Failed");
				break;
			case INVALID_SKU:
				Iab.purchaseError(caller, "Invalid SKU");
				break;
			case ALREADY_ENTITLED:
				Iab.purchaseError(caller, "Item was already purchased");
				break;
         }
	 
	}
	
	@Override
	 
    public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse purchaseUpdatesResponse) {
		switch (purchaseUpdatesResponse.getPurchaseUpdatesRequestStatus()) { 
        	case SUCCESSFUL:
        		for (final Receipt receipt : purchaseUpdatesResponse.getReceipts()) {
        			final String sku = receipt.getSku();
        			switch (receipt.getItemType()) {
        				case ENTITLED:
        					Iab.purchaseComplete(caller, sku, purchaseUpdatesResponse.getUserId()+receipt.getSku());
        					break;
        			}
        		}
        		final Offset newOffset = purchaseUpdatesResponse.getOffset();
        		if (purchaseUpdatesResponse.isMore()) {
        			PurchasingManager.initiatePurchaseUpdatesRequest(newOffset);
        		}
        		else
        		{
        			Iab.restoreComplete(caller);
        		}
        		break;
        	case FAILED:
        		Iab.restoreError(caller, "Request Failed");
        }
	}
} 
