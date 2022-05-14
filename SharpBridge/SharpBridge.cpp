#include "SharpBridge.h"
#include <msclr\marshal_cppstd.h>
using namespace System;
using namespace msclr::interop;
using namespace System::Runtime::InteropServices;
using namespace Proxy;

static void IndexCallback(String^ index)
{
	if (SharpBridge::Proxy::Instance().indexCallback != nullptr)
		SharpBridge::Proxy::Instance().indexCallback(marshal_as<std::string>(index));
}

SharpBridge::Proxy::Proxy()
{
	ProxySharp::Instance->ReceiveIndexEvent += gcnew ProxySharp::OnReceiveIndex(&IndexCallback);
}

SharpBridge::Proxy& SharpBridge::Proxy::Instance()
{
	static Proxy instance;
	return instance;
}

SharpBridge::Proxy::~Proxy()
{
	Stop();
}

void SharpBridge::Proxy::SetProxyPort(int port)
{
	ProxySharp::Instance->Port = port;
}

int SharpBridge::Proxy::GetProxyPort()
{
	return ProxySharp::Instance->Port;
}

void SharpBridge::Proxy::SetEnableTunnel(bool enable)
{
	ProxySharp::Instance->EnableTunnel = enable;
}

bool SharpBridge::Proxy::GetEnableTunnel()
{
	return ProxySharp::Instance->EnableTunnel;
}

bool SharpBridge::Proxy::Start()
{
	return ProxySharp::Instance->Start();
}

void SharpBridge::Proxy::Stop()
{
	ProxySharp::Instance->Stop();
}

std::string SharpBridge::Proxy::GetLocalIPAddress()
{
	auto address = ProxySharp::Instance->LocalIPAddress;
	return marshal_as<std::string>(address);
}

std::string SharpBridge::Proxy::GetIndexByUid(const std::string& uid)
{
	auto index = ProxySharp::Instance->GetIndexStringByUid(marshal_as<String^>(uid));
	return marshal_as<std::string>(index);
}
