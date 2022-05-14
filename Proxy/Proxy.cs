using System.Collections.Concurrent;
using System.Net;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using System.Text.RegularExpressions;
using System.Timers;
using System.Web;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Serilog;
using Titanium.Web.Proxy;
using Titanium.Web.Proxy.EventArguments;
using Titanium.Web.Proxy.Models;
using Timer = System.Timers.Timer;

namespace Proxy;

public class ProxySharp
{
    /// <summary>
    /// 单例对象
    /// </summary>
    private static readonly ProxySharp _instance = new ProxySharp();

    /// <summary>
    /// 获取单例对象
    /// </summary>
    public static ProxySharp Instance => _instance;

    /// <summary>
    /// 代理服务器
    /// </summary>
    private ProxyServer _proxyServer;

    private ExplicitProxyEndPoint? _endPoint = null;

    private class UserInfo
    {
        public string Uid;
        public string Sign;
        public int timestamp;
        public string Index;

        public UserInfo(string uid = "", string sign = "", int time = 0)
        {
            Uid = uid;
            Sign = sign;
            timestamp = time;
            Index = "";
        }
    }

    /// <summary>
    /// 清理signDict用的定时器
    /// </summary>
    private Timer _signTimer;

    /// <summary>
    /// // 用户数据的dict，key为uid，value为sign,uid和时间戳，定时清理
    /// </summary>
    private ConcurrentDictionary<string, UserInfo> _userInfoDict = new ConcurrentDictionary<string, UserInfo>();
    
    /// <summary>
    /// 本机IP地址
    /// </summary>
    public string LocalIPAddress;

    /// <summary>
    /// 游戏服务器host列表
    /// </summary>
    public static List<string> HostList = new List<string>
            {"sunborngame.com", "ppgame.com", "sn-game.txwy.tw", "girlfrontline.co.kr"};

    /// <summary>
    /// 登录获取uid的url列表
    /// </summary>
    public static List<string> UidList = new List<string>
            {"/Index/getDigitalSkyNbUid", "/Index/getUidTianxiaQueue", "/Index/getUidEnMicaQueue"};

    /// <summary>
    /// 登录信息的url
    /// </summary>
    public static string IndexUrl = "Index/index";

    private bool _enableTunnel = false;

    /// <summary>
    /// 是否解密SSL数据，用于台服
    /// </summary>
    public bool EnableTunnel
    {
        get => _enableTunnel;
        set
        {
            _enableTunnel = value;
            Port = Port;
            Log.Information($"Set EnableTunnel={EnableTunnel}");
        }
    }

    /// <summary>
    /// 构造函数
    /// </summary>
    private ProxySharp()
    {
        _proxyServer = new ProxyServer()
        {
            TcpTimeWaitSeconds = 10,
            ConnectionTimeOutSeconds = 15,
            ReuseSocket = false,
            EnableConnectionPool = false,
            ForwardToUpstreamGateway = false
        };

        _signTimer = new Timer(Defines.SignExpireTime)
        { AutoReset = true, Enabled = true, Interval = Defines.SignExpireTime };
        _signTimer.Elapsed += _signTimerElapsed;
        _signTimer.Start();
        Port = 18888;

        Log.Logger = new LoggerConfiguration()
            .MinimumLevel.Debug()
            .WriteTo.Console()
            .WriteTo.File("Proxy.log")
            .CreateLogger();

        using (var socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, 0))
        {
            socket.Connect(Defines.NetTestIP, 65530);
            LocalIPAddress = socket.LocalEndPoint is IPEndPoint endPoint ? endPoint.Address.ToString() : "";
        }
    }

    private Task ServerCertificateValidationCallback(object sender, CertificateValidationEventArgs e)
    {
        if (e.SslPolicyErrors == SslPolicyErrors.None)
        {
            e.IsValid = true;
        }
        return Task.CompletedTask;
    }

    private Task BeforeTunnelConnectResponse(object sender, TunnelConnectSessionEventArgs e)
    {
        try
        {
            e.DecryptSsl = false;
            var host = e.HttpClient.Request.Host;
            var url = e.HttpClient.Request.Url;

            if (host == null)
                return Task.CompletedTask;
            if (!ShouldProxy(url))
                return Task.CompletedTask;

            e.DecryptSsl = true;
            Log.Debug($"Proxy response {url}");
        }
        catch (Exception err)
        {
            Log.Error($"Error on BeforeTunnelConnectResponse:\n{err}");
        }
        return Task.CompletedTask;
    }

    private Task BeforeTunnelConnectRequest(object sender, TunnelConnectSessionEventArgs e)
    {
        try
        {
            e.DecryptSsl = false;
            var host = e.HttpClient.Request.Host;
            var url = e.HttpClient.Request.Url;

            if (host == null)
                return Task.CompletedTask;
            if (!ShouldProxy(url))
                return Task.CompletedTask;

            e.DecryptSsl = true;
            Log.Debug($"Proxy request {url}");
        }
        catch (Exception err)
        {
            Log.Error($"Error on BeforeTunnelConnectRequest:\n{err}");
        }
        return Task.CompletedTask;
    }

    /// <summary>
    /// 清理到期的sign
    /// </summary>
    private void _signTimerElapsed(object? sender, ElapsedEventArgs e)
    {
        var time = Utils.GetUTC();
        UserInfo tmp;
        foreach (var it in _userInfoDict)
        {
            if (time - it.Value.timestamp > Defines.SignExpireTime)
            {
                _userInfoDict.TryRemove(it.Key, out tmp);
            }
        }
    }

    /// <summary>
    /// 析构函数
    /// </summary>
    ~ProxySharp()
    {
        Stop();
    }

    /// <summary>
    /// 监听端口
    /// </summary>
    public int Port
    {
        get => _endPoint?.Port ?? 0;
        set
        {
            Log.Information($"Change port into {value}");
            var running = _proxyServer.ProxyRunning;
            if (running)
                _proxyServer.Stop();
            if(_endPoint != null)
                _proxyServer.RemoveEndPoint(_endPoint);
            _endPoint = new ExplicitProxyEndPoint(IPAddress.Any, value, EnableTunnel);
            if (EnableTunnel)
            {
                _proxyServer.CertificateManager.SaveFakeCertificates = true;
                _proxyServer.ServerCertificateValidationCallback += ServerCertificateValidationCallback;
                _endPoint.BeforeTunnelConnectRequest += BeforeTunnelConnectRequest;
                _endPoint.BeforeTunnelConnectResponse += BeforeTunnelConnectResponse;
            }
            else
            {
                _proxyServer.CertificateManager.SaveFakeCertificates = false;
                _proxyServer.CertificateManager.RootCertificate = new X509Certificate2();
            }
            _proxyServer.BeforeResponse += BeforeResponse;
            _proxyServer.BeforeRequest += BeforeRequest;
            _endPoint = new ExplicitProxyEndPoint(IPAddress.Any, 18888);
            _endPoint.BeforeTunnelConnectRequest += BeforeTunnelConnectRequest;
            _endPoint.BeforeTunnelConnectResponse += BeforeTunnelConnectResponse;
            _proxyServer.AddEndPoint(_endPoint);
            if (running)
                Start();
        }
    }

    /// <summary>
    /// 是否启用对非游戏请求的禁用
    /// </summary>
    public bool EnableBlocking = false;

    /// <summary>
    /// 不屏蔽的链接，会和url匹配，正则表达式
    /// </summary>
    public string NonBlockingPattern =
        @"(gfcn-(game|passport|transit)\.[\w\.]+\.sunborngame\.com)|(sn-game\.txwy\.tw)|(girlfrontline\.co\.kr)";

    /// <summary>
    /// 启动代理
    /// </summary>
    public bool Start()
    {
        try
        {
            if (!_proxyServer.ProxyRunning)
                _proxyServer.Start();
            return true;
        }
        catch (Exception e)
        {
            return false;
        }
    }

    /// <summary>
    /// 停止代理
    /// </summary>
    public void Stop()
    {
        if (_proxyServer.ProxyRunning)
            _proxyServer.Stop();
        Log.Information($"Stop proxy server");
    }

    /// <summary>
    /// Request前的处理函数，主要用于保存request body
    /// </summary>
    private async Task BeforeRequest(object sender, SessionEventArgs e)
    {
        var host = e.HttpClient.Request.Host;
        var url = e.HttpClient.Request.Url;
        if (host == null)
            return;
        if (ShouldProxy(url))
        {
            // 要先在request里读取body才能保存下来
            var body = await e.GetRequestBody();
            if (body.Length == 0) return;
            e.HttpClient.Request.KeepBody = true;
            Log.Debug($"Proxy request {url}");
            return;
        }

        // 心跳包
        if (url.EndsWith("/Index/heartBeat"))
        {
            var body = await e.GetRequestBodyAsString();
            if (body.Length == 0) return;
            var uid = GetUidFromRequest(body);
            if (_userInfoDict.ContainsKey(uid))
            {
                var tmp = _userInfoDict[uid];
                tmp.timestamp = Utils.GetUTC();
                _userInfoDict[uid] = tmp;
            }

            return;
        }

        // 此时是没有匹配
        if (EnableBlocking && !Regex.IsMatch(url, NonBlockingPattern))
        {
            e.Ok("Blocked!");
        }
    }

    /// <summary>
    /// Response处理函数
    /// </summary>
    private async Task BeforeResponse(object sender, SessionEventArgs e)
    {
        try
        {
            var host = e.HttpClient.Request.Host;
            var url = e.HttpClient.Request.Url;
            Console.WriteLine(url);
            if (host == null)
                return;
            if (!ShouldProxy(url))
                return;
            Log.Debug($"Proxy response {url}");
            
            var requestString = await e.GetRequestBodyAsString();
            var responseString = await e.GetResponseBodyAsString();

            if (UidList.Any(it => url.EndsWith(it)))
            {
                GetUid(requestString, responseString);
                return;
            }

            if (url.EndsWith("Index/index"))
            {
                GetIndex(requestString, responseString);
                return;
            }
        }
        catch (Exception err)
        {
            Log.Error($"Error while process response {e.HttpClient.Request.Url}");
        }
    }

    private bool ShouldProxy(string url)
    {
        if (HostList.All(it => !url.Contains(it)))
            return false; // 不在host列表里
        if (!UidList.Any(it => url.EndsWith(it)) && !url.EndsWith(IndexUrl))
            return false; // 不在要处理的列表里
        return true;
    }

    /// <summary>
    /// 解析sign信息并将其保存到dict中
    /// </summary>
    private void GetUid(string request, string response)
    {
        if (!response.StartsWith("#"))
            return;
        var data = Cipher.DecodeDefault(response);
        if (data == "")
            return;
        var obj = (JObject)JsonConvert.DeserializeObject(data);
        if (obj == null || !obj.ContainsKey(Defines.Sign) || !obj.ContainsKey(Defines.Uid))
            return;
        var userInfo = new UserInfo(obj[Defines.Uid].Value<string>(),
            obj[Defines.Sign].Value<string>(), Utils.GetUTC());
        _userInfoDict[userInfo.Uid] = userInfo;
        Log.Information($"Get user uid: {userInfo.Uid}");
    }

    /// <summary>
    /// 从request中获取uid
    /// </summary>
    private string? GetUidFromRequest(string request)
    {
        var parsedReq = HttpUtility.ParseQueryString(request);
        var uid = parsedReq[Defines.Uid];
        return uid;
    }

    /// <summary>
    /// 从request中获取uid并进一步获取UserInfo
    /// </summary>
    private UserInfo? GetUserInfo(string request)
    {
        var uid = GetUidFromRequest(request);
        if (uid == null || !_userInfoDict.ContainsKey(uid))
            return null;
        var user = _userInfoDict[uid];
        return user;
    }

    /// <summary>
    /// 更新用户信息的时间戳然后更新到dict中
    /// </summary>
    private void UpdateUserInfo(UserInfo user)
    {
        user.timestamp = Utils.GetUTC();
        _userInfoDict[user.Uid] = user;
    }
    
    /// <summary>
    /// 解析index游戏初始化数据
    /// </summary>
    private void GetIndex(string request, string response)
    {
        var user = GetUserInfo(request);
        if (user == null)
            return;
        var data = Cipher.Decode(response, user.Sign);
        if (string.IsNullOrEmpty(data))
            return;

        UpdateUserInfo(user);
        user.Index = data;
        Log.Information($"Get user index: {user.Uid}");
        File.WriteAllText($"userinfo-{user.Index}.json", data);
        ReceiveIndexEvent?.Invoke(data);
    }

    public string GetIndexStringByUid(string Uid)
    {
        if (!_userInfoDict.ContainsKey(Uid))
            return "";
        var user = _userInfoDict[Uid];
        return user.Index;
    }

    public delegate void OnReceiveIndex(string index);
    public event OnReceiveIndex ReceiveIndexEvent;
}