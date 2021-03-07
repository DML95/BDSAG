function main()
	print("lua.main")
end

function getConfig()
	print("lua.getConfig")
	return 1000000,		--sizeDB
		100, 			--sizeSesion
		8080,			--port
		"INFO"			--logs
end

function checkDeviceUse(platformName,deviceType,deviceName)
	print("lua.checkDeviceUse: platformName: "..platformName)
	print("lua.checkDeviceUse: deviceType: "..deviceType)
	print("lua.checkDeviceUse: deviceName: "..deviceName)
	return deviceType=="GPU"
end