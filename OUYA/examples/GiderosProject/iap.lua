
local file = io.open("key.der", "rb")
local appKey = file:read( "*a" )
io.close( file )

ouya:init("00000000-0000-0000-0000-000000000000", appKey)

--ouya:requestProducts({"test", "test2"})
--ouya:requestPurchase("test")
--ouya:requestReceipts() 
--ouya:requestUserId()

stage:addEventListener(Event.MOUSE_DOWN, function(e)
	print("purchasing")
	ouya:requestPurchase("test")
end)


ouya:addEventListener(Event.PRODUCT_REQUEST_SUCCESS, function(e)
	print("PRODUCT_REQUEST_SUCCESS")
	print_r(e.products)
end)

ouya:addEventListener(Event.PURCHASE_REQUEST_SUCCESS, function(e)
	print("PURCHASE_REQUEST_SUCCESS", e.productId)
end)

ouya:addEventListener(Event.RECEIPT_REQUEST_SUCCESS, function(e)
	print("RECEIPT_REQUEST_SUCCESS")
	print_r(e.receipts)
end)

ouya:addEventListener(Event.USER_REQUEST_SUCCESS, function(e)
	print("USER_REQUEST_SUCCESS", e.userId)
end)



ouya:addEventListener(Event.PRODUCT_REQUEST_FAILED, function(e)
	print("PRODUCT_REQUEST_FAILED", e.error)
end)

ouya:addEventListener(Event.PURCHASE_REQUEST_FAILED, function(e)
	print("PURCHASE_REQUEST_FAILED", e.error)
end)

ouya:addEventListener(Event.RECEIPT_REQUEST_FAILED, function(e)
	print("RECEIPT_REQUEST_FAILED", e.error)
end)

ouya:addEventListener(Event.USER_REQUEST_FAILED, function(e)
	print("USER_REQUEST_FAILED", e.error)
end)

