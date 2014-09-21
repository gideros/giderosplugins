#include <ouya.h>
#include <jni.h>
#include <stdlib.h>
#include <glog.h>

extern "C" {
JavaVM *g_getJavaVM();
JNIEnv *g_getJNIEnv();
}

class GOUYA
{
public:
	GOUYA()
	{
		gid_ = g_NextId();
		
		JNIEnv *env = g_getJNIEnv();

		jclass localClass = env->FindClass("com/giderosmobile/android/plugins/ouya/GOuya");
		cls_ = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);
		
		jclass class_bundle = env->FindClass("android/os/Bundle");
		clsBundle = static_cast<jclass>(env->NewGlobalRef(class_bundle));
		env->DeleteLocalRef(class_bundle);
		
		jclass class_sparse = env->FindClass("android/util/SparseArray");
		clsSparse = static_cast<jclass>(env->NewGlobalRef(class_sparse));
		env->DeleteLocalRef(class_sparse);

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "init", "(J)V"), (jlong)this);
	}

	~GOUYA()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "cleanup", "()V"));
		
		env->DeleteGlobalRef(cls_);
		env->DeleteGlobalRef(clsBundle);
		env->DeleteGlobalRef(clsSparse);
		
		gevent_RemoveEventsWithGid(gid_);
	}
	
	void initialize(const char *devId, const char *appId, size_t size){
		JNIEnv *env = g_getJNIEnv();
		jstring jdevId = env->NewStringUTF(devId);
		jbyteArray jappId = env->NewByteArray(size);
		env->SetByteArrayRegion(jappId, 0, size, (const jbyte*)appId);
		env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "setUp", "(Ljava/lang/String;[B)V"), jdevId, jappId);
		env->DeleteLocalRef(jdevId);
		env->DeleteLocalRef(jappId);
	}
	
	void requestProducts(const char * const *params){
		JNIEnv *env = g_getJNIEnv();
		jobject jparams = env->NewObject(clsSparse, env->GetMethodID(clsSparse, "<init>", "()V"));
		int i = 0;
		while (*params)
		{
			jstring jVal = env->NewStringUTF(*params);
			env->CallVoidMethod(jparams, env->GetMethodID(clsSparse, "put", "(ILjava/lang/Object;)V"), (jint)i, jVal);
			env->DeleteLocalRef(jVal);
			params++;
			i++;
		}
		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "requestProducts", "(Ljava/lang/Object;)V"), jparams);
		env->DeleteLocalRef(jparams);
	}
	
	void requestPurchase(const char *prodId){
		JNIEnv *env = g_getJNIEnv();
		jstring jprodId = env->NewStringUTF(prodId);
		env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "requestPurchase", "(Ljava/lang/String;)V"), jprodId);
		env->DeleteLocalRef(jprodId);
	}
	
	void requestReceipts(){
		JNIEnv *env = g_getJNIEnv();
		env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "requestReceipts", "()V"));
	}
	
	void requestUserId(){
		JNIEnv *env = g_getJNIEnv();
		env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "requestUserId", "()V"));
	}
	
	void onKeyDownEvent(jint keyCode, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_KeyEvent *event = (ouya_KeyEvent*)malloc(sizeof(ouya_KeyEvent));
		event->keyCode = (int)keyCode;
		event->playerId = (int)playerId;
		gevent_EnqueueEvent(gid_, callback_s, OUYA_KEY_DOWN_EVENT, event, 1, this);
	}
	
	void onKeyUpEvent(jint keyCode, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_KeyEvent *event = (ouya_KeyEvent*)malloc(sizeof(ouya_KeyEvent));
		event->keyCode = (int)keyCode;
		event->playerId = (int)playerId;
		gevent_EnqueueEvent(gid_, callback_s, OUYA_KEY_UP_EVENT, event, 1, this);
	}
	
	void onRightJoystick(jfloat x, jfloat y, jdouble angle, jdouble strength, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_JoystickEvent *event = (ouya_JoystickEvent*)malloc(sizeof(ouya_JoystickEvent));
		event->x = (float)x;
		event->y = (float)y;
		event->angle = (double)angle;
		event->playerId = (int)playerId;
		event->strength = (double)strength;
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_RIGHT_JOYSTICK_EVENT, event, 1, this);
	}
	
	void onLeftJoystick(jfloat x, jfloat y, jdouble angle, jdouble strength, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_JoystickEvent *event = (ouya_JoystickEvent*)malloc(sizeof(ouya_JoystickEvent));
		event->x = (float)x;
		event->y = (float)y;
		event->angle = angle;
		event->playerId = (int)playerId;
		event->strength = (double)strength;
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_LEFT_JOYSTICK_EVENT, event, 1, this);
	}
	
	void onRightTrigger(jfloat strength, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_TriggerEvent *event = (ouya_TriggerEvent*)malloc(sizeof(ouya_TriggerEvent));
		event->strength = (float)strength;
		event->playerId = (int)playerId;
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_RIGHT_TRIGGER_EVENT, event, 1, this);
	}
	
	void onLeftTrigger(jfloat strength, jint playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		ouya_TriggerEvent *event = (ouya_TriggerEvent*)malloc(sizeof(ouya_TriggerEvent));
		event->strength = (float)strength;
		event->playerId = (int)playerId;
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_LEFT_TRIGGER_EVENT, event, 1, this);
	}
	
	void onUserChanged(jstring playerId)
	{
		JNIEnv *env = g_getJNIEnv();

		const char *value = env->GetStringUTFChars(playerId, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(playerId, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_USER_CHANGED_EVENT, event, 1, this);
	}
	
	std::string mapGetStr(const char *str, jobject jsubobj)
	{
		JNIEnv *env = g_getJNIEnv();
		//get value
		jstring jStr = env->NewStringUTF(str);
		jstring jretStr = (jstring)env->CallObjectMethod(jsubobj, env->GetMethodID(clsBundle, "getString", "(Ljava/lang/String;)Ljava/lang/String;"), jStr);
		env->DeleteLocalRef(jStr);
	
		const char *retVal = env->GetStringUTFChars(jretStr, NULL);
		std::string result = retVal;
		env->ReleaseStringUTFChars(jretStr, retVal);

		return result;
	}
	
	int mapGetInt(const char *str, jobject jsubobj)
	{
		JNIEnv *env = g_getJNIEnv();
		//get value
		jstring jStr = env->NewStringUTF(str);
		int ret = (int)env->CallIntMethod(jsubobj, env->GetMethodID(clsBundle, "getInt", "(Ljava/lang/String;)I"), jStr);
		env->DeleteLocalRef(jStr);
		
		return ret;
	}
	
	void map2products(jobject jmapobj)
	{
		JNIEnv *env = g_getJNIEnv();
		int size = (int)env->CallIntMethod(jmapobj, env->GetMethodID(clsSparse, "size", "()I"));
		if(size == 0)
		{
			return;
		}
		
		products.clear();
		
		for (int i = 0; i < size; i++) {
			jobject jsubobj = env->CallObjectMethod(jmapobj, env->GetMethodID(clsSparse, "valueAt", "(I)Ljava/lang/Object;"), (jint)i);
			
			Product gprod = {this->mapGetStr("id", jsubobj), this->mapGetStr("name", jsubobj), this->mapGetInt("price", jsubobj)};
			
			products.push_back(gprod);
			
			env->DeleteLocalRef(jsubobj);
		}
	}
	
	void map2receipts(jobject jmapobj)
	{
		JNIEnv *env = g_getJNIEnv();
		int size = (int)env->CallIntMethod(jmapobj, env->GetMethodID(clsSparse, "size", "()I"));
		if(size == 0)
		{
			return;
		}
		
		receipts.clear();
		
		for (int i = 0; i < size; i++) {
			jobject jsubobj = env->CallObjectMethod(jmapobj, env->GetMethodID(clsSparse, "valueAt", "(I)Ljava/lang/Object;"), (jint)i);
			
			Receipt grec = {this->mapGetStr("id", jsubobj), this->mapGetStr("uuid", jsubobj), this->mapGetStr("userId", jsubobj), this->mapGetInt("price", jsubobj), this->mapGetInt("time", jsubobj)};
			
			receipts.push_back(grec);
			
			env->DeleteLocalRef(jsubobj);
		}
	}
	
	void onProductRequestSuccess(jobject jproducts)
	{
		JNIEnv *env = g_getJNIEnv();
		this->map2products(jproducts);
		
		size_t size = sizeof(ouya_Products);
		int count = (int)products.size();
		
		for (std::size_t i = 0; i < count; ++i)
		{
			size += sizeof(ouya_Product);
			size += products[i].id.size() + 1;
			size += products[i].name.size() + 1;
		}
		
		// allocate it
		ouya_Products *event = (ouya_Products*)malloc(size);
		
		// and copy the data into it
		char *ptr = (char*)event + sizeof(ouya_Products);
		
		event->count = count;
		event->products = (ouya_Product*)ptr;
		
		ptr += products.size() * sizeof(ouya_Product);
		 
		for (std::size_t i = 0; i < count; ++i)
		{	
			event->products[i].id = ptr;
			strcpy(ptr, products[i].id.c_str());
			ptr += products[i].id.size() + 1;
		
			event->products[i].name = ptr;
			strcpy(ptr, products[i].name.c_str());
			ptr += products[i].name.size() + 1;
		
			event->products[i].price = products[i].price;
		}
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_PRODUCT_REQUEST_SUCCESS_EVENT, event, 1, this);
	}
	
	void onProductRequestFailed(jstring error)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(error, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(error, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_PRODUCT_REQUEST_FAILED_EVENT, event, 1, this);
	}
	
	void onPurchaseRequestSuccess(jstring productId)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(productId, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(productId, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_PURCHASE_REQUEST_SUCCESS_EVENT, event, 1, this);
	}
	
	void onPurchaseRequestFailed(jstring error)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(error, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(error, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_PURCHASE_REQUEST_FAILED_EVENT, event, 1, this);
	}
	
	void onReceiptRequestSuccess(jobject jreceipts)
	{
		JNIEnv *env = g_getJNIEnv();
		
		this->map2receipts(jreceipts);
		
		size_t size = sizeof(ouya_Receipts);
		int count = (int)receipts.size();
		
		for (std::size_t i = 0; i < count; ++i)
		{
			size += sizeof(ouya_Receipt);
			size += receipts[i].id.size() + 1;
			size += receipts[i].uuid.size() + 1;
			size += receipts[i].userId.size() + 1;
		}
		
		// allocate it
		ouya_Receipts *event = (ouya_Receipts*)malloc(size);
		
		// and copy the data into it
		char *ptr = (char*)event + sizeof(ouya_Receipts);
		
		event->count = count;
		event->receipts = (ouya_Receipt*)ptr;
		
		ptr += receipts.size() * sizeof(ouya_Receipt);
		 
		for (std::size_t i = 0; i < count; ++i)
		{	
			event->receipts[i].id = ptr;
			strcpy(ptr, receipts[i].id.c_str());
			ptr += receipts[i].id.size() + 1;
			
			event->receipts[i].uuid = ptr;
			strcpy(ptr, receipts[i].uuid.c_str());
			ptr += receipts[i].uuid.size() + 1;
			
			event->receipts[i].userId = ptr;
			strcpy(ptr, receipts[i].userId.c_str());
			ptr += receipts[i].userId.size() + 1;
		
			event->receipts[i].price = receipts[i].price;
			event->receipts[i].time = receipts[i].time;
		}
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_RECEIPT_REQUEST_SUCCESS_EVENT, event, 1, this);
	}
	
	void onReceiptRequestFailed(jstring error)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(error, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(error, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_RECEIPT_REQUEST_FAILED_EVENT, event, 1, this);
	}
	
	void onUserRequestSuccess(jstring userId)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(userId, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(userId, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_USER_REQUEST_SUCCESS_EVENT, event, 1, this);
	}
	
	void onUserRequestFailed(jstring error)
	{
		JNIEnv *env = g_getJNIEnv();
		
		const char *value = env->GetStringUTFChars(error, NULL);
		
		ouya_Parameter *event = (ouya_Parameter*)gevent_CreateEventStruct1(
			sizeof(ouya_Parameter),
			offsetof(ouya_Parameter, value), value);
	
		env->ReleaseStringUTFChars(error, value);
		
		gevent_EnqueueEvent(gid_, callback_s, OUYA_USER_REQUEST_FAILED_EVENT, event, 1, this);
	}
	
	g_id addCallback(gevent_Callback callback, void *udata)
	{
		return callbackList_.addCallback(callback, udata);
	}
	void removeCallback(gevent_Callback callback, void *udata)
	{
		callbackList_.removeCallback(callback, udata);
	}
	void removeCallbackWithGid(g_id gid)
	{
		callbackList_.removeCallbackWithGid(gid);
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		((GOUYA*)udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		callbackList_.dispatchEvent(type, event);
	}

private:
	gevent_CallbackList callbackList_;

private:
	jclass cls_;
	jclass clsBundle;
	jclass clsSparse;
	std::vector<Product> products;
	std::vector<Receipt> receipts;
	g_id gid_;
};

extern "C" {

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onKeyDownEvent(JNIEnv *env, jclass clz, jint keyCode, jint playerId, jlong data)
{
	((GOUYA*)data)->onKeyDownEvent(keyCode, playerId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onKeyUpEvent(JNIEnv *env, jclass clz, jint keyCode, jint playerId, jlong data)
{
	((GOUYA*)data)->onKeyUpEvent(keyCode, playerId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onRightJoystick(JNIEnv *env, jclass clz, jfloat x, jfloat y, jdouble angle, jdouble strength, jint playerId, jlong data)
{
	((GOUYA*)data)->onRightJoystick(x, y, angle, strength, playerId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onLeftJoystick(JNIEnv *env, jclass clz, jfloat x, jfloat y, jdouble angle, jdouble strength, jint playerId, jlong data)
{
	((GOUYA*)data)->onLeftJoystick(x, y, angle, strength, playerId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onRightTrigger(JNIEnv *env, jclass clz, jfloat strength, jint playerId, jlong data)
{
	((GOUYA*)data)->onRightTrigger(strength, playerId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onLeftTrigger(JNIEnv *env, jclass clz, jfloat strength, jint playerId, jlong data)
{
	((GOUYA*)data)->onLeftTrigger(strength, playerId);
}


void Java_com_giderosmobile_android_plugins_ouya_GOuya_onUserChanged(JNIEnv *env, jclass clz, jstring playerId, jlong data)
{
	((GOUYA*)data)->onUserChanged(playerId);
}


void Java_com_giderosmobile_android_plugins_ouya_GOuya_onProductRequestSuccess(JNIEnv *env, jclass clz, jobject products, jlong data)
{
	((GOUYA*)data)->onProductRequestSuccess(products);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onProductRequestFailed(JNIEnv *env, jclass clz, jstring error, jlong data)
{
	((GOUYA*)data)->onProductRequestFailed(error);
}


void Java_com_giderosmobile_android_plugins_ouya_GOuya_onPurchaseRequestSuccess(JNIEnv *env, jclass clz, jstring productId, jlong data)
{
	((GOUYA*)data)->onPurchaseRequestSuccess(productId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onPurchaseRequestFailed(JNIEnv *env, jclass clz, jstring error, jlong data)
{
	((GOUYA*)data)->onPurchaseRequestFailed(error);
}


void Java_com_giderosmobile_android_plugins_ouya_GOuya_onReceiptRequestSuccess(JNIEnv *env, jclass clz, jobject receipts, jlong data)
{
	((GOUYA*)data)->onReceiptRequestSuccess(receipts);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onReceiptRequestFailed(JNIEnv *env, jclass clz, jstring error, jlong data)
{
	((GOUYA*)data)->onReceiptRequestFailed(error);
}


void Java_com_giderosmobile_android_plugins_ouya_GOuya_onUserRequestSuccess(JNIEnv *env, jclass clz, jstring userId, jlong data)
{
	((GOUYA*)data)->onUserRequestSuccess(userId);
}

void Java_com_giderosmobile_android_plugins_ouya_GOuya_onUserRequestFailed(JNIEnv *env, jclass clz, jstring error, jlong data)
{
	((GOUYA*)data)->onUserRequestFailed(error);
}

}

static GOUYA *s_ouya = NULL;

extern "C" {

void ouya_init()
{
	s_ouya = new GOUYA;
}

void ouya_cleanup()
{
	delete s_ouya;
	s_ouya = NULL;
}

void ouya_initialize(const char *devId, const char *appId, size_t size)
{
	s_ouya->initialize(devId, appId, size);
}

void ouya_requestProducts(const char * const *params)
{
	s_ouya->requestProducts(params);
}

void ouya_requestPurchase(const char *prodId)
{
	s_ouya->requestPurchase(prodId);
}

void ouya_requestReceipts()
{
	s_ouya->requestReceipts();
}

void ouya_requestUserId()
{
	s_ouya->requestUserId();
}


g_id ouya_addCallback(gevent_Callback callback, void *udata)
{
	return s_ouya->addCallback(callback, udata);
}

void ouya_removeCallback(gevent_Callback callback, void *udata)
{
	s_ouya->removeCallback(callback, udata);
}

void ouya_removeCallbackWithGid(g_id gid)
{
	s_ouya->removeCallbackWithGid(gid);
}

}
