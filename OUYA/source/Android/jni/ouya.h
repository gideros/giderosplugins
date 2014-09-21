#ifndef OUYA_H
#define OUYA_H

#include <gglobal.h>
#include <gevent.h>
#include <string>

typedef struct ouya_KeyEvent
{
	int keyCode;
	int playerId;
} ouya_KeyEvent;

typedef struct ouya_JoystickEvent
{
	float x;
	float y;
	double angle;
	double strength;
	int playerId;
} ouya_JoystickEvent;

typedef struct ouya_TriggerEvent
{
	float x;
	float y;
	double angle;
	double strength;
	int playerId;
} ouya_TriggerEvent;



typedef struct ouya_Parameter
{
    const char *value;
} ouya_Parameter;

typedef struct Product
{
	std::string id;
	std::string name;
	int price;
} Product;

typedef struct ouya_Product
{
	const char* id;
	const char* name;
	int price;
} ouya_Product;

typedef struct ouya_Products
{
	int count;
	ouya_Product *products;
} ouya_Products;

typedef struct Receipt
{
	std::string id;
	std::string uuid;
	std::string userId;
	int price;
	int time;
} Receipt;

typedef struct ouya_Receipt
{
	const char *id;
	const char *uuid;
	const char *userId;
	int price;
	int time;
} ouya_Receipt;

typedef struct ouya_Receipts
{
	int count;
	ouya_Receipt *receipts;
} ouya_Receipts;

enum
{
	OUYA_KEY_DOWN_EVENT,
	OUYA_KEY_UP_EVENT,
	OUYA_RIGHT_JOYSTICK_EVENT,
	OUYA_LEFT_JOYSTICK_EVENT,
	OUYA_RIGHT_TRIGGER_EVENT,
	OUYA_LEFT_TRIGGER_EVENT,
	OUYA_USER_CHANGED_EVENT,
	OUYA_PRODUCT_REQUEST_SUCCESS_EVENT,
	OUYA_PRODUCT_REQUEST_FAILED_EVENT,
	OUYA_PURCHASE_REQUEST_SUCCESS_EVENT,
	OUYA_PURCHASE_REQUEST_FAILED_EVENT,
	OUYA_RECEIPT_REQUEST_SUCCESS_EVENT,
	OUYA_RECEIPT_REQUEST_FAILED_EVENT,
	OUYA_USER_REQUEST_SUCCESS_EVENT,
	OUYA_USER_REQUEST_FAILED_EVENT
};


#ifdef __cplusplus
extern "C" {
#endif

G_API void ouya_init();
G_API void ouya_cleanup();

G_API void ouya_initialize(const char *devId, const char *appId, size_t size);
G_API void ouya_requestProducts(const char * const *params);
G_API void ouya_requestPurchase(const char *prodId);
G_API void ouya_requestReceipts();
G_API void ouya_requestUserId();

G_API g_id ouya_addCallback(gevent_Callback callback, void *udata);
G_API void ouya_removeCallback(gevent_Callback callback, void *udata);
G_API void ouya_removeCallbackWithGid(g_id gid);

#ifdef __cplusplus
}
#endif

#endif