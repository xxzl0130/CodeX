namespace Proxy;

public class Utils
{
    /// <summary>
    /// 获取UTC时间戳
    /// </summary>
    /// <returns>UTC时间戳</returns>
    public static int GetUTC()
    {
        return (int)(DateTime.Now - new DateTime(1970, 1, 1)).TotalSeconds;
    }

    /// <summary>
    /// 为Base64字符串添加末尾的补足"="
    /// </summary>
    /// <param name="s">原始Base64字符</param>
    /// <returns>补足"="后的字符串</returns>
    public static string AddB64Padding(string s)
    {
        var paddingLen = (s.Length / 4 * 4 + 4 - s.Length) % 4;
        var paddingString = "";
        for (var i = 0; i < paddingLen; ++i)
            paddingString += "=";
        return s + paddingString;
    }
}