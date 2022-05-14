using System.Runtime.InteropServices;

namespace Proxy;

public class Cipher
{
    private const string Dll = "GF_Decoder.dll";
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr gf_decode_str([MarshalAs(UnmanagedType.LPStr)] string src, [MarshalAs(UnmanagedType.LPStr)] string key, bool withGZip = true);
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr gf_decode_default([MarshalAs(UnmanagedType.LPStr)] string src);
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr gf_encode_str([MarshalAs(UnmanagedType.LPStr)] string src, [MarshalAs(UnmanagedType.LPStr)] string key, bool withGZip = true);
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr gf_encode_default([MarshalAs(UnmanagedType.LPStr)] string src);
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void gf_free_str(IntPtr str);

    private static string GetStringAndFree(IntPtr ptr)
    {
        var result = Marshal.PtrToStringAnsi(ptr);
        gf_free_str(ptr);
        return result ?? "";
    }

    public static string Decode(string src, string sign, bool withGZip = true)
    {
        var ptr = gf_decode_str(src, sign, withGZip);
        return GetStringAndFree(ptr);
    }

    public static string DecodeDefault(string src)
    {
        var ptr = gf_decode_default(src);
        return GetStringAndFree(ptr);
    }

    public static string Encode(string src, string sign, bool withGZip = true)
    {
        var ptr = gf_encode_str(src, sign, withGZip);
        return GetStringAndFree(ptr);
    }

    public static string EncodeDefault(string src)
    {
        var ptr = gf_encode_default(src);
        return GetStringAndFree(ptr);
    }
}