namespace Proxy;

public class Defines
{
    /// <summary>
    /// sign过期时间，30分钟
    /// </summary>
    public const int SignExpireTime = 30 * 60 * 1000;

    /// <summary>
    /// Request中的数据key
    /// </summary>
    public const string OutDataKey = "outdatacode";

    /// <summary>
    /// 用于测试网络的公网IP
    /// </summary>
    public const string NetTestIP = "114.114.114.114";

    /// <summary>
    /// 代表uid的key字符串
    /// </summary>
    public const string Uid = "uid";

    /// <summary>
    /// 代表sign的key字符串
    /// </summary>
    public const string Sign = "sign";

    /// <summary>
    /// 采集的数据类型
    /// </summary>
    public class Type
    {
        /// <summary>
        /// 造枪
        /// </summary>
        public const string Gun = "Gun";

        /// <summary>
        /// 战役
        /// </summary>
        public const string Mission = "Mission";

        /// <summary>
        /// 造装备
        /// </summary>
        public const string Equip = "Equip";
    }

    /// <summary>
    /// 搜救妖精id
    /// </summary>
    public const int SearchFairyID = 16;

    /// <summary>
    /// 使用的妖精的技能等级
    /// </summary>
    public const string FairySkillLvKey = "fairy_skill_lv";

    /// <summary>
    /// 使用的妖精的id
    /// </summary>
    public const string UseFairyIdKey = "use_fairy_id";
}