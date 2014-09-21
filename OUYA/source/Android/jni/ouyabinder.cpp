#include "ouya.h"
#include "gideros.h"
#include <glog.h>

// some Lua helper functions
#ifndef abs_index
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif

static void luaL_newweaktable(lua_State *L, const char *mode)
{
	lua_newtable(L);			// create table for instance list
	lua_pushstring(L, mode);
	lua_setfield(L, -2, "__mode");	  // set as weak-value table
	lua_pushvalue(L, -1);             // duplicate table
	lua_setmetatable(L, -2);          // set itself as metatable
}

static void luaL_rawgetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_rawget(L, idx);
}

static void luaL_rawsetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

static const char* RIGHT_JOYSTICK = "rightJoystick";
static const char* LEFT_JOYSTICK = "leftJoystick";
static const char* RIGHT_TRIGGER = "rightTrigger";
static const char* LEFT_TRIGGER = "leftTrigger";
static const char* KEY_DOWN = "keyDown";
static const char* KEY_UP = "keyUp";
static const char* USER_CHANGED = "userChanged";
static const char* PRODUCT_REQUEST_SUCCESS = "productRequestSuccess";
static const char* PRODUCT_REQUEST_FAILED = "productRequestFailed";
static const char* PURCHASE_REQUEST_SUCCESS = "purchaseRequestSuccess";
static const char* PURCHASE_REQUEST_FAILED = "purchaseRequestFailed";
static const char* RECEIPT_REQUEST_SUCCESS = "receiptRequestSuccess";
static const char* RECEIPT_REQUEST_FAILED = "receiptRequestFailed";
static const char* USER_REQUEST_SUCCESS = "userRequestSuccess";
static const char* USER_REQUEST_FAILED = "userRequestFailed";

static const int BUTTON_A = 97;
static const int BUTTON_DPAD_DOWN = 20;
static const int BUTTON_DPAD_LEFT = 21;
static const int BUTTON_DPAD_RIGHT = 22;
static const int BUTTON_DPAD_UP = 19;
static const int BUTTON_L1 = 102;
static const int BUTTON_L2 = 104;
static const int BUTTON_L3 = 106;
static const int BUTTON_MENU = 82;
static const int BUTTON_O = 96;
static const int BUTTON_R1 = 103;
static const int BUTTON_R2 = 105;
static const int BUTTON_R3 = 107;
static const int BUTTON_U = 99;
static const int BUTTON_Y = 100;

static char keyWeak = ' ';

class Ouya : public GEventDispatcherProxy
{
public:
    Ouya(lua_State *L) : L(L)
    {
        ouya_init();
		ouya_addCallback(callback_s, this);		
		initialized_ = false;
    }
    
    ~Ouya()
    {
		ouya_removeCallback(callback_s, this);
		ouya_cleanup();
    }
	
	void checkInit()
    {
        if (!initialized_)
            luaL_error(L, "You need to provide devId and appKey using ouya:init(devId, appKey) before calling this method.");
    }
	
	void init(const char *devId, const char *appId, size_t size)
	{
		if (initialized_)
			luaL_error(L, "Ouya already initialized");
		ouya_initialize(devId, appId, size);
		initialized_ = true;
	}
	
	void requestProducts(const char * const *params)
	{
		checkInit();
		ouya_requestProducts(params);
	}
	
	void requestPurchase(const char *prodId)
	{
		checkInit();
		ouya_requestPurchase(prodId);
	}
	
	void requestReceipts()
	{
		checkInit();
		ouya_requestReceipts();
	}
	
	void requestUserId()
	{
		checkInit();
		ouya_requestUserId();
	}
	
private:
	static void callback_s(int type, void *event, void *udata)
	{
		static_cast<Ouya*>(udata)->callback(type, event);
	}
	
	void callback(int type, void *event)
	{
        dispatchEvent(type, event);
	}
	
	void dispatchEvent(int type, void *event)
	{
        luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
        luaL_rawgetptr(L, -1, this);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 2);
            return;
        }
        
        lua_getfield(L, -1, "dispatchEvent");
		
        lua_pushvalue(L, -2);
        
        lua_getglobal(L, "Event");
        lua_getfield(L, -1, "new");
        lua_remove(L, -2);
        
        switch (type)
        {
			case OUYA_KEY_DOWN_EVENT:
                lua_pushstring(L, KEY_DOWN);
                break;
			case OUYA_KEY_UP_EVENT:
                lua_pushstring(L, KEY_UP);
                break;
			case OUYA_RIGHT_JOYSTICK_EVENT:
                lua_pushstring(L, RIGHT_JOYSTICK);
                break;
			case OUYA_LEFT_JOYSTICK_EVENT:
                lua_pushstring(L, LEFT_JOYSTICK);
                break;
			case OUYA_RIGHT_TRIGGER_EVENT:
                lua_pushstring(L, RIGHT_TRIGGER);
                break;
			case OUYA_LEFT_TRIGGER_EVENT:
                lua_pushstring(L, LEFT_TRIGGER);
                break;
			case OUYA_USER_CHANGED_EVENT:
                lua_pushstring(L, USER_CHANGED);
                break;
			case OUYA_PRODUCT_REQUEST_SUCCESS_EVENT:
                lua_pushstring(L, PRODUCT_REQUEST_SUCCESS);
                break;
			case OUYA_PRODUCT_REQUEST_FAILED_EVENT:
                lua_pushstring(L, PRODUCT_REQUEST_FAILED);
                break;
			case OUYA_PURCHASE_REQUEST_SUCCESS_EVENT:
                lua_pushstring(L, PURCHASE_REQUEST_SUCCESS);
                break;
			case OUYA_PURCHASE_REQUEST_FAILED_EVENT:
                lua_pushstring(L, PURCHASE_REQUEST_FAILED);
                break;
			case OUYA_RECEIPT_REQUEST_SUCCESS_EVENT:
                lua_pushstring(L, RECEIPT_REQUEST_SUCCESS);
                break;
			case OUYA_RECEIPT_REQUEST_FAILED_EVENT:
                lua_pushstring(L, RECEIPT_REQUEST_FAILED);
                break;
			case OUYA_USER_REQUEST_SUCCESS_EVENT:
                lua_pushstring(L, USER_REQUEST_SUCCESS);
                break;
			case OUYA_USER_REQUEST_FAILED_EVENT:
                lua_pushstring(L, USER_REQUEST_FAILED);
                break;
        }

        lua_call(L, 1, 1);
		
		if (type == OUYA_KEY_DOWN_EVENT || type == OUYA_KEY_UP_EVENT)
        {
            ouya_KeyEvent *event2 = (ouya_KeyEvent*)event;
            
			lua_pushnumber(L, event2->keyCode);
			lua_setfield(L, -2, "keyCode");
			
			lua_pushnumber(L, event2->playerId);
			lua_setfield(L, -2, "playerId");
        }
		else if (type == OUYA_RIGHT_JOYSTICK_EVENT || type == OUYA_LEFT_JOYSTICK_EVENT)
        {
            ouya_JoystickEvent *event2 = (ouya_JoystickEvent*)event;
			
			lua_pushnumber(L, event2->x);
			lua_setfield(L, -2, "x");
			
			lua_pushnumber(L, event2->y);
			lua_setfield(L, -2, "y");
			
			lua_pushnumber(L, event2->angle);
			lua_setfield(L, -2, "angle");
			
			lua_pushnumber(L, event2->strength);
			lua_setfield(L, -2, "strength");
			
			lua_pushnumber(L, event2->playerId);
			lua_setfield(L, -2, "playerId");
        }
		else if (type == OUYA_RIGHT_TRIGGER_EVENT || type == OUYA_LEFT_TRIGGER_EVENT)
        {
            ouya_JoystickEvent *event2 = (ouya_JoystickEvent*)event;
			
			lua_pushnumber(L, event2->strength);
			lua_setfield(L, -2, "strength");
			
			lua_pushnumber(L, event2->playerId);
			lua_setfield(L, -2, "playerId");
        }
		else if(type == OUYA_USER_CHANGED_EVENT)
		{
			ouya_Parameter *event2 = (ouya_Parameter*)event;
			
			lua_pushstring(L, event2->value);
			lua_setfield(L, -2, "userId");
		}
		else if(type == OUYA_PRODUCT_REQUEST_FAILED_EVENT || type == OUYA_PURCHASE_REQUEST_FAILED_EVENT || type == OUYA_RECEIPT_REQUEST_FAILED_EVENT || type == OUYA_USER_REQUEST_FAILED_EVENT)
		{
			ouya_Parameter *event2 = (ouya_Parameter*)event;
			
			lua_pushstring(L, event2->value);
			lua_setfield(L, -2, "error");
		}
		else if(type == OUYA_PURCHASE_REQUEST_SUCCESS_EVENT)
		{
			ouya_Parameter *event2 = (ouya_Parameter*)event;
			
			lua_pushstring(L, event2->value);
			lua_setfield(L, -2, "productId");
		}
		else if(type == OUYA_USER_REQUEST_SUCCESS_EVENT)
		{
			ouya_Parameter *event2 = (ouya_Parameter*)event;
			
			lua_pushstring(L, event2->value);
			lua_setfield(L, -2, "userId");
		}
		else if(type == OUYA_PRODUCT_REQUEST_SUCCESS_EVENT)
		{
			ouya_Products *event2 = (ouya_Products*)event;
            
			lua_newtable(L);
			
			for (int i = 0; i < event2->count; ++i)
			{					
				lua_newtable(L);
				
				lua_pushstring(L, event2->products[i].id);
				lua_setfield(L, -2, "id");
	
				lua_pushstring(L, event2->products[i].name);
				lua_setfield(L, -2, "name");
				
				lua_pushnumber(L, event2->products[i].price);
				lua_setfield(L, -2, "price");
				
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "products");
		}
		else if(type == OUYA_RECEIPT_REQUEST_SUCCESS_EVENT)
		{
			ouya_Receipts *event2 = (ouya_Receipts*)event;
            
			lua_newtable(L);
			
			for (int i = 0; i < event2->count; ++i)
			{					
				lua_newtable(L);
				
				lua_pushstring(L, event2->receipts[i].id);
				lua_setfield(L, -2, "id");
				
				lua_pushstring(L, event2->receipts[i].uuid);
				lua_setfield(L, -2, "uuid");
				
				lua_pushstring(L, event2->receipts[i].userId);
				lua_setfield(L, -2, "userId");
				
				lua_pushnumber(L, event2->receipts[i].price);
				lua_setfield(L, -2, "price");
				
				lua_pushnumber(L, event2->receipts[i].time);
				lua_setfield(L, -2, "time");
				
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "receipts");
		}

		lua_call(L, 2, 0);
		
		lua_pop(L, 2);
	}

private:
    lua_State *L;
    bool initialized_;
};

static int destruct(lua_State* L)
{
	void *ptr = *(void**)lua_touserdata(L, 1);
	GReferenced* object = static_cast<GReferenced*>(ptr);
	Ouya *ouya = static_cast<Ouya*>(object->proxy());
	
	ouya->unref();
	
	return 0;
}

static Ouya *getInstance(lua_State* L, int index)
{
	GReferenced *object = static_cast<GReferenced*>(g_getInstance(L, "Ouya", index));
	Ouya *ouya = static_cast<Ouya*>(object->proxy());
    
	return ouya;
}

static int init(lua_State* L)
{
	Ouya *ouya = getInstance(L, 1);
	const char *devId = luaL_checkstring(L, 2);
	size_t size;
	const char *appId = luaL_checklstring(L, 3, &size);
	ouya->init(devId, appId, size);
	return 0;
}

static std::vector<std::string> tableToVector(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);

    std::vector<std::string> result;
    int n = lua_objlen(L, index);
    for (int i = 1; i <= n; ++i)
    {
        lua_rawgeti(L, index, i);
        result.push_back(luaL_checkstring(L, -1));
        lua_pop(L, 1);
    }

    return result;
}

static int requestProducts(lua_State* L)
{
	Ouya *ouya = getInstance(L, 1);
	std::vector<std::string> param = tableToVector(L, 2);

	std::vector<const char*> param2;
	for (size_t i = 0; i < param.size(); ++i)
		param2.push_back(param[i].c_str());
	param2.push_back(NULL);
	
	ouya->requestProducts(&param2[0]);
	return 0;
}

static int requestPurchase(lua_State* L)
{
	Ouya *ouya = getInstance(L, 1);
	const char *prodId = luaL_checkstring(L, 2);
	ouya->requestPurchase(prodId);
	return 0;
}

static int requestReceipts(lua_State* L)
{
	Ouya *ouya = getInstance(L, 1);
	ouya->requestReceipts();
	return 0;
}

static int requestUserId(lua_State* L)
{
	Ouya *ouya = getInstance(L, 1);
	ouya->requestUserId();
	return 0;
}

static int loader(lua_State *L)
{
	const luaL_Reg functionlist[] = {
		{"init", init},
		{"requestProducts", requestProducts},
		{"requestPurchase", requestPurchase},
		{"requestReceipts", requestReceipts},
		{"requestUserId", requestUserId},
		{NULL, NULL},
	};
    
    g_createClass(L, "Ouya", "EventDispatcher", NULL, destruct, functionlist);
    
	// create a weak table in LUA_REGISTRYINDEX that can be accessed with the address of keyWeak
    luaL_newweaktable(L, "v");
    luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);
    
	lua_getglobal(L, "Event");
	lua_pushstring(L, RIGHT_JOYSTICK);
	lua_setfield(L, -2, "RIGHT_JOYSTICK");
	lua_pushstring(L, LEFT_JOYSTICK);
	lua_setfield(L, -2, "LEFT_JOYSTICK");
	lua_pushstring(L, RIGHT_TRIGGER);
	lua_setfield(L, -2, "RIGHT_TRIGGER");
	lua_pushstring(L, LEFT_TRIGGER);
	lua_setfield(L, -2, "LEFT_TRIGGER");
	lua_pushstring(L, USER_CHANGED);
	lua_setfield(L, -2, "USER_CHANGED");
	lua_pushstring(L, PRODUCT_REQUEST_SUCCESS);
	lua_setfield(L, -2, "PRODUCT_REQUEST_SUCCESS");
	lua_pushstring(L, PRODUCT_REQUEST_FAILED);
	lua_setfield(L, -2, "PRODUCT_REQUEST_FAILED");
	lua_pushstring(L, PURCHASE_REQUEST_SUCCESS);
	lua_setfield(L, -2, "PURCHASE_REQUEST_SUCCESS");
	lua_pushstring(L, PURCHASE_REQUEST_FAILED);
	lua_setfield(L, -2, "PURCHASE_REQUEST_FAILED");
	lua_pushstring(L, RECEIPT_REQUEST_SUCCESS);
	lua_setfield(L, -2, "RECEIPT_REQUEST_SUCCESS");
	lua_pushstring(L, RECEIPT_REQUEST_FAILED);
	lua_setfield(L, -2, "RECEIPT_REQUEST_FAILED");
	lua_pushstring(L, USER_REQUEST_SUCCESS);
	lua_setfield(L, -2, "USER_REQUEST_SUCCESS");
	lua_pushstring(L, USER_REQUEST_FAILED);
	lua_setfield(L, -2, "USER_REQUEST_FAILED");
	lua_pop(L, 1);
	
	lua_getglobal(L, "KeyCode");
	lua_pushnumber(L, BUTTON_DPAD_DOWN);
	lua_setfield(L, -2, "BUTTON_DPAD_DOWN");
	lua_pushnumber(L, BUTTON_DPAD_LEFT);
	lua_setfield(L, -2, "BUTTON_DPAD_LEFT");
	lua_pushnumber(L, BUTTON_DPAD_RIGHT);
	lua_setfield(L, -2, "BUTTON_DPAD_RIGHT");
	lua_pushnumber(L, BUTTON_DPAD_UP);
	lua_setfield(L, -2, "BUTTON_DPAD_UP");
	lua_pushnumber(L, BUTTON_L1);
	lua_setfield(L, -2, "BUTTON_L1");
	lua_pushnumber(L, BUTTON_L2);
	lua_setfield(L, -2, "BUTTON_L2");
	lua_pushnumber(L, BUTTON_L3);
	lua_setfield(L, -2, "BUTTON_L3");
	lua_pushnumber(L, BUTTON_R1);
	lua_setfield(L, -2, "BUTTON_R1");
	lua_pushnumber(L, BUTTON_R2);
	lua_setfield(L, -2, "BUTTON_R2");
	lua_pushnumber(L, BUTTON_R3);
	lua_setfield(L, -2, "BUTTON_R3");
	lua_pushnumber(L, BUTTON_MENU);
	lua_setfield(L, -2, "BUTTON_MENU");
	lua_pushnumber(L, BUTTON_O);
	lua_setfield(L, -2, "BUTTON_O");
	lua_pushnumber(L, BUTTON_U);
	lua_setfield(L, -2, "BUTTON_U");
	lua_pushnumber(L, BUTTON_Y);
	lua_setfield(L, -2, "BUTTON_Y");
	lua_pushnumber(L, BUTTON_A);
	lua_setfield(L, -2, "BUTTON_A");
	lua_pop(L, 1);

    Ouya *ouya = new Ouya(L);
	g_pushInstance(L, "Ouya", ouya->object());
    
	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, ouya);
	lua_pop(L, 1);
    
	lua_pushvalue(L, -1);
	lua_setglobal(L, "ouya");
    
    return 1;
}
    
static void g_initializePlugin(lua_State *L)
{
    lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	
	lua_pushcfunction(L, loader);
	lua_setfield(L, -2, "ouya");
	
	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State *L)
{
    
}

REGISTER_PLUGIN("Ouya", "1.0")
