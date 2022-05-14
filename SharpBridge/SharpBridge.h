#pragma once

#ifdef SHARP_BRIDGE_LIB
#define BRIDGE_EXPORT __declspec(dllexport)
#else
#define BRIDGE_EXPORT __declspec(dllimport)
#endif

#include <string>
#include <functional>

namespace SharpBridge
{
	class BRIDGE_EXPORT Proxy
	{
	private:
		Proxy();

	public:
		static Proxy& Instance();
		~Proxy();

		/**
		 * \brief 设置代理端口
		 * \param port 端口号
		 */
		void SetProxyPort(int port);
		/**
		 * \brief 获取代理端口号
		 * \return 端口号
		 */
		int GetProxyPort();
		/**
		 * \brief 设置是否解密SSL，用于台服
		 */
		void SetEnableTunnel(bool enable);
		/**
		 * \brief 是否解密SSL，用于台服
		 */
		bool GetEnableTunnel();
		/**
		 * \brief 启动代理
		 * \return 是否成功
		 */
		bool Start();
		/**
		 * \brief 停止代理
		 */
		void Stop();
		/**
		 * \brief 获取本地IP地址
		 */
		std::string GetLocalIPAddress();
		/**
		 * \brief 通过uid获取index信息
		 * \param uid uid
		 * \return index
		 */
		std::string GetIndexByUid(const std::string& uid);
		typedef std::function<void(const std::string&)> ReceiveIndexCallback;
		/**
		 * \brief 设置收到index信息时的回调，适用于本地一个人用的情况
		 * \param callback 回调函数
		 */
		ReceiveIndexCallback indexCallback = nullptr;
	};
}
