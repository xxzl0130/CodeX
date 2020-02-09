# CodeX
少女前线芯片计算器CodeX  
[使用说明](./doc/使用说明.md)  
转载请注明出处，禁止用于商业。  
![GitHub](https://img.shields.io/github/license/xxzl0130/CodeX) ![GitHub release (latest by date)](https://img.shields.io/github/v/release/xxzl0130/CodeX) ![GitHub All Releases](https://img.shields.io/github/downloads/xxzl0130/CodeX/total)  

## 第一版
目前功能：  
* 从本地代理/网络代理获取芯片数据  
* 配置各小队参数  
* 为各小队配置不同的芯片组合方案（例如用不用5格）  
* 按格数筛选方案  
* 在方案列表中直接对各属性排序  
* Top n
* 计算并显示当前各小队的满强化属性 

TODO：
- [ ] 保存为各小队选择的配置  
- [ ] 在计算中排除已经选择的配置中的芯片  

## 依赖
[zlib](https://github.com/madler/zlib)  
[gzip-hpp](https://github.com/mapbox/gzip-hpp)  
[openssl](https://github.com/openssl/openssl)  

芯片图片素材来自[GFTool](https://github.com/hycdes/GFTool)  
芯片解法数据来自[GFChipToolX](https://github.com/xxzl0130/GFChipToolX)  
