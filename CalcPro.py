###############################################
#                Calc Pro更新日志
# 17.12.17 8.00-11.00 普通计算
#          12.00-14.40变量定义、函数定义
#          15.40-18.00函数使用及debug
# 17.12.19 21.00-21.30解方程
#                     正负号支持
#               -23.30内置函数支持
# 18.1.31  加入帮助文档
# 18.2.3   All Day    加入颜色模块
#                     plot,sum,...
# 18.2.4   9.04-10.20 将函数值存储方式改为dict
#               10.40 按 含/不含自变量 双缓存
# 18.2.10  15.38-16:50分数和根式输出
#                     增加递归函数标记
# 18.2.22             bug修复,plot |
#                   To Do
#
#               版本号1.3.1
#
###############################################
import re
import math as m
import random
from time import time
import matplotlib.pyplot as plt
from os import system
####################颜色模块#######################
# Fore: BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, RESET.  
# Back: BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, RESET.  
# Style: DIM, NORMAL, BRIGHT, RESET_ALL  
from colorama import  init, Fore, Style  
init(autoreset=True)
def color(c_style,st):
    return c_style+st+Fore.RESET
def printc(st):
    print(Style.BRIGHT+st+Fore.RESET)
re_alp=re.compile(r"^[a-zA-Z]+'?|'$")
re_num=re.compile(r'^[\+\-]?[0-9\.]+e?[\+\-]?[0-9\.]*$')
re_num2=re.compile(r'^[\+\-]?[0-9]+')
colorset=['#005980','#00B1FF','#4479FF','#2E912D','#4A33E8','#B38CFF','#B0433A','#3390A8','#D9283E','#540032','#46BC9F','#820333']
plot_file='_plot.png'
switches_init={'time':False,'debug':False,'vline':True,'hline':False,'fraction':True,'sub':False}
settings_init={'bound':1e5,'precision':1e-9,'iteration':4000,'cab':200}
switches,settings=switches_init.copy(),settings_init.copy()
input_count=0
last_warning=''#防止warning刷屏
var_def_init={'pi':m.pi,'e':m.e}
func_def_init={'out':[1,[],[],[{},{}]]}#变量个数，[变量列表]，[[函数定义]]，部分定义[{不带自变量:[]},{带自变量:[]}]
var_def,func_def=var_def_init.copy(),func_def_init.copy()
func_ite=set()#标记递归定义的函数
func_in={'int':int,'sqrt':m.sqrt,'ln':m.log,'lg':m.log10,'abs':m.fabs,'sin':m.sin,'cos':m.cos,'tan':m.tan,'sinh':m.sinh,'cosh':m.cosh,'tanh':m.tanh,'arcsin':m.asin,'arccos':m.acos,'arctan':m.atan,'gamma':m.gamma,'erf':m.erf}
func_in2={'max':max,'min':min,'log':lambda x,y:m.log(y,x),'C':lambda x,y:m.gamma(x+1)/m.gamma(x-y+1)/m.gamma(y+1),'A':lambda x,y:m.gamma(x+1)/m.gamma(x-y+1),'randn':random.gauss}
primes=[2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973]
#10000以内的质数表1229个质数
help_document=r'''
            Calc Pro by John Ao 
            ~~~~~~~~~~~~~~~~~~~ 
本程序支持实数域表达式运算、变量和函数定义、方程求解， 
函数列表、求和、求积、积分、求导、绘图，分解质因数等功能， 
多行输入可用半角分号';'隔开。 

$c$·表达式运算 $e$ 

    ·忽略空白符 

    ·按 Ctrl+C 中断操作或放弃当前输入行 

    ·支持一元运算符 + - 和二元运算符 + - * / \ % ^ 

    ·数字和常数、变量、函数之间的乘号可以省略 

    ·支持科学计数法，如 1e3, 1e-3，但$y$1e+3=1 * e + 3$e$  

    ·含小数点或用科学计数法表示的数视为$y$float$e$，否则视为$y$int$e$ 

    ·内置基础函数 
        ·一元函数 '''+','.join([x for x in func_in])+' \n        ·二元函数 '+','.join([x for x in func_in2])+r''' 

    ·内置函数out(n)存储第n次输出的结果（n可以为负） 

    ·内置常量有 pi,e 

    $g$示例： 
    1) 3+4*2/(1-5)^2^3-C(5,2)max(-pi,e)+sin'(2pi) 
    
$c$·变量和函数的定义及使用 $e$ 

    ·变量名、函数名和函数的自变量名$y$只能是a-z和A-Z的组合$e$ 

    ·$y$变量名和函数名不能相同$e$ 

    ·用'='符号表示定义，若为变量定义，'='后的表达式会被$y$立即计算并赋值$e$给变量 

    ·递归函数的初值定义方式与变量定义相同 

    ·变量或函数可参与表达式运算 

    ·变量rnd为[0,1)上$y$一致分布$e$的随机数 
     获取服从$y$正态分布$e$N(μ,σ²)的随机数使用函数randn(μ,σ²) 

    ·清除某一定义输入 $clean 变量名或函数名 

    ·清除所有定义输入 $clean 

    $g$示例： 
    1) abc=1+2/3; 4+abc 
       定义变量abc并使用 

    2) f(ab,bc)=(ab+bc)/2; f(3,4) 
       定义二元函数f(ab,bc)并使用 

    3) $clean g;g(x)=g(x-1)+g(x-2); g(1)=1; g(2)=1; g(2000) 
       清除g的定义，重新定义g(x)为Fibonacci数列并求第2000项 
    
$c$·方程求解 - 牛顿法解方程 $e$ 

    ·(以x0为初值)求解形如 f(x)=A 的问题 

     $solve f A x0 

    ·x0为可选参数 

    ·$y$默认精度为1e-9$e$，修改精度使用命令 
     $precision 数值 

    ·$y$默认最大迭代次数为4000$e$，修改最大迭代次数使用命令 
     $iteration 数值 

    $g$示例： 
    1) g(y)=ln(y); $solve g 1 2.7 
       以2.7为初值求解方程ln(y)=1 
       
    2) f(x)=max(sin(x/2),x); $solve f -1 0.4 
       以0.4为初值求解方程max(sin(x/2),x)=-1 

$c$·函数列表、求和、求积 $e$ 

    ·从a到b以步长step列表、求和、求积$y$（省略step则默认为1）$e$ 
        ·$list f a b step 
        ·$sum f a b step 
        ·$product f a b step 

    ·绘制函数求和、求积的折线图 
        ·$sumplot f a b step 
        ·$productplot f a b step 

    ·同时对多个函数列表用'+'或','连接 

    $g$示例： 
    1) f(x)=1-1/(2x)^2;$productplot f 1 1000;2/out(-1) 
        ∞               2 
        ∏ (1-1/(2*x)^2)=— 
        x=1              π 
       绘制f(x)从1到1000的部分积图像并近似计算π的值 

    2) f(x)=2^x;g(x)=x^2;$list f+g 0 5 
       从0到5对f(x)和g(x)列表 

    3) g(x)=-(-1)^x/(2x-1);$sumplot g 1 100;ln(2)-out(-1) 
        ∞ 
        ∑ -(-1)^x/(2*x-1)=ln(2) 
        x=1 
       绘制g(x)从1到100的部分和图像并近似计算ln2的值 

$c$·函数积分 $e$ 
    ·对一元函数f(x)，从a到b积分，取p份分割$y$（省略p则默认为1000）$e$ 
     $integrate f a b p 

$c$·函数求导 $e$ 
    ·求一元函数f(x)在a处的近似导数，使用f'(a) 
    $g$示例: 
    1) sin'(pi/6) 

$c$·函数绘图 $e$ 

    ·对一元函数f(x)，从a到b以步长step取样绘图（默认a b间均匀取样）并将图像文件保存在程序同目录下 
        ·$plot f a b step    折线图 
        ·$plot .f a b step   散点图 
        ·$plot *f a b step   积分图 
        ·$plot f' a b step   导数图 
        ·$plot #f a b step   填充f与x轴间的部分 
        ·上述开关在不冲突情况下可组合使用 

    ·在同一图中画多个函数用'+'或','连接 
     $plot f+g+h a b step 

    ·参数方程 x=f(t),y=g(t) 用 f&g 表示 
     $plot f&g a b step 

    ·参数方程 x=f(t),y=g(t,x) 用 f|g 表示 
     $plot f|g a b step 

    ·$y$函数值超过bound的点舍去$e$（默认为1e3）, 重新指定bound命令为 
     $bound 数值 

    ·输入 $vline 开启/关闭水平切线显示 
    ·输入 $hline 开启/关闭竖直切线显示 

    ·极坐标绘图使用$polar代替$plot 

    ·函数密度图 
        ·绘制当x∈[a,b]时函数y=f(x)的值在区间[y1,y2]上的概率密度分布图 
         $density f a b y1 y2 step 

        ·绘图开关 . * ' # 均适用 

        ·区间[y1,y2]$y$默认分为500个取样子区间$e$，修改默认值使用命令 
         $cab 数值 

    $g$示例： 
    1) f(x)=x^2;$plot f -1 1 
       绘制f(x)=x²在[-1,1]上的图像 

    2) g(1)=1;g(2)=1;g(x)=g(x-1)+g(x-2);g(1)=1;g(2)=1;$plot g+.g 1 20 1 
       绘制g(x)在[1,20]上的图像 

    3) $bound 6;$plot gamma+gamma' -1 5 1e-2 
       绘制gamma(x)及其导数在[-1,5]上的图像 

    4) $plot sin+.cos*+.sin&cos 0 2*pi 
       在[0,2π]上绘制sin(x)的折线图、cos的积分的散点图、参数方程x=sin(t)&y=cos(t)的散点图 

    5) f(x)=1/x;$plot ln'+.f 0 10 
       在[0,10]上绘制ln'(x)的折线图和1/x的散点图 

    6) $bound 1e7;f(x)=e^x;$plot f -5 20 
       调整绝对值上界为1e7，绘制e^x在[-5,20]上的图像 

    7) f(x)=1/x;g(x)=x*ln(x);$bound 5;$plot f+g 0 3 1e-2 

    8) f(x)=1-sin(x);$polar sin+cos+f 0 2pi 

    9) f(x)=randn(0,1);g(y,x)=rnd*h(x);h(x)=e^(-x^2/2)/sqrt(2pi);$plot .f|g+#h -3 3 2e-3 
       在正态分布曲线N(0,1)下方随机取点 

    10) f(x)=3x^2;$bound 5;$density .f+*f+f' 0 1 0 1 

$c$·分解质因数 $e$ 
    （为保证计算速度，$y$取10000以内的质数表进行求解$e$） 
    ·求p的质因数分解： 
     $factor p 

$c$·运算计时  $e$ 

    ·输入 $time 开启/关闭计时功能 

$c$·分数输出 $e$ 

    ·输入 $fraction 或 $. 开启/关闭按分数格式输出功能 
    $y$注意：由于精度限制，在一些极端情况下结果可能不准确$y$ 

    $g$示例： 
    1) $fraction$;sqrt(4/5)pi;arctan(1) 

$c$·开关和设定 $e$ 

    ·开关有 time vline hline fraction，以time为例 
     输入 $time       开启/关闭time 
     输入 $time$      开启time 
     输入 $time$$     关闭time 

    ·设定有 bound precision iteration，以bound为例 
     输入 $bound      显示当前bound值 
     输入 $bound$     重置bound值 
     输入 $bound 1e3  设置bound值为1e3 

    ·输入 $reset      重置所有开关和设定 
    '''

# $c$·递归函数优化* $e$ 
#     ·为避免递归函数计算溢出，使用 $sub 指令开启/关闭全局强制分解优化， 
#      即将问题f(1000)分解为f(100),f(200),...,f(1000) 
#      同时此开关也会影响非递归函数的计算。 
#     ·默认值为False，一般无需修改。 
help_document=help_document.replace('$c$',Fore.CYAN).replace('$e$',Fore.RESET).replace('$g$',Fore.GREEN).replace('$y$',' '+Fore.YELLOW)
def main():
    global var_def,func_def,input_count,switches,settings,last_warning
    printc(Fore.CYAN+random_ico())
    printc('************************************\n'+
          '*     Calc Pro by John Ao v1.3     *\n'+
          '*        查看帮助请输入 $help      *\n'+
Style.DIM+'*       请注意切换至英文输入法     *\n'+Style.BRIGHT+
          '************************************\n')
    while True: #交互模式if改为while
        # try:     #^C中断
        #     e_in=input('In:=')
        # except:
        #     print()
        #     continue
        #e_in=r'$time;g(1)=1;g(2)=1;g(x)=g(x-1)+g(x-2);g(400)'
        #e_in=r'g(1)=1;g(2)=1;g(x)=g(x-1)+g(x-2);$list g 1 10'
        e_in=r"g(x)=g(x);g(x)=x+1;g(1)=1"
        while e_in and e_in[0]==' ':
            e_in=e_in[1:]
        for exp_in in [x for x in re.split(r'\s*;\s*',e_in) if x]:
            input_count+=1
            last_warning=''
            time_s=time()
            try:
                if re.match(r'^\s*\$',exp_in):
                    exp_in=intellispell(exp_in.replace('$.','$fraction').replace(',','+'))
                    printc(Fore.GREEN+'In[%d]:='%input_count+Fore.WHITE+exp_in)
                    #预留特殊指令
                    exp_spec=[x for x in re.split(r'\s+',exp_in) if x]
                    cmd=exp_spec[0].lower()[1:]
                    exp_spec[0:1]=[]
                    if cmd=='solve':
                        exp_re=equ_solve(exp_spec)
                    elif cmd=='sum':
                        exp_re=func_sum(exp_spec)
                    elif cmd=='sumplot':
                        exp_re=func_sumplot(exp_spec)
                    elif cmd=='list':
                        exp_re=func_list(exp_spec)
                    elif cmd=='product':
                        exp_re=func_mlt(exp_spec)
                    elif cmd=='productplot':
                        exp_re=func_mltplot(exp_spec)
                    elif cmd=='plot':
                        exp_re=func_plot(exp_spec)
                    elif cmd=='density':
                        exp_re=func_density(exp_spec)
                    elif cmd=='integrate':
                        exp_re=func_integ(exp_spec)
                    elif cmd=='polar':
                        exp_re=func_plot(exp_spec,polar=True)
                    elif cmd=='factor':
                        exp_re=factor_gui(exp_spec)
                    elif cmd=='clean':
                        if exp_spec and s_type(exp_spec[0])=='alp':
                            if (not exp_spec[0] in dict(func_in,**func_in2)) and exp_spec[0] in func_def:
                                func_def.pop(exp_spec[0])
                                func_ite.discard(exp_spec[0])
                            if exp_spec[0] in var_def:
                                var_def.pop(exp_spec[0])
                        else:
                            var_def=var_def_init.copy()
                            func_def=func_def_init.copy()
                        exp_re=''
                    elif cmd=='reset':
                        switches=switches_init.copy()
                        settings=settings_init.copy()
                        exp_re=''
                    elif cmd in switches:
                        switches[cmd]=not switches[cmd]
                        exp_re=cmd.capitalize()+(' on' if switches[cmd] else ' off')
                    elif cmd[:-1] in switches:
                        cmd=cmd[:-1]
                        switches[cmd]=True
                        exp_re=cmd.capitalize()+' on'
                    elif cmd[:-2] in switches:
                        cmd=cmd[:-2]
                        switches[cmd]=False
                        exp_re=cmd.capitalize()+' off'
                    elif cmd in settings:
                        if exp_spec:
                            exp_re=calc_(exp_spec[0])
                            if not isinstance(exp_re,str):
                                settings[cmd]=exp_re
                        else:
                            exp_re=settings[cmd]
                    elif cmd[:-1] in settings:
                        cmd=cmd[:-1]
                        settings[cmd]=settings_init[cmd]
                        exp_re=settings[cmd]
                    else: #help或者其他
                        exp_re=help_document
                else:
                    exp_in=intellispell(re.sub(r'\s+','',exp_in))
                    printc(Fore.GREEN+'In[%d]:='%input_count+Fore.WHITE+exp_in)
                    exp_arr=to_arr(exp_in)
                    if '=' in exp_arr:
                        equ_pos=exp_arr.index('=')
                        if re.match(r'^[a-zA-Z]+\=',exp_in):
                            assert not exp_arr[0] in dict(func_def,**func_in,**func_in2)
                            exp_re=cal(exp_arr[2:])
                            if not isinstance(exp_re,str):
                                var_def[exp_arr[0]]=exp_re
                        elif re.match(r'^\w+\([\w,]*\w+\)\=',exp_in):
                            f=exp_arr[0]
                            f_var=exp_arr[2:equ_pos-1:2]
                            f_isdef=False #是否为部分定义
                            f_exp=exp_arr[equ_pos+1:]
                            f_var_allnum=True#是否全为数值
                            for i in f_var:
                                if s_type(i)=='num':
                                    f_isdef=True
                                else:
                                    f_var_allnum=False
                            if f in func_in:
                                exp_re='Error - Function [%s] Defined Inside!\n'%f+Fore.CYAN+'List of inside functions:\n%s'%','.join([x for x in func_in]+[x for x in func_in2])
                            elif f=='out':
                                exp_re='Error - Function [out] is reserved!\n'+Fore.CYAN+'->Try $help for details.'
                            else:
                                exp_re=''
                                if f in func_def:
                                    if f_isdef:
                                        if not f in func_ite:
                                            exp_re='Error - Invalid for non-iteration function [%s]'%f
                                        else:
                                            if f_var_allnum:
                                                func_def[f][3][0][to_str(f_var)]=f_exp
                                            else:
                                                func_def[f][3][1][to_str(f_var)]=f_exp
                                    else:
                                        if f_exp!=func_def[f][2]!=[]:#如果前后表达式不同，清除已有部分定义并重新设置递归标记
                                            func_def[f]=[equ_pos/2-1,f_var,f_exp,[{},{}]]
                                        else:
                                            func_def[f]=[equ_pos/2-1,f_var,f_exp,func_def[f][3]]
                                        func_ite.discard(f)
                                        for f_ite in func_ite:
                                            if f_ite in f_exp:
                                                func_ite.add(f)
                                                break
                                        else:
                                            if f in f_exp:
                                                func_ite.add(f)
                                else:
                                    if f_isdef:
                                        if f_var_allnum:
                                            func_def[f]=[equ_pos/2-1,[],[],[{to_str(f_var):f_exp},{}]]
                                        else:
                                            func_def[f]=[equ_pos/2-1,[],[],[{},{to_str(f_var):f_exp}]]
                                    else:
                                        func_def[f]=[equ_pos/2-1,f_var,f_exp,[{},{}]]
                                        func_ite.discard(f)
                                        for f_ite in func_ite:
                                            if f_ite in f_exp:
                                                func_ite.add(f)
                                                break
                                        else:
                                            if f in f_exp:
                                                func_ite.add(f)
                                #exp_re=func_def[f]
                                if not exp_re:
                                    exp_re='Function [%s] Defined.'%f
                        else:
                            exp_re='Error - Unexpected [=]'
                    else:
                        func_sub=switches['sub']
                        if re.match(r'^[a-zA-Z]+\([0-9\.]+e?[\+\-]?[0-9\.]*\)$',exp_in):
                            func_sub=switches['sub']
                            f=exp_arr[0]
                            func_sub|=f in func_ite
                            func_sub&=not (f in func_in or f in func_in2)
                            if func_sub:
                                x=to_num(exp_arr[2])
                                if is_int(x):
                                    if x>100:
                                        i=1
                                        while i*100<x:
                                            exp_arr[2]=str(int(i*100))
                                            cal(exp_arr)
                                            i+=1
                                        exp_arr[2]=str(x)
                        exp_re=cal(exp_arr)
            except BaseException as e:
                if not isinstance(e,IndexError):exp_re='Error - Unable to Evaluate\n'+Fore.CYAN+'->Maybe try $help'
                if switches['debug']:print(e)
            finally:
                exp_re_=exp_re
                exp_re=str(exp_re)
                if exp_re:
                    if s_type(exp_re)=='num':
                        func_def['out'][3][0][str(input_count)]=[exp_re]
                        if switches['fraction']:
                            exp_re=str(pretty_print(exp_re_))
                    if 'Error' in exp_re:
                        exp_re=Fore.RED+exp_re
                    elif 'Warning' in exp_re:
                        exp_re=Fore.YELLOW+exp_re
                    else:
                        exp_re=Fore.WHITE+str(exp_re)
                    printc(Fore.GREEN+'Out[%d]='%input_count+exp_re+' ')
                if switches['time']:
                    time_used=time()-time_s
                    print(color(Fore.YELLOW,'Time used: %.3fs '%time_used))
                print()
        print()

def s_type(s):
    #判定s的类型
    if re_alp.match(s):
        return 'alp'
    elif s in r'+-*/^%\(),=~`':
        return 'sym'
    elif re_num.match(s):
        return 'num'
    else:
        return 'unknown'
def intellispell(st):
    m=re.findall(r'\de[\-0]*\d',st)
    if m:
        for ma in m:
            st=st.replace(ma,ma.replace('e','@'))
    m=re.findall(r'\d[a-zA-Z]|[a-zA-Z]\d|\)[a-zA-Z\d]|\)\(',st)
    if m:
        for ma in m:
            st=st.replace(ma,ma[0]+'*'+ma[1])
    return st.replace('@','e')
def calc_(st):
    return cal(to_arr(st))
def to_arr(st):
    #替换正负号
    st=re.sub(r'(?<![\w\)])\+','~',st)
    st=re.sub(r'(?<![\w\)])\-','`',st)
    #将st拆分为数组
    last=''
    pos=-1
    arr=[]
    last_type='non'
    for j in range(len(st)):
        i=st[j]
        pos+=1
        t=s_type(i)
        if t=='unknown':
            arr.append(last)
            last=''
            last_type='non'
            continue
        if i=='e' and s_type(st[j-1])=='num' and re_num2.match(st[j+1:]):
            t='sci' #处理科学计数法
        elif (t=='num' or i in '+-') and last_type=='sci':
            t='sci'
        else:
            if (not((last_type=='non') or (t==last_type))) or (t=='sym'):
                arr.append(last)
                last=''
        last+=i
        last_type=t
    arr=list(filter(lambda x:x!='',arr+[last]))
    return arr
def cal(arr):
    #======================转化为前缀========================
    nums=[]
    syms=[]
    pn=[]#正负号
    para_c=0
    global last_warning
    prio={'(':-1,')':-2,',':-3,'+':1,'-':1,'~':1,'`':1,'*':2,'/':2,'%':2,'\\':2,'^':3}
    for i in arr:
        if i=='rnd' or i[1:]=='rnd':
            i=i.replace('rnd',str(random.random()))
        t=s_type(i)
        if t=='num':
            nums.append(i)
        elif t=='alp':
            if i in var_def:
                nums.append(str(var_def[i]))
            elif i in func_def or i in func_in or i in func_in2:
                syms.append(i)
            elif i[:-1] in func_def or i[:-1] in func_in or i[:-1] in func_in2:
                syms.append(i)
            else:
                now_warning='Warning - [%s] Undefined, taken as 0'%i
                if now_warning!=last_warning:
                    printc(Fore.YELLOW+now_warning)
                    last_warning=now_warning
                nums.append('0')
        elif i in '~`':
            syms.append(i)
        elif t=='sym':
            p=prio[i]
            if p==-1:
                syms.append(i)
                para_c+=1
            elif p==-2:
                para_c-=1
                if para_c<0:
                    return 'Error - Unmatched Parathesis'
                s=syms[-1]
                while not s=='(':
                    nums.append(syms.pop())
                    s=syms[-1]
                syms.pop()
                if len(syms)>0:
                    s=syms[-1]
                    if s_type(s)=='alp':
                        nums.append(syms.pop())
            elif p==-3:
                s=syms[-1]
                while not s=='(':
                    nums.append(syms.pop())
                    s=syms[-1]
            else:
                while len(syms)>0:
                    s=syms[-1]
                    if ((prio[s]>p) and (p==3)) or ((prio[s]>=p) and (p!=3)):
                        nums.append(s)
                        syms.pop()
                    else:
                        break
                syms.append(i)
    if syms:
        nums+=syms[::-1]
    #=======================计算===========================
    re_num=[]
    for k in nums:
        t=s_type(k)
        if t=='num':
            tmp=to_num(k)
            if isinstance(tmp,str):
                return tmp
            else:
                re_num.append(tmp)
        elif k in '~`':
            if len(re_num)<1:
                return 'Error - Insufficient numbers [1]'
            if k=='`':
                re_num[-1]=-re_num[-1]
        elif t=='sym':
            if len(re_num)<2:
                return 'Error - Insufficient numbers [1]'
            b=re_num.pop()
            a=re_num.pop()
            if k=='+':
                c=a+b
            elif k=='-':
                c=a-b
            elif k=='*':
                c=a*b
            elif k=='/':
                c=a/b
            elif k=='%':
                c=a%b
            elif k=='^':
                c=m.pow(a,b)
            elif k=='\\':
                c=(a-a%b)/b
            re_num.append(c)
        elif t=='alp':
            if k[-1]=="'":#求导数
                if len(re_num)<1:
                    return 'Error - Insufficient numbers'
                x=re_num.pop()
                err=1e-10
                f_re=calc_('{0}({1}+{2})-{0}({1})'.format(k[:-1],x,err))
                assert not isinstance(f_re,str)
                f_re/=err
            else:
                if k in func_in:#内置函数
                    if len(re_num)<1:
                        return 'Error - Insufficient numbers'
                    f_re=func_in[k](re_num.pop())
                elif k in func_in2:
                    if len(re_num)<2:
                        return 'Error - Insufficient numbers'
                    y,x=re_num.pop(),re_num.pop()
                    f_re=func_in2[k](x,y)
                else:
                    f_list=func_def[k]#一定在func_def中，不然已经当做变量处理
                    f_def={}#把定义中的相关变量替换为数值
                    f_exp=[]
                    f_len=int(f_list[0])
                    f_match=[]
                    f_matchexp=[]
                    if len(re_num)<f_len:
                        return 'Error - Insufficient numbers [2]'
                    f_num=re_num[-f_len:]
                    #寻找已有[部分定义]
                    if k=='out':#如果是out()函数，允许负标号
                        if f_num[0]<0:
                            f_num[0]=int(input_count+f_num[0])
                    f_num_str=to_str(f_num)
                    if (k in func_ite or k=='out') and f_num_str in f_list[3][0]:
                        f_match=f_num_str.split(',')
                        f_matchexp=f_list[3][0][f_num_str]
                    else:
                        for key in f_list[3][1]:
                            matched=True
                            i=key.split(',')
                            for j in range(f_len):
                                if not((s_type(i[j])=='alp') or (i[j]==str(f_num[j]))):
                                    matched=False
                            if matched:
                                f_match=i
                                f_matchexp=f_list[3][1][key]
                    if f_match==[]:#如果没有找到[部分定义]则使用[定义]
                        f_match=f_list[1]
                        f_matchexp=f_list[2]
                    assert f_match!=[]
                    for i in range(f_len):
                        f_var=f_match[-1-i]
                        p=re_num.pop()
                        if s_type(f_var)=='alp':
                            f_def[f_var]=p
                    for i in f_matchexp:
                        if (s_type(i)=='alp') and (i in f_def):
                            i=str(f_def[i])
                        f_exp.append(i)
                    f_re=cal(f_exp)
                    if isinstance(f_re,str):
                        return f_re
                    if k in func_ite:
                        func_def[k][3][0][','.join(map(str,f_num))]=[str(f_re)]#如果递归则添加[部分定义]
            re_num.append(f_re)
    if len(re_num)!=1:
        return 'Error - Too many numbers'
    return re_num[0]

def to_num(i):
    if '.' in i or 'e' in i:
        try:
            i=float(i)
        except BaseException:
            return 'Error - [%s] is not of type float'%i
    else:
        try:
            i=int(i)
        except BaseException:
            return 'Error - [%s] is not of type int'%i
    return i

def equ_solve(arr):
    f=arr[0]
    c=calc_(arr[1])
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    equ_delta=equ_preci=m.fabs(settings['precision'])
    equ_ite=settings['iteration']
    x__=2.71828e-3
    if len(arr)==3:
        x__=float(calc_(arr[2]))
    for i in range(equ_ite):
        #最大迭代次数
        y=cal([f,'(',str(x__),')'])
        y_=cal([f,'(',str(x__+equ_delta),')'])
        y__=cal([f,'(',str(x__-equ_delta),')'])
        if isinstance(y,str) or isinstance(y_,str) or isinstance(y__,str) or (y_==y__):
            x__+=1e3
            continue
        x_=x__-(y-c)/(y_-y__)*2*equ_delta
        if abs(x_-x__)<equ_preci:
            break
        x__=x_
    else:
        printc(Fore.YELLOW+'Warning:Itenaration Limit Exceded!\nThe result might be inaccurate.')
    return x__
def func_plot(arr,polar=False):
    assert len(arr)>=3
    global colorset
    colors=len(colorset)-1
    fs=set(arr[0].replace(',','+').split('+'))
    is_empty=True
    x_axis=False
    y_axis=False
    outofbound=False
    start_=calc_(arr[1])
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else (end-start_)/100
    assert (end-start_)*step>0
    s=int(step/m.fabs(step))
    if polar:
        ax=plt.subplot(polar=True)
    for index,f in enumerate(fs):
        scatter=('.' in f)
        inte=('*' in f)
        deri=("'" in f)
        fill=('#' in f)
        f=re.sub(r"\.|\*|'|#",'',f)
        verticle=[]     #竖直渐近线
        horizontal=[]   #水平切线
        xs=[]
        ys=[]
        xss=[]
        yss=[]
        vhline=False
        fn=f
        if '&' in f:
            f=[x for x in fn.split('&') if x]
            assert len(f)>=2
            f_x,f_y=f[0],f[1]
            for f_ in [f_x,f_y]:
                if ((not f_ in func_def) or (func_def[f_][0]!=1)) and (not f_ in func_in):
                    return 'Error - Function [%s] with exactly 1 varible needed!'%f_
            start=start_-step
            xl=lambda s,t:cal([f_x,'(',str(s),')'])
            yl=lambda x,y,s,t:cal([f_y,'(',str(s),')'])
        elif '|' in f:
            f=[x for x in fn.split('|') if x]
            assert len(f)>=2
            f_x,f_y=f[0],f[1]
            if ((not f_x in func_def) or (func_def[f_x][0]!=1)) and (not f_x in func_in):
                return 'Error - Function [%s] with exactly 1 varible needed!'%f_x
            if ((not f_y in func_def) or (func_def[f_y][0]!=2)) and (not f_y in func_in2):
                return 'Error - Function [%s] with exactly 2 varible needed!'%f_y
            start=start_-step
            xl=lambda s,t:cal([f_x,'(',str(s),')'])
            yl=lambda x,y,s,t:cal([f_y,'(',str(s),',',str(x),')'])
        else:
            vhline=True
            if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
                return 'Error - Function [%s] with exactly 1 varible needed!'%f
            y_o=''
            k_o=0 #用来保存上一次的斜率
            if inte:
                start=start_-step/2
                xl=lambda s,t:s+t/2
                yl=lambda x,y,s,t:y+t/6*calc_('{2}({0}-{1})+4*{2}({0})+{2}({0}+{1})'.format(s,t/2,f))
            elif deri:
                start=start_-step
                xl=lambda s,t:s+t/2
                yl=lambda x,y,s,t:calc_('{0}({1}+{2})-{0}({1})'.format(f,s,t))/t
            else:
                start=start_-step
                xl=lambda s,t:s
                yl=lambda x,y,s,t:cal([f,'(',str(s),')'])
        last_y=0
        while (end-start)*s>0:
            start+=step
            try:
                x=xl(start,step)
                y=yl(x,last_y,start,step)
                assert not(isinstance(x,str) or isinstance(y,str))
                if vhline:
                    if not y_o:y_o=y
                    if not (y-y_o)*k_o>=-0.5: #尽量避免瑕点
                        xss.append(xs)
                        yss.append(ys)
                        verticle.append(start-step/2)
                        xs=[]
                        ys=[]
                    else:
                        if ((y-y_o)*k_o<=0 and k_o!=0):
                            horizontal.append((y+y_o)/2)
                    k_o=y-y_o
                    y_o=y
                if m.fabs(y)<settings['bound']: #舍去较大的点
                    if polar and y<0:
                        xs.append(x+m.pi)
                        ys.append(0-y)
                    else:
                        xs.append(x)
                        ys.append(y)
                else:
                    outofbound=True
                last_y=y
            except:
                xss.append(xs)
                yss.append(ys)
                verticle.append(x)
                xs=[]
                ys=[]
        xss.append(xs)
        yss.append(ys)
        ys=[]
        xs=[]
        for y in yss:
            ys+=y
        for x in xss:
            xs+=x
        if ys:
            is_empty=False
            max_y,min_y=max(ys),min(ys)
            if min_y<=0<=max_y:
                x_axis=True
            max_x,min_x=max(xs),min(xs)
            if min_x<=0<=max_x:
                y_axis=True
            if switches['vline'] and not polar:
                for x in verticle:
                    plt.axvline(x,linestyle='-.',color=colorset[0],linewidth=0.9)
            if switches['hline'] and not polar:
                for y in horizontal:
                    plt.axhline(y,linestyle='-.',color=colorset[0],linewidth=0.9)
            if x_axis and not polar:
                plt.axhline(0,color='black',linewidth=0.7)
            if y_axis and not polar:
                plt.axvline(0,color='black',linewidth=0.7)
            for x,y in zip(xss,yss):
                if scatter:
                    plt.scatter(x,y,color=colorset[index%colors+1],alpha=0.5,marker='.')
                else:
                    plt.plot(x,y,color=colorset[index%colors+1],linewidth=1.3)
                if fill:
                    plt.fill_between(x,y,color=colorset[index%colors+1]+'70')
            if inte:
                fn='∫'+fn+'(x)dx'
            elif deri:
                fn=fn+"'(x)"
            else:
                fn=fn+'(x)'
            plt.figtext(0.01,0.9-0.05*index,fn,color=colorset[index%colors+1])
    if not is_empty:
        if outofbound:
            printc(Fore.YELLOW+'Warning: Part of the graph is out of bound(%s) and not shown\n'%settings['bound']+Fore.CYAN+'->Use $bound to modify bound')
        plt.grid(True)
        #plt.get_xaxis().get_major_formatter().set_useOffset(False)
        if polar:
            ax.set_rmin(0)
        system('attrib -h %s 2>nul'%plot_file)
        plt.savefig(plot_file)
        plt.clf()
        system('start '+plot_file+'&&attrib +h %s 2>nul'%plot_file)
    else:
        printc(Fore.YELLOW+'Warning: Nothing to plot\n'+Fore.CYAN+'->Maybe out of bound or region?')
    return ''
def func_density(arr):
    assert len(arr)>=5
    global colorset
    colors=len(colorset)-1
    fs=set(arr[0].split('+'))
    is_empty=True
    start_=calc_(arr[1])
    end=calc_(arr[2])
    left=calc_(arr[3])
    right=calc_(arr[4])
    cab_num=int(settings['cab'])
    step=calc_(arr[5]) if len(arr)>5 else (end-start_)/1000
    assert (end-start_)*step>0
    assert (right-left)*int(cab_num)>0
    s=int(step/m.fabs(step))
    diff=(right-left)/cab_num
    he=1/m.fabs(diff*(end-start_)/step)
    left_=left
    outofbound=False
    for index,f in enumerate(fs):
        scatter=('.' in f)
        inte=('*' in f)
        deri=("'" in f)
        fill=('#' in f)
        f=re.sub(r"\.|\*|'|#",'',f)
        cab=[0]*cab_num
        left=left_
        xs=[]
        ys=[]
        if inte:
            start=start_-step/2
            yl=lambda y,s,t:y+calc_('{2}({0}-{1})+4*{2}({0})+{2}({0}+{1})'.format(s,t/2,f))*t/6
        elif deri:
            start=start_-step/2
            yl=lambda y,s,t:calc_('{0}({1}+{2})-{0}({1})'.format(f,s,t))/t
        else:
            start=start_
            yl=lambda y,s,t:cal([f,'(',str(s),')'])
        last_y=0
        while (end-start)*s>0:
            start+=step
            try:
                y=yl(last_y,start,step)
                if isinstance(y,str):
                    continue
                ind=int((y-left)/diff)
                if 0<=ind<cab_num:
                    cab[ind]+=1
            except:
                pass
            last_y=y
        left-=diff/2
        for i in cab:
            left+=diff
            # if not i==0:
            i*=he
            if i<settings['bound']:
                xs.append(left)
                ys.append(i)
            else:
                outofbound=True
        if ys:
            is_empty=False
            if scatter:
                plt.scatter(xs,ys,color=colorset[index%colors+1],alpha=0.5,marker='.')
            else:
                plt.plot(xs,ys,color=colorset[index%colors+1],linewidth=1.3)
            if fill:
                plt.fill_between(xs,ys,color=colorset[index%colors+1]+'70')
            if inte:
                fn='∫'+f+'(x)dx'
            elif deri:
                fn=f+"'(x)"
            else:
                fn=f+'(x)'
            plt.figtext(0.01,0.9-0.05*index,fn,color=colorset[index%colors+1])
    if not is_empty:
        if outofbound:
            printc(Fore.YELLOW+'Warning: Part of the graph is out of bound(%s) and not shown\n'%settings['bound']+Fore.CYAN+'->Use $bound to modify bound')
        plt.grid(True)
        #plt.get_xaxis().get_major_formatter().set_useOffset(False)
        system('attrib -h %s 2>nul'%plot_file)
        plt.savefig(plot_file)
        plt.clf()
        system('start '+plot_file+'&&attrib +h %s 2>nul'%plot_file)
    else:
        printc(Fore.YELLOW+'Warning: Nothing to plot\n'+Fore.CYAN+'->Maybe out of bound or region?')
    return ''
def func_list(arr):
    assert len(arr)>=3
    fs=arr[0].split('+')
    for f in fs:
        if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
            return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1
    assert (end-start)*step>=0
    s=int(step/m.fabs(step))
    printc('x\t'+'\t\t'.join([f+'(x)' for f in fs]))
    while (end-start)*s>=0:
        exp=str(start)
        for f in fs:
            try:
                exp_re=cal([f,'(',str(start),')'])
                assert not isinstance(exp_re,str)
                exp_re='%-10f'%exp_re
            except:
                exp_re=Fore.RED+'Error   '+Fore.WHITE
            exp+='\t'+exp_re
        printc(exp)
        start+=step
    return ''
def func_sum(arr):
    assert len(arr)>=3
    f=arr[0]
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    start_=start
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1
    assert (end-start)*step>0
    s=int(step/m.fabs(step))
    exp_re=0
    while (end-start)*s>=0:
        try:
            tmp=cal([f,'(',str(start),')'])
            assert not isinstance(tmp,str)
        except:
            pass
        else:
            exp_re+=tmp
        start+=step
    printc(Fore.LIGHTCYAN_EX+'From %s to %s.'%(start_,start-step))
    return exp_re
def func_sumplot(arr):
    assert len(arr)>=3
    f=arr[0]
    scatter=('.' in f)
    f=f.replace('.','')
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    start_=start
    x=[]
    y=[]
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1
    assert (end-start)*step>0
    s=int(step/m.fabs(step))
    exp_re=0
    while (end-start)*s>=0:
        try:
            tmp=cal([f,'(',str(start),')'])
            assert not isinstance(tmp,str)
        except:
            pass
        else:
            exp_re+=tmp
            x.append(start)
            y.append(exp_re)
        start+=step
    if x:
        if scatter:
            plt.scatter(x,y,color=(0,0.7,1),alpha=0.5,marker='.')
        else:
            plt.plot(x,y,color=(0,0.7,1),linewidth=1.3)
        system('attrib -h %s 2>nul'%plot_file)
        plt.grid(True)
        plt.savefig(plot_file)
        plt.clf()
        system('start '+plot_file+'&&attrib +h %s 2>nul'%plot_file)
    else:
        raise Exception('Nothing to plot')
    return exp_re
def func_mltplot(arr):
    assert len(arr)>=3
    f=arr[0]
    scatter=('.' in f)
    f=f.replace('.','')
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    start_=start
    x=[]
    y=[]
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1
    assert (end-start)*step>0
    s=int(step/m.fabs(step))
    exp_re=1
    while (end-start)*s>=0:
        try:
            tmp=cal([f,'(',str(start),')'])
            assert not isinstance(tmp,str)
        except:
            pass
        else:
            exp_re*=tmp
            x.append(start)
            y.append(exp_re)
        start+=step
    if x:
        if scatter:
            plt.scatter(x,y,color=(0,0.7,1),alpha=0.5,marker='.',s=4)
        else:
            plt.plot(x,y,color=(0,0.7,1),linewidth=1.3)
        system('attrib -h %s 2>nul'%plot_file)
        plt.grid(True)
        plt.savefig(plot_file)
        plt.clf()
        system('start '+plot_file+'&&attrib +h %s 2>nul'%plot_file)
    else:
        raise Exception('Nothing to plot')
    return exp_re
def func_mlt(arr):
    assert len(arr)>=3
    f=arr[0]
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    start_=start
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1
    assert (end-start)*step>0
    s=int(step/m.fabs(step))
    exp_re=1
    while (end-start)*s>=0:
        try:
            tmp=cal([f,'(',str(start),')'])
            assert not isinstance(tmp,str)
        except:
            pass
        else:
            exp_re*=tmp
        start+=step
    printc(Fore.LIGHTCYAN_EX+'From %s to %s.'%(start_,start-step))
    return exp_re
def func_integ(arr):
    assert len(arr)>=3
    f=arr[0]
    if ((not f in func_def) or (func_def[f][0]!=1)) and (not f in func_in):
        return 'Error - Function [%s] with exactly 1 varible needed!'%f
    start=calc_(arr[1])
    end=calc_(arr[2])
    step=calc_(arr[3]) if len(arr)>=4 else 1000
    assert not isinstance((end-start)*step,str)
    step*=2
    delta=(end-start)/step
    result=0
    warning_=False    #计算错误警告
    for i in range(step+1):
        try:
            exp_re=cal([f,'(',str(start+i*delta),')'])
            assert not isinstance(exp_re,str)
        except:
            warning_=True
        else:
            if i==0 or i==step+1:
                result+=exp_re
            elif i%2==0:
                result+=exp_re*4
            else:
                result+=exp_re*2
    result*=delta/3
    if warning_:
        printc(Fore.YELLOW+'Error detected, the result maybe inaccurate:')
    return result

def int_(num):
    return int(num+0.5)

def is_int(num,preci=1e-10):
    return -preci<=int_(num)-num<=preci
def pretty_print(num):#分数、根式、pi的显示
    if num>1e15 or num<1e-5:
        return num
    for p in range(1,10000):
        q=num*p
        q_=q
        if is_int(q):
            q=int_(q)
            if p==1:
                return str(q)
            else:
                return str(q)+'/'+str(p)
        q=q_/m.pi
        if is_int(q):
            q=int_(q)
            if p==1:
                return str(q)+'π' if q!=1 else 'π'
            else:
                return (str(q)+'π' if q!=1 else 'π')+'/'+str(p)
        q=q_*m.pi
        if is_int(q):
            q=int_(q)
            return str(q)+'/'+('('+str(p)+'π)' if p!=1 else 'π')
    num_pi=(num/m.pi)**2
    num_=num**2
    flag_pi=False
    for p in range(1,10000):
        q=num_*p
        if is_int(q):
            break
        q=num_pi*p
        if is_int(q):
            flag_pi=True
            break 
    else:
        return num
    q=int_(q)
    p_f=factor(p)
    q_f=factor(q)
    p_out=1
    for x,y in p_f.items():
        if y%2==1:
            y+=1
            if x in q_f:
                q_f[x]+=1
            else:
                q_f[x]=1
        p_out*=x**(y/2)
    q_out=1
    q_in=1
    for x,y in q_f.items():
        if y%2==1:
            q_in*=x
            y-=1
        q_out*=x**(y/2)
    if q_in>1e8:
        print(Fore.YELLOW+'Warning: Might be inaccurate for big numbers.'+Fore.RESET)
    p_out,q_out,q_in=str(int_(p_out)),str(int_(q_out)),'√'+str(int_(q_in))
    if q_out=='1':
        qq=q_in
    else:
        qq=q_out+q_in
    qq+='π' if flag_pi else ''
    if p_out=='1':
        return qq
    else:
        return qq+'/'+p_out

def factor(num):
    fact={}
    for p in primes:
        while num%p==0:
            num/=p
            if p in fact:
                fact[p]+=1
            else:
                fact[p]=1
        if num==1:
            break
    else:
        fact[int(num)]=1
    return fact
# def factor2(num):#理论上可行，运行太慢
#     global primes
#     fact={}
#     for p in primes:
#         while num%p==0:
#             num/=p
#             if p in fact:
#                 fact[p]+=1
#             else:
#                 fact[p]=1
#         if num==1:
#             break
#         if num!=1 and p==primes[-1]:
#             q,found=p,False
#             while not found:
#                 q+=2
#                 for i in primes:
#                     if q%i==0:
#                         break
#                     if i*i>q:
#                         found=True
#                         break
#             primes.append(q)
#     if num!=1:
#         fact[int(num)]=1
#     return fact

def factor_gui(arr):
    assert len(arr)>=1
    num=to_num(arr[0])
    assert not isinstance(num,str)
    num=int(num)
    num_f=factor(num)
    exp_re=[]
    for x in sorted(num_f):
        y=str(num_f[x])
        exp_re.append(str(x)+('^'+y if y!='1' else ''))
    return '*'.join(exp_re)

def to_str(arr):
    return ','.join(map(str,arr))

def random_ico():
    ico=[
       [r'  ___    __    __    ___    ____  ____  _____ ''\n',
        r' / __)  /__\  (  )  / __)  (  _ \(  _ \(  _  )''\n',
        r'( (__  /(__)\  )(__( (__    )___/ )   / )(_)( ''\n',
        r' \___)(__)(__)(____)\___)  (__)  (_)\_)(_____)''\n'],
       [r'_________        .__           __________                ''\n',
        r'\_   ___ \_____  |  |   ____   \______   \_______  ____  ''\n',
        r'/    \  \/\__  \ |  | _/ ___\   |     ___/\_  __ \/  _ \ ''\n',
        r'\     \____/ __ \|  |_\  \___   |    |     |  | \(  <_> )''\n',
        r' \______  (____  /____/\___  >  |____|     |__|   \____/ ''\n',
        r'        \/     \/          \/                            ''\n'],
       [r' _______  _______  ___      _______    _______  ______    _______ ''\n',
        r'|       ||   _   ||   |    |       |  |       ||    _ |  |       |''\n',
        r'|       ||  |_|  ||   |    |       |  |    _  ||   | ||  |   _   |''\n',
        r'|       ||       ||   |    |       |  |   |_| ||   |_||_ |  | |  |''\n',
        r'|      _||       ||   |___ |      _|  |    ___||    __  ||  |_|  |''\n',
        r'|     |_ |   _   ||       ||     |_   |   |    |   |  | ||       |''\n',
        r'|_______||__| |__||_______||_______|  |___|    |___|  |_||_______|''\n'],
       [r'   ______      __        ____           ''\n',
        r'  / ____/___ _/ /____   / __ \_________ ''\n',
        r' / /   / __ `/ / ___/  / /_/ / ___/ __ \ ''\n',
        r'/ /___/ /_/ / / /__   / ____/ /  / /_/ /''\n',
        r'\____/\__,_/_/\___/  /_/   /_/   \____/ ''\n',
        r'                                        ''\n']]
    return ''.join(ico[random.randint(0,len(ico)-1)])
if __name__=='__main__':
    main()