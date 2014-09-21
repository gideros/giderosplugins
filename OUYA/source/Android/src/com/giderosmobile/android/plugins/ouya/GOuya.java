package com.giderosmobile.android.plugins.ouya;

import java.io.UnsupportedEncodingException;
import java.lang.ref.WeakReference;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.spec.X509EncodedKeySpec;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import org.json.JSONException;
import org.json.JSONObject;

import tv.ouya.console.api.OuyaAuthenticationHelper;
import tv.ouya.console.api.OuyaController;
import tv.ouya.console.api.OuyaEncryptionHelper;
import tv.ouya.console.api.OuyaErrorCodes;
import tv.ouya.console.api.OuyaFacade;
import tv.ouya.console.api.OuyaPurchaseHelper;
import tv.ouya.console.api.OuyaResponseListener;
import tv.ouya.console.api.Product;
import tv.ouya.console.api.Purchasable;
import tv.ouya.console.api.Receipt;

import android.accounts.AccountManager;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.util.SparseArray;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class GOuya
{
	public static WeakReference<Activity> sActivity;
	private static long sData = 0;
	private static float epsilon = 0.00001f;
	public static PublicKey mPublicKey;
	public static HashMap<String,String> mOutstandingPurchaseRequests;
	private static final String PRODUCTS_INSTANCE_STATE_KEY = "Products";
	private static final String RECEIPTS_INSTANCE_STATE_KEY = "Receipts";
	public static final int PURCHASE_AUTHENTICATION_ACTIVITY_ID = 1;
	public static final int GAMER_UUID_AUTHENTICATION_ACTIVITY_ID = 2;
	public static OuyaFacade ouyaFacade;
	private static float lastLS_X = 0;
	private static float lastLS_Y = 0;
	private static float lastRS_X = 0;
	private static float lastRS_Y = 0;
     
    /*
     * Handling app events
     */
    
	public static void onCreate(Activity activity)
	{
		ouyaFacade = OuyaFacade.getInstance();
		sActivity =  new WeakReference<Activity>(activity);
		mOutstandingPurchaseRequests = new HashMap<String, String>();
		//init ouya controller
		OuyaController.init(sActivity.get());
	}
    
    static public void onDestroy()
    {
    	cleanup(); 
    }
    
    static public void onStart(){
    	IntentFilter accountsChangedFilter = new IntentFilter();
        accountsChangedFilter.addAction(AccountManager.LOGIN_ACCOUNTS_CHANGED_ACTION);
        sActivity.get().registerReceiver(mAuthChangeReceiver, accountsChangedFilter);
    }
    
    static public void onStop(){
    	sActivity.get().unregisterReceiver(mAuthChangeReceiver);
    }
	
	static public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        if(resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
                case GAMER_UUID_AUTHENTICATION_ACTIVITY_ID:
                    requestUserId();
                    break;
                case PURCHASE_AUTHENTICATION_ACTIVITY_ID:
                    restartInterruptedPurchase();
                    break;
            }
        }
    }
	
	/*
	 * Gideros methods
	 */
	
	static public void init(long data)
	{
		sData = data;
	}
	
	
	static public void cleanup()
	{
   		sData = 0;
   		ouyaFacade.shutdown();
	}

    /*
     * Controller Part
     */

    static public boolean onKeyDown(final int keyCode, KeyEvent event){
    	OuyaController controller = OuyaController.getControllerByDeviceId(event.getDeviceId());
    	if(controller != null)
    	{
    		int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
    		if(sData != 0)
    			onKeyDownEvent(keyCode, player, sData);
    		return true;
        }
        return false;
    }
    
    static public boolean onKeyUp(final int keyCode, KeyEvent event){
    	OuyaController controller = OuyaController.getControllerByDeviceId(event.getDeviceId());
    	if(controller != null)
    	{
    		int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
    		if(sData != 0)
    			onKeyUpEvent(keyCode, player, sData);
    		return true;
        }
        return false;
    }
    
    static public boolean onGenericMotionEvent(MotionEvent event) {
        OuyaController c = OuyaController.getControllerByDeviceId(event.getDeviceId());
        if (c != null) {
        	int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
        	
        	//joystick
        	if((event.getSource() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0) {
        		float LS_X = event.getAxisValue(OuyaController.AXIS_LS_X);
        	    float LS_Y = event.getAxisValue(OuyaController.AXIS_LS_Y);
        	    float RS_X = event.getAxisValue(OuyaController.AXIS_RS_X);
        	    float RS_Y = event.getAxisValue(OuyaController.AXIS_RS_Y);
        	    float L2 = event.getAxisValue(OuyaController.AXIS_L2);
        	    float R2 = event.getAxisValue(OuyaController.AXIS_R2);
        	    
        	    if(LS_X*LS_X + LS_Y*LS_Y <= OuyaController.STICK_DEADZONE * OuyaController.STICK_DEADZONE)
        	    {
        	    	LS_X = 0;
        	    	LS_Y = 0;
        	    }
        	    
        	    if(lastLS_X != LS_X || lastLS_Y != LS_Y)
        	    {
        	    	lastLS_X = LS_X;
        	    	lastLS_Y = LS_Y;
        	    	double strength = Math.sqrt(LS_X*LS_X + LS_Y*LS_Y);
        	    	double angle = Math.acos(LS_X/strength);
        	    	if(LS_Y>0)
        	    	{
        	    		angle= -angle+2*Math.PI;
        	    	}
        	    	angle = -angle+2*Math.PI;
        	    	if(sData != 0)
        	    		onLeftJoystick(LS_X, LS_Y, angle, strength, player, sData);
        	    }
        	    
        	    if(RS_X*RS_X + RS_Y*RS_Y <= OuyaController.STICK_DEADZONE * OuyaController.STICK_DEADZONE)
        	    {
        	    	RS_X = 0;
        	    	RS_Y = 0;
        	    }
        	    
        	    if(lastRS_X != RS_X || lastRS_Y != RS_Y)
        	    {
        	    	lastRS_X = RS_X;
        	    	lastRS_Y = RS_Y;
        	    	double strength = Math.sqrt(RS_X*RS_X + RS_Y*RS_Y);
        	    	
        	    	double angle = Math.acos(RS_X/strength);
        	    	if(RS_Y>0)
        	    	{
        	    		angle= -angle+2*Math.PI;
        	    	}
        	    	angle = -angle+2*Math.PI;
    
        	    	//Log.d("Angle", "" + angle*180/Math.PI );
        	    	
        	    	//if RS_X<0 then vajag +180 grâdus
        	    	if(sData != 0)
        	    		onRightJoystick(RS_X, RS_Y, angle, strength, player, sData);
        	    }
        	    
        	    
        	    if(Math.abs(L2) > OuyaController.STICK_DEADZONE)
        	    {
        	    	if(sData != 0)
        	    		onLeftTrigger(L2, player, sData);
        	    }
        	    
        	    if(Math.abs(R2) > OuyaController.STICK_DEADZONE)
        	    {
        	    	if(sData != 0)
        	    		onRightTrigger(R2, player, sData);
        	    }
        	}
        }
        return true;
    }
    
    /*
     * Controller Events
     */
    
	private static native void onKeyDownEvent(int keyCode, int playerId, long data);
	private static native void onKeyUpEvent(int keyCode, int playerId, long data);
	private static native void onRightJoystick(float x, float y, double angle, double strength, int playerId, long data);
	private static native void onLeftJoystick(float x, float y, double angle, double strength, int playerId, long data);
	private static native void onRightTrigger(float strength, int playerId, long data);
	private static native void onLeftTrigger(float strength, int playerId, long data);
	
	/*
     * User Account Change Events
     */
	
	static public void onUserChanged(String userId){
		if(sData != 0)
			onUserChanged(userId, sData);
	}
	
	private static native void onUserChanged(String userId, long data);
    
    /*
     * IAP part API
     */
    
    static public void setUp(String devId, byte[] appId){
    	ouyaFacade.init(sActivity.get(), devId);
    	 // Create a PublicKey object from the key data downloaded from the developer portal.
    	 try {
             X509EncodedKeySpec keySpec = new X509EncodedKeySpec(appId);
             KeyFactory keyFactory = KeyFactory.getInstance("RSA");
             mPublicKey = keyFactory.generatePublic(keySpec);
         } catch (Exception e) {
         }
    }
    
    static public void requestProducts(Object products){
    	List<Purchasable> list = new ArrayList<Purchasable>();
    	SparseArray<String> p = (SparseArray<String>)products;
        int size = p.size();
        for(int i = 0; i < size; i++) {
        	list.add(new Purchasable(p.valueAt(i)));
        }
        ouyaFacade.requestProductList(list, new GOuyaProductRequestListener());
    }
    
    static public void requestPurchase(String productId){
    	try{
    		makePurchase(productId);
    	} catch (Exception ex) {
            onPurchaseRequestFailed(ex.getMessage());
        }
    }
    
    static public void requestReceipts(){
    	ouyaFacade.requestReceipts(new GOuyaReceiptRequestListener());
    }
    
    static public void requestUserId(){
    	ouyaFacade.requestGamerUuid(new GOuyaUserRequestListener(false));
    }
    
    /*
     * IAP Event handlers
     */
	
	static public void onProductRequestSuccess(Object arr){
		if(sData != 0)
			onProductRequestSuccess(arr, sData);
	}
	
	static public void onProductRequestFailed(String error){
		if(sData != 0)
			onProductRequestFailed(error, sData);
	}
	
	static public void onPurchaseRequestSuccess(String productId){
		if(sData != 0)
			onPurchaseRequestSuccess(productId, sData);
	}
	
	static public void onPurchaseRequestFailed(String error){
		if(sData != 0)
			onPurchaseRequestFailed(error, sData);
	}
	
	static public void onReceiptRequestSuccess(Object arr){
		if(sData != 0)
			onReceiptRequestSuccess(arr, sData);
	}
	
	static public void onReceiptRequestFailed(String error){
		if(sData != 0)
			onReceiptRequestFailed(error, sData);
	}
	
	static public void onUserRequestSuccess(String userId){
		if(sData != 0)
			onUserRequestSuccess(userId, sData);
	}
	
	static public void onUserRequestFailed(String error){
		if(sData != 0)
			onUserRequestFailed(error, sData);
	}
	
	/*
     * IAP Native Event handlers
     */
	
	private static native void onProductRequestSuccess(Object arr, long data);
	private static native void onProductRequestFailed(String error, long data);
	private static native void onPurchaseRequestSuccess(String productId, long data);
	private static native void onPurchaseRequestFailed(String error, long data);
	private static native void onReceiptRequestSuccess(Object arr, long data);
	private static native void onReceiptRequestFailed(String error, long data);
	private static native void onUserRequestSuccess(String userId, long data);
	private static native void onUserRequestFailed(String error, long data);
	
	/*
	 * Helper functions
	 */
	
    static public void restartInterruptedPurchase() {
        final String suspendedPurchaseId = OuyaPurchaseHelper.getSuspendedPurchase(sActivity.get());
        if(suspendedPurchaseId == null) {
            return;
        }
        requestPurchase(suspendedPurchaseId);
    }
	
    
    static public void makePurchase(String productId) throws NoSuchAlgorithmException, JSONException, NoSuchProviderException, NoSuchPaddingException, InvalidKeyException, InvalidAlgorithmParameterException, IllegalBlockSizeException, BadPaddingException, UnsupportedEncodingException {
    	 SecureRandom sr = SecureRandom.getInstance("SHA1PRNG");

         // This is an ID that allows you to associate a successful purchase with
         // it's original request. The server does nothing with this string except
         // pass it back to you, so it only needs to be unique within this instance
         // of your app to allow you to pair responses with requests.
         String uniqueId = Long.toHexString(sr.nextLong());

         JSONObject purchaseRequest = new JSONObject();
         purchaseRequest.put("uuid", uniqueId);
         purchaseRequest.put("identifier", productId);
         purchaseRequest.put("testing", "true"); // This value is only needed for testing, not setting it results in a live purchase
         String purchaseRequestJson = purchaseRequest.toString();

         byte[] keyBytes = new byte[16];
         sr.nextBytes(keyBytes);
         SecretKey key = new SecretKeySpec(keyBytes, "AES");

         byte[] ivBytes = new byte[16];
         sr.nextBytes(ivBytes);
         IvParameterSpec iv = new IvParameterSpec(ivBytes);

         Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding", "BC");
         cipher.init(Cipher.ENCRYPT_MODE, key, iv);
         byte[] payload = cipher.doFinal(purchaseRequestJson.getBytes("UTF-8"));

         cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding", "BC");
         cipher.init(Cipher.ENCRYPT_MODE, mPublicKey);
         byte[] encryptedKey = cipher.doFinal(keyBytes);

         Purchasable purchasable =
                 new Purchasable(
                         productId,
                         Base64.encodeToString(encryptedKey, Base64.NO_WRAP),
                         Base64.encodeToString(ivBytes, Base64.NO_WRAP),
                         Base64.encodeToString(payload, Base64.NO_WRAP) );

         synchronized (mOutstandingPurchaseRequests) {
             mOutstandingPurchaseRequests.put(uniqueId, productId);
         }
    	ouyaFacade.requestPurchase(purchasable, new GOuyaPurchaseListener(productId));
    }
    
    private static BroadcastReceiver mAuthChangeReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
        	ouyaFacade.requestGamerUuid(new GOuyaUserRequestListener(true));
        }
    };
}

/*
 * Custom OUYA Request Listeners
 */

class GOuyaProductRequestListener implements OuyaResponseListener<ArrayList<Product>> {
    @Override
    public void onSuccess(ArrayList<Product> products) {
    	SparseArray<Bundle> arr = new SparseArray<Bundle>();
    	int i = 0; 
        for(Product p : products) {
        	Bundle map = new Bundle();
        	map.putString("id", p.getIdentifier());
        	map.putString("name", p.getName());
        	map.putInt("price", p.getPriceInCents());
        	arr.put(i, map);
        	i++;
        }
        GOuya.onProductRequestSuccess(arr);
    }

    @Override
    public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
    	GOuya.onProductRequestFailed(errorMessage);
    }

	@Override
	public void onCancel() {}
};

class GOuyaPurchaseListener implements OuyaResponseListener<String> {
	private String mProduct;
	
	GOuyaPurchaseListener(final String product) {
        mProduct = product;
    }
	
    @Override
    public void onSuccess(String result) {
    	String productId;
        try {
            OuyaEncryptionHelper helper = new OuyaEncryptionHelper();

            JSONObject response = new JSONObject(result);
            if(response.has("key") && response.has("iv")) {
                String id = helper.decryptPurchaseResponse(response, GOuya.mPublicKey);
                String storedProduct;
                synchronized (GOuya.mOutstandingPurchaseRequests) {
                    storedProduct = GOuya.mOutstandingPurchaseRequests.remove(id);
                }
                if(storedProduct == null || !storedProduct.equals(mProduct)) {
                    onFailure(
                        OuyaErrorCodes.THROW_DURING_ON_SUCCESS, 
                        "No purchase outstanding for the given purchase request",
                        Bundle.EMPTY);
                    return;
                }
                productId = storedProduct;
            } else {
                Product product = new Product(new JSONObject(result));
                productId = product.getIdentifier();
                if(!mProduct.equals(product.getIdentifier())) {
                    onFailure(
                        OuyaErrorCodes.THROW_DURING_ON_SUCCESS, 
                        "Purchased product is not the same as purchase request product", 
                        Bundle.EMPTY);
                    return;
                }
            }
            GOuya.onPurchaseRequestSuccess(productId);
        } catch (Exception e) {
        	onFailure(
                    OuyaErrorCodes.THROW_DURING_ON_SUCCESS, 
                    "Purchase failed", 
                    Bundle.EMPTY);
        }
    }

    @Override
    public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
    	 boolean wasHandledByAuthHelper =
                 OuyaAuthenticationHelper.
                         handleError(
                                 GOuya.sActivity.get(), errorCode, errorMessage,
                                 errorBundle, GOuya.PURCHASE_AUTHENTICATION_ACTIVITY_ID,
                                 new OuyaResponseListener<Void>() {
                                     @Override
                                     public void onSuccess(Void result) {
                                         GOuya.restartInterruptedPurchase();   // Retry the purchase if the error was handled.
                                     }

                                     @Override
                                     public void onFailure(int errorCode, String errorMessage,
                                                           Bundle optionalData) {
                                    	 GOuya.onPurchaseRequestFailed(errorMessage);
                                     }

                                     @Override
                                     public void onCancel() {
                                    	 GOuya.onPurchaseRequestFailed("User canceled purchase");
                                     }
                                 });


         if(!wasHandledByAuthHelper) {
        	 GOuya.onPurchaseRequestFailed(errorMessage);
         }
    }

	@Override
	public void onCancel() {
		GOuya.onPurchaseRequestFailed("User canceled purchase");		
	}
};

class GOuyaReceiptRequestListener implements OuyaResponseListener<String> {
    @Override
    public void onSuccess(String receiptResponse) {
    	OuyaEncryptionHelper helper = new OuyaEncryptionHelper();
        List<Receipt> receipts = null;
        try {
            JSONObject response = new JSONObject(receiptResponse);
            if(response.has("key") && response.has("iv")) {
                receipts = helper.decryptReceiptResponse(response, GOuya.mPublicKey);
            } else {
                receipts = helper.parseJSONReceiptResponse(receiptResponse);
            }
        } catch (Exception e) {
        	onFailure(
                    OuyaErrorCodes.THROW_DURING_ON_SUCCESS, 
                    e.getMessage(), 
                    Bundle.EMPTY);
        }
        SparseArray<Bundle> arr = new SparseArray<Bundle>();
        int i = 0; 
        for (Receipt r : receipts) {
        	Bundle map = new Bundle();
        	map.putString("id", r.getIdentifier());
        	map.putString("uuid", r.getUuid());
        	map.putString("userId", r.getGamer());
        	map.putInt("price", r.getPriceInCents());
        	map.putInt("time", (int)(r.getPurchaseDate().getTime()/1000));
            arr.put(i, map);
        	i++;
        }
        GOuya.onReceiptRequestSuccess(arr);
    }

    @Override
    public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
    	GOuya.onReceiptRequestFailed(errorMessage);
    }

	@Override
	public void onCancel() {}
};

class GOuyaUserRequestListener implements OuyaResponseListener<String> {
	private boolean newUser;
	
	GOuyaUserRequestListener(boolean isNew) {
        newUser = isNew;
    }
    @Override
    public void onSuccess(String result) {
    	if(newUser)
    	{
    		GOuya.onUserChanged(result);
    	}
    	else
    	{
    		GOuya.onUserRequestSuccess(result);
    	}
    }

    @Override
    public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
    	boolean wasHandledByAuthHelper =
                OuyaAuthenticationHelper.
                        handleError(
                                GOuya.sActivity.get(), errorCode, errorMessage,
                                errorBundle, GOuya.GAMER_UUID_AUTHENTICATION_ACTIVITY_ID,
                                new OuyaResponseListener<Void>() {
                                    @Override
                                    public void onSuccess(Void result) {
                                    	GOuya.ouyaFacade.requestGamerUuid(new GOuyaUserRequestListener(newUser));   // Retry the fetch if the error was handled.
                                    }

                                    @Override
                                    public void onFailure(int errorCode, String errorMessage,
                                                          Bundle optionalData) {
                                        GOuya.onUserRequestFailed(errorMessage);
                                    }

                                    @Override
                                    public void onCancel() {}
                                });

        if (!wasHandledByAuthHelper) {
        	GOuya.onUserRequestFailed(errorMessage);
        }
    }

	@Override
	public void onCancel() {}
};
