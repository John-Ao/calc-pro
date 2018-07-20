@echo off
setlocal enabledelayedexpansion
::___________________________________________________________________
::
::       本计算器除特殊声明外所有代码均由  John Ao 独立编写，
::                     转载或用于他图请保留版权信息。
::
::                       :addmnu函数由 pcl_test 编写
::                       :mlt函数由  happy886rr  编写
::___________________________________________________________________
set "title=Calc Pro.2 - Ao"
title %title%
for /l %%i in (1,1,1000) do @set ".%%i=%%i"
set "to.add=00000000000000000000000000000000000000000000000000000000000000000000000000000000000005"
set "add.add=00000000000000000000000000000000000000000000000000000000000000000000000000000000000001"
set "Precision=16"
set /a "varreplace=1,timing=1"
call :execute "PI=3.1415926535"
call :execute "E=2.718281828"
:begin
echo,
set /a "in.acc+=1"
set "in.%in.acc%=0"
set /p in.%in.acc%=In[%in.acc%]:=
if "!in.%in.acc%:~0,1!"=="$" goto :command
if "!in.%in.acc%:~0,1!"=="#" (
cmd /s /d /c "!in.%in.acc%:~1!"
goto :begin
)
set "in.%in.acc%.time=%time%"
set "in.%in.acc%=!in.%in.acc%: =!"
set "in.%in.acc%=!in.%in.acc%:[=(!"
set "in.%in.acc%=!in.%in.acc%:]=)!"
set "in.%in.acc%=!in.%in.acc%:{=(!"
set "in.%in.acc%=!in.%in.acc%:}=)!"
call :parse in.%in.acc% out.%in.acc%
echo,Out[%in.acc%]=!out.%in.acc%!
if "%timing%"=="1" (
call :etime !in.%in.acc%.time! %time% in.%in.acc%.time
set "in.%in.acc%.time=000!in.%in.acc%.time!"
echo Time used:!in.%in.acc%.time:~0,-2!.!in.%in.acc%.time:~-2!s
)
goto :begin

:command
set "com=!in.%in.acc%:~1!"
if /i "%com%"=="time off" set "timing=0"&goto :begin
if /i "%com%"=="time on" set "timing=0"&goto :begin
if /i "%com%"=="exit" exit
if /i "%com%"=="new" endlocal&start "" "%~dpnx0"&exit
if /i "%com%"=="clear" endlocal&setlocal enabledelayedexpansion&goto :begin
if /i "%com%"=="cls" cls&goto :begin
for /f "tokens=1,2 delims= " %%a in ("%com%") do (
	if /i "%%a"=="precision" (
		if "%%b"=="off" (
			set "pre=.o"
		) else (
		if "%%b"=="on" (
			set "pre="
		) else (
			set "Precision=%%b"
		))
	)
)
goto :begin

:parse [v.input] [v.result]__________________________________________________
set "%1.0=("
set "%1.p=!%1!"
set "$1=%1.p"
call :toarr
::__________to avoid "too much recursion error"
call :concatnum %1.p %1
set "%1.pmatch=0"
set "%1.ematch=0"
for /l %%i in (1,1,!%1.length!) do (
	set /a "follow=%%i+1"
	if "!%1.%%i.type!"=="alp" (
		if "!%1.%%i.follow!"=="(" (
			set "%1.%%i.type=fun"
			set "%1.%%i.prv=9"
		) else (
			set "%1.%%i.type=var"
			if %1.ematch lss 1 call :push %1.varlist .%%i
		)
	) else (
		if "!%1.%%i.type!"=="sym" (
			if "!%1.%%i!"=="+" set "%1.%%i.type=add"&set "%1.%%i.prv=5"
			if "!%1.%%i!"=="-" set "%1.%%i.type=mnu"&set "%1.%%i.prv=5"
			if "!%1.%%i!"=="*" set "%1.%%i.type=mlt%pre%"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="/" set "%1.%%i.type=div"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="\" set "%1.%%i.type=div2"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="^" set "%1.%%i.type=pow%pre%"&set "%1.%%i.prv=7"
			if "!%1.%%i!"=="(" set "%1.%%i.type=lp"&set "%1.%%i.prv=0"&set /a "%1.pmatch+=1"
			if "!%1.%%i!"==")" set "%1.%%i.type=rp"&set "%1.%%i.prv=0"&set /a "%1.pmatch-=1"
			if "!%1.%%i!"=="%%" set "%1.%%i.type=mod"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="=" (
				set "%1.%%i.type=equ"
				set /a "%1.ematch+=1"
				if !%1.ematch! equ 1 set "%1.equ=%%i"
			)
			if "!%1.%%i!"=="," set "%1.%%i.type=dli"&set "%1.%%i.prv=0"
		)
	)
	if !%1.pmatch! lss 0 (
		echo,Error:Unmatched parathesis!
		exit /b
	)
)
if !%1.pmatch! neq 0 (
	echo,Error:Unmatched parathesis!
	exit /b
)
if !%1.ematch! gtr 1 (
	echo,Error:!%1.ematch! [=]'s are found!
	exit /b
)
if "!%1.ematch!"=="1" (
if "!%1.1.type!"=="var" (
call :def.var %1 %2
) else (
call :def.fun %1 %2
)
exit /b
)
for /l %%i in (1,1,!%1.length!) do (
	if "!%1.%%i.type!"=="var" (
		if "%varreplace%"=="1" (
			call :myset %1.%%i var.!%1.%%i!
			if "!%1.%%i!"=="" set "%1.%%i=0"
		)
	set "%1.%%i.type=num"
	)
)
::________________________to p____________________________
for /l %%i in (1,1,!%1.length!) do (
	if "!%1.%%i.type!"=="num" (
		call :pusht %1.r %1.%%i
	) else (
	if "!%1.%%i.type!"=="lp" (
		call :push %1.syms .%%i
		set "%1.sp.lp=%%i"
	) else (
	if "!%1.%%i.type!"=="fun" (
		call :push %1.syms .%%i
	) else (
	if "!%1.%%i.type!"=="rp" (
		call :popall %1 %1.syms %1.r
	) else (
	if "!%1.%%i.type!"=="dli" (
		call :popall %1 %1.syms %1.r
		call :push %1.syms .!%1.sp.lp!
	) else (
		if not defined %1.%%i.prv (
			echo,Error:Undefined syntax - [!%1.%%i!]&exit /b
		)
		call :popsym %1 %1.syms %1.r %%i
		call :push %1.syms .%%i
	)))))
)
call :popall %1 %1.syms %1.r
call :cal %1.r %1.result
set "%2=!%1.result!"
exit /b

:cal [host] [return]____________________________________________________
set "%1.nums.length=0"
for /l %%i in (1,1,!%1.length!) do (
	if "!%1.%%i.type!"=="num" (
		call :push %1.nums %1.%%i
	) else (
	if "!%1.%%i.type!"=="fun" (
		call :fapply !%1.%%i! %1.nums %1.calre %1.host.%%i
		call :push %1.nums %1.calre
	) else (
		call :pop %1.nums %1.cal1
		call :pop %1.nums %1.cal2
		if "!%1.cal1!"=="" (
			echo,Function error:[!%1.%%i!] needs 2 arguments!
		) else (
		if "!%1.cal2!"=="" (
			echo,Function error:[!%1.%%i!] needs 2 arguments!
		) else (
			call :!%1.%%i.type! !%1.cal2! !%1.cal1! %1.calre
			call :push %1.nums %1.calre
		))
	))
)
set /a "%1.nums.length+=0"
if "!%1.nums.length!"=="1" (
	set "%2=!%1.nums.1!"
) else (
if !%1.nums.length! gtr 1 (
	echo,Maybe:Multi- or Invalid input!
	set "%2=(!%1.nums.1!"
	for /l %%i in (2,1,!%1.nums.length!) do (
		set "%2=!%2!,!%1.nums.%%i!"
	)
	set "%2=!%2!)"
) else (
	echo,Invalid input!
))
exit /b

:test [host]_________________________________________________________
(
call :return %1
call :return %1.r
call :return %1.syms
echo,
)
exit /b

:popsym [host] [syms] [output] [num]_________________________________
call :pop %2 %2.tmp1
if "!%2.tmp1!"=="" exit /b
call :myset prv1 %1.!%2.tmp1!.prv
call :myset prv2 %1.%4.prv
::_________________^exception______________________
if !prv1! equ !prv2! (
if !prv1! equ 7 (
call :push %2 %2.tmp1
call :push %2 .%4
exit /b
))
::_________________otherwise_________________________
if not !prv1! lss !prv2! (
call :pusht %3 %1.!%2.tmp1!
call :popsym %1 %2 %3 %4
) else (
call :push %2 %2.tmp1
)
exit /b

:popall [host] [syms] [output]_______________________________________
set /a "%2.length+=0"
if !%2.length! lss 1 exit /b
call :pop %2 %2.tmp1
call :myset ppo %1.!%2.tmp1!
if "!ppo!"=="(" exit /b
call :pusht %3 %1.!%2.tmp1!
call :popall %1 %2 %3
exit /b

:toarr [$1=str]______________________________________________________
set /a "%$1%.acc+=1"
set "%$1%.head=!%$1%:~0,1!"
set "%$1%.tail=!%$1%:~1!"
set "%$1%.!%$1%.acc!=!%$1%.head!"
if "!%$1%.tail!"=="" (
set /a "%$1%.length=%$1%.acc"
) else (
set "%$1%=!%$1%.tail!"
goto :toarr
)
exit /b

:concatnum [input] [output]__________________________________________
set "ccn.now="
set "ccn.flag.old="
set "ccn.flag.sp=0"
for /l %%A in (1,1,!%1.length!) do (
	set "%1.now=!%1.%%A!"
	set "ccn.flag=0"
	for %%B in (@=.=0=1=2=3=4=5=6=7=8=9) do (
		if "!%1.now!"=="%%B" set "ccn.flag=num"
	)
	for %%B in (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) do (
		if /i "!%1.now!"=="%%B" set "ccn.flag=alp"
	)
	if "!ccn.flag!"=="0" (
		if "%ccn.flag.sp%"=="at" (
			if not "!%1.now!"=="+" (
			if not "!%1.now!"=="-" (
			echo,Error:Invalid Exponent^! - [!%1.now!]
			exit /b
			))
			set "ccn.flag=num"
		)
	)
	set /a "ccn.l.t=%2.length+1"
	if "!ccn.flag!"=="0" (
		if not "!ccn.now!"=="" (
			call :push %2 ccn.now
			set "ccn.now="
			set /a "ccn.l.t=%2.length+1"
		)
		set "%2.!ccn.l.t!.type=sym"
		call :push %2 %1.now
	) else (
		if "!ccn.flag!"=="!ccn.flag.old!" (
			set "ccn.now=!ccn.now!!%1.now!"
		) else (
			if not "!ccn.now!"=="" (
				call :push %2 ccn.now
			)
			set "ccn.now=!%1.now!"
		)
		set /a "ccn.l.t=%2.length+1"
		set "%2.!ccn.l.t!.type=!ccn.flag!"
	)
	set "ccn.flag.old=!ccn.flag!"
	set "ccn.flag=0"
	if "!%1.now!"=="@" (
		if "%pre%"==".o" echo Error:Invalid [@],precision is off^!&exit /b
		set "ccn.flag.sp=at"
	) else (
		set "ccn.flag.sp=0"
	)
)
call :push %2 ccn.now
exit /b

:push [v.arr] [v.str]________________________________________________
if not "!%2!"=="" (
set "%1.!%1.length!.follow=!%2!"
set /a "%1.length+=1"
set "%1.!%1.length!=!%2!"
set /a "%1..length=%1.length+1"
set "%1.!%1..length!.before=!%2!"
)
exit /b

:pusht [v.arr] [v.str]_______________________________________________
if not "!%2!"=="" (
set "%1.!%1.length!.follow=!%2!"
set /a "%1.length+=1"
set "%1.!%1.length!=!%2!"
set "%1.!%1.length!.type=!%2.type!"
set /a "%1..length=%1.length+1"
set "%1.!%1..length!.before=!%2!"
)
exit /b

:pop [v.arr] [v.str]_________________________________________________
set /a "pop.len=!%1.length!+0"
if %pop.len% equ 0 set "%2="&exit /b
set "%2=!%1.%pop.len%!"
set /a "%1.length-=1"
exit /b

:return [v.arr]______________________________________________________
set "%1.re="
if "!%1.length!"=="0" exit /b
set "return="
for /l %%i in (1,1,!%1.length!) do (
set "%1.re=!%1.re!!return!!%1.%%i![!%1.%%i.type!]"
set "return= "
)
echo,%1=!%1.re!
exit /b

:arrto [v.arr] [v.str] [v.start] [v.end]_______________________________
set "arrto="
for /l %%i in (!%3!,1,!%4!) do set "arrto=!arrto!!%1.%%i!"
set "%2=%arrto%"
exit /b

:myset ____________________________________________________________
set "%1=!%2!"
exit /b

:def.var [host] [v.re]_______________________________________________
if !%1.length! lss 2 (
echo,Error:No expression for [!%1.1!]^!
exit /b
)
for /l %%i in (3,1,!%1.length!) do (
set "%1.def=!%1.def!!%1.%%i!"
)
call :parse %1.def %1.def.re
set "var.!%1.1!=!%1.def.re!"
set "%2=!%1.1!:=!%1.def.re!"
exit /b

:def.fun [host] [v.re]_______________________________________________
set /a "%1.equ-=2"
set "%1.deffun=1"
for /l %%i in (3,2,!%1.equ!) do (
if not "!%1.%%i.type!"=="var" set "%1.deffun=0"
)
if "!%1.deffun!"=="0" (
call :let.fun %1 %2
exit /b
)
set /a "%1.equ+=3"
call :arrto %1 func.!%1.1! %1.equ %1.length
set /a "%1.equ-=3"
set "func.!%1.1!.var.length=0"
for /l %%i in (3,2,!%1.equ!) do (
call :push func.!%1.1!.var %1.%%i
)
set "%2=Function [!%1.1!] defined."
exit /b

:let.fun [host] [v.re]_______________________________________________
set /a "func.!%1.1!.known.acc+=1"
call :myset lf.acc func.!%1.1!.known.acc
for /l %%i in (3,2,!%1.equ!) do (
call :push func.!%1.1!.known.arr.!lf.acc! %1.%%i
)
if not defined func.!%1.1!.var.length call :myset func.!%1.1!.var.length func.!%1.1!.known.arr.!lf.acc!.length
set "lf.arr=func.!%1.1!.known.arr.!lf.acc!"
call :push func.!%1.1!.known lf.arr
set /a "%1.equ+=3"
call :arrto %1 %1.lf.p %1.equ %1.length
set /a "lf.gap=%1.length-%1.equ"
if %lf.gap% equ 0 (
	if !%1.lf.p! lss a (
		set "func.!%1.1!.known.arr.!lf.acc!=!%1.lf.p!"
	)
)
set "func.!%1.1!.sub.!lf.acc!=!%1.lf.p!"
set /a "%1.equ-=2"
call :arrto %1 %2 .1 %1.equ
set "%2=!%2!:=!%1.lf.p!"
exit /b

:fapply [function] [numlist] [return] [host]_________________________
set "%4.fapply.toknow.length=0"
for /l %%i in (1,1,!func.%1.var.length!) do (
	call :pop %2 %4.fapply.%%i
	if "!%4.fapply.%%i!"=="" (
		echo,Error:function [%1] needs more arguments^!
		exit /b
	)
	call :push %4.fapply.toknow %4.fapply.%%i
)
call :reverse %4.fapply.toknow
call :getknown %1 %4.fapply.toknow %3 %4
if "!%3!"=="" (
	if "!func.%1!"=="" echo,Error:Undefined.&exit /b
	for /l %%i in (1,1,!func.%1.var.length!) do (
		call :myset %4.backup.!func.%1.var.%%i! var.!func.%1.var.%%i!
		set "var.!func.%1.var.%%i!=!%4.fapply.toknow.%%i!"
	)
	set "%4.func.%1=!func.%1!"
	call :parse %4.func.%1 %3
	for /l %%i in (1,1,!func.%1.var.length!) do (
		call :myset var.!func.%1.var.%%i! %4.backup.!func.%1.var.%%i!
	)
set /a "func.%1.known.acc+=1"
call :myset lf.acc func.%1.known.acc
for /l %%i in (1,1,!%4.fapply.toknow.length!) do (
	call :push func.%1.known.arr.!lf.acc! %4.fapply.toknow.%%i
)
set "lf.arr=func.%1.known.arr.!lf.acc!"
call :push func.%1.known lf.arr
set "func.%1.known.arr.!lf.acc!=!%3!"
)
exit /b

:getknown [function] [arr toknow] [return] [host]____________________
set "%3="
set "gk.match="
for /l %%i in (1,1,!func.%1.known.length!) do (
	call :equarr !func.%1.known.%%i! %2 gk
	if "!gk!"=="1" set "gk.match=%%i"
)
if "!gk.match!"=="" exit /b
set "gk.arr=func.%1.known.arr.%gk.match%"
if not "!%gk.arr%!"=="" set "%3=!%gk.arr%!"&exit /b
set "gk.fun=func.%1.sub.%gk.match%"
for /l %%i in (1,1,!%gk.arr%.length!) do (
	call :myset %4.backup.!%gk.arr%.%%i! var.!%gk.arr%.%%i!
	set "var.!%gk.arr%.%%i!=!%2.%%i!"
)
set "%4.!gk.fun!=!%gk.fun%!"
call :parse %4.!gk.fun! %3
for /l %%i in (1,1,!func.%1.var.length!) do (
	call :myset var.!func.%1.var.%%i! %4.backup.!func.%1.var.%%i!
)
exit /b

:equarr [arr1] [arr2] [return] __________________________________
set "%3=1"
for /l %%i in (1,1,!%1.length!) do (
	if not "!%1.%%i!"=="!%2.%%i!" (
		if "!%1.%%i!" lss "a" (
			if "!%2.%%i!" lss "a" (
				call :mnu !%1.%%i! !%2.%%i! ea.r
				if not "!ea.r!"=="0" if not "!ea.r!"== "0@0" set "%3=0"
			)
		)
	)
)
exit /b

:reverse [v.arr]_____________________________________________________
for /l %%i in (1,1,!%1.length!) do (
	call :pop %1 reverse
	call :push %1.rever reverse
)
for /l %%i in (1,1,!%1.rever.length!) do (
	call :push %1 %1.rever.%%i
)
exit /b

:execute [expression]________________________________________________
set /a "exe.acc+=1"
set "exe.!exe.acc!=%~1"
call :parse exe.!exe.acc! exe.!exe.acc!.o
exit /b




































::________________________Scientific Calculation_________________________
:addmnu
call :toat %1 add1
call :toat %3 add2
if %add1.at% gtr %add2.at% (
set "addbig=%add1.num%"
set "addbig.at=%add1.at%"
set "addsma=%add2.num%"
set /a "adddif=add1.at-add2.at"
) else (
set "addbig=%add2.num%"
set "addbig.at=%add2.at%"
set "addsma=%add1.num%"
set /a "adddif=add2.at-add1.at"
)
if !adddif! gtr !Precision! set "%4=%addbig%@%addbig.at%"&exit /b
if %adddif% equ 0 (
call :addmnu.o %add1.num% %2 %add2.num% add.r
set "%4=!add.r!@%add1.at%"
exit /b
)
call :mlt.o !addsma! 0.!add.add:~-%adddif%! add.rr
call :addmnu.o %addbig% %2 %add.rr% add.r
call :toat %add.r%@%addbig.at% %4
exit /b

:add
call :addmnu%pre% %1 + %2 %3
exit /b

:add.o
call :addmnu.o %1 + %2 %3
exit /b

:mnu
call :addmnu%pre% %1 - %2 %3
exit /b

:div
set /a "%3=%1/%2"
exit /b
:div2
set /a "%3=%1/%2"
exit /b

:mod
set /a "%3=%1%%2"
exit /b

:pow.o
set "pow=1"
set "topow=%1"
set "powto=%2"
:pow.l.o
set /a "powmod=powto%%2"
if %powto% equ 0 (
	set "%3=!pow!"
	exit /b
)
if %powmod% neq 0 call :mlt.o %pow% %topow% pow
set /a "powto>>=1"
call :mlt.o %topow% %topow% topow
goto :pow.l.o

:pow
set "pow=1@0"
call :toat %1 topow
set "powto=%2"
:pow.l
set /a "powmod=powto%%2"
if %powto% equ 0 (
	set "%3=!pow!"
	exit /b
)
if %powmod% neq 0 call :mlt %pow% %topow% pow f
set /a "powto>>=1"
call :mlt %topow% %topow% topow f
goto :pow.l

:mlt.o //by happy886rr_________________________________________________
setlocal
for /f "tokens=1,2 delims=*" %%a in ("%1*%2") do (
	set A=%%a&set A=!A: =!
	set B=%%b&set B=!B: =!
)
for /f "tokens=1,2 delims=." %%a in ("!A!") do (
	set A1=%%a&set A=!A1!%%b
	if "%%b"=="" (set PA=0) else (
		set A2=%%b
		for %%i in (512 256 128 64 32 16 8 4 2 1) do (
    			if not "!A2:~%%i!"=="" (
				set/a PA+=%%i
				set "A2=!A2:~%%i!"
			)
		)
		if "!A2:~1!"=="" (set/a PA+=1)
	)
)
for /f "tokens=1,2 delims=." %%a in ("!B!") do (
	set B1=%%a&set B=!B1!%%b
	if "%%b"=="" (set PB=0) else (
		set B2=%%b
		for %%i in (512 256 128 64 32 16 8 4 2 1) do (
    			if not "!B2:~%%i!"=="" (
				set/a PB+=%%i
				set "B2=!B2:~%%i!"
			)
		)
		if "!B2:~1!"=="" (set/a PB+=1)
	)
)
set/a PO=PA+PB
CALL :CUT !A! A NA
CALL :CUT !B! B NB
set/a N=NA+NB,NA*=4,NB*=4
for /l %%i in (1 1 !N!) do (
	for /l %%j in (1 1 %%i) do (
		set/a j=%%i-%%j+1
		if defined A[%%j] (
			if defined B[!j!] (
				set/a sum=A[%%j]*B[!j!]+sum
			)
		)
	)
	set/a s=sum+10000
	set sum=!sum:~0,-4!
	set pul=!s:~-4!!pul!
)
if !PO! equ 0 (
	for /l %%i in (1 1 10) do (
		if "!pul:~0,1!"=="0" (
			set pul=!pul:~1!
		)
	)
	set "mlt.r=!pul!"
) else (
	set pre=!pul:~0,-%PO%!
	for /l %%i in (1 1 10) do (
		if "!pre:~0,1!"=="0" (
			set pre=!pre:~1!
		)
	)
	if not defined pre (set pre=0)
	set "mlt.r=!pre!.!pul:~-%PO%!"
)
endlocal&set "%3=%mlt.r%"&exit /b

:mlt //by happy886rr_________________________________________________
setlocal
if "%4"=="" (
call :toat "%1" mlt1
call :toat "%2" mlt2
) else (
set "mlt1=%1"
set "mlt2=%2"
)
for /f "tokens=1,2,3,4 delims=@" %%a in ("%mlt1%@%mlt2%") do (
	set A=%%a&set A=!A: =!&set "CA=%%b"
	set B=%%c&set B=!B: =!&set "CB=%%d"
)
for /f "tokens=1,2 delims=." %%a in ("!A!") do (
	set A1=%%a&set A=!A1!%%b
	if "%%b"=="" (set PA=0) else (
		call :getlength %%b PA
	)
)
for /f "tokens=1,2 delims=." %%a in ("!B!") do (
	set B1=%%a&set B=!B1!%%b
	if "%%b"=="" (set PB=0) else (
		call :getlength %%b PB
	)
)
set/a PO=PA+PB
set/a CO=CA+CB
CALL :CUT !A! A NA
CALL :CUT !B! B NB
set/a N=NA+NB,NA*=4,NB*=4
for /l %%i in (1 1 !N!) do (
	for /l %%j in (1 1 %%i) do (
		set/a j=%%i-%%j+1
		if defined A[%%j] (
			if defined B[!j!] (
				set/a sum=A[%%j]*B[!j!]+sum
			)
		)
	)
	set/a s=sum+10000
	set sum=!sum:~0,-4!
	set pul=!s:~-4!!pul!
)
if !PO! equ 0 (
	for /l %%i in (1 1 10) do (
		if "!pul:~0,1!"=="0" (
			set pul=!pul:~1!
		)
	)
	set "mlt.r=!pul!"
) else (
	set pre=!pul:~0,-%PO%!
	for /l %%i in (1 1 10) do (
		if "!pre:~0,1!"=="0" (
			set pre=!pre:~1!
		)
	)
	if not defined pre (set pre=0)
	set "mlt.r=!pre!.!pul:~-%PO%!"
)
call :toat %mlt.r%@%CO% mlt.r.t
endlocal&set "%3=%mlt.r.t%"&exit /b
:CUT 分割数组
set num=%1
if "!num:~-5!"=="!num:~-4!" (
	set %2[1]=!num!
	set %3=1
	goto :eof
)
for /l %%i in (1 1 225) do (
	if "!num:~0,-4!"=="" (
		set/a %2[%%i]=!num!
		set %3=%%i
		goto :eof
	)
	set/a %2[%%i]=1!num:~-4!-10000
	set num=!num:~0,-4%!
)
exit /b

:toat [num] [return]_________________________________________________
setlocal
set "toatn=%~1"
set "err=0"
(echo;%toatn%|find "@">nul )||set "err=1"
if !err! equ 1 set "toatn=%toatn%@0"
set "err=0"
(echo;%toatn%|find ".">nul )||set "err=1"
if !err! equ 1 set "toatn=%toatn:@=.@%"
call :toat2 %toatn%
for /l %%i in (1 1 5) do (
	if "!to.num:~0,1!"=="0" (
		set to.num=!to.num:~1!
	)
)
if "!to.num:~0,1!"=="." set "to.num=0%to.num%"
set "$2=%to.num%"
call :dezero
set "to.num=%$2%"
set "to.r=%to.num%@%to.3%"
(endlocal
set "%2=%to.r%
set "%2.at=%to.3%"
set "%2.num=%to.num%"
)
exit /b
:toat2 ______________________________________________________________
set "toatn=%~1"
for /f "tokens=1,2,3 delims=.@" %%a in ("%toatn%") do (
set "to.1=%%a"
set "to.2=%%b"
set "to.3=%%c"
if "!to.3!"=="" (
set "to.3=!to.2!"
set "to.2="
)
set /a "to.3+=0"
)
if "%to.1%"=="0" if not "%to.2%"=="" (
	set "to.1=%to.2:~0,1%"
	set "to.2=%to.2:~1%"
	set /a "to.3-=1"
	call :toat2 !to.1!.!to.2!@!to.3!
	exit /b
)
call :getlength "%to.1%" to.1.l
set /a "to.1.l-=1"
if %to.1.l% gtr 0 (
set "to.2=!to.1:~-%to.1.l%!%to.2%"
set "to.1=%to.1:~0,1%"
set /a "to.3+=to.1.l"
)
if "%to.2%"=="" (
set "to.num=%to.1%"
exit /b
)
call :getlength "!to.2:~%Precision%!" to.2.tail
if "!to.2:~%Precision%!" gtr "4a" (
call :add.o "%to.1%.%to.2%" "0.0!to.add:~-%Precision%!" to.4
set "to.num=!to.4:~0,-%to.2.tail%!"
) else (
set "to.num=%to.1%.!to.2:~0,%Precision%!"
)
exit /b
:dezero _____________________________________________________________
set "dz=%$2%"
if "%dz%"=="" exit /b
if "%dz:~-1%"=="%dz:~-2%" exit /b
if "%dz:~-1%"=="0" (
set "$2=%$2:~0,-1%"
goto :dezero
) else (
if "%dz:~-1%"=="." (
set "$2=%$2:~0,-1%"
)
exit /b
)
::___________________________________________________________________

:getlength [str] [return]
set "gl=%~1"
if "%gl%"=="" set "%2=0"&exit /b
set "glr=0"
for %%i in (512 256 128 64 32 16 8 4 2 1) do (
	if not "!gl:~%%i!"=="" (
		set/a "glr+=%%i"
		set "gl=!gl:~%%i!"
	)
)
if "!gl:~1!"=="" (set/a "glr+=1")
set /a "%2=glr"
exit /b

:addmnu.o //by pcl_test from bathome___________________________________
if "%~2" neq "+" if "%~2" neq "-" (set "%~4=算术运算符不正确"&goto :eof)
if "%~4" equ "" (set "%~4=缺少结果变量"&goto :eof)

if "%~1" equ "0" (
    if "%~3" equ "0" (set "%~4=0") else (
        set a=%~3
        if "%~2" equ "+" (
            set "%~4=%~3"
        ) else (
            if "!a:~,1!" equ "-" (set "%~4=!a:~1!") else (set "%~4=-%~3")
        )
    )
    goto :eof
)
if "%~3" equ "0" (set "%~4=%~1"&goto :eof)
if "%~1" equ "%~3" if "%~2" equ "-" (set "%~4=0"&goto :eof)

set a=%~1.0
set b=%~3.0
for /f "tokens=1,2 delims=." %%a in ("%a:-=%") do set "a_1=%%a"&set "a_2=%%b"
for /f "tokens=1,2 delims=." %%a in ("%b:-=%") do set "b_1=%%a"&set "b_2=%%b"
call :strlen %a_1% L1_1
call :strlen %a_2% L1_2
call :strlen %b_1% L2_1
call :strlen %b_2% L2_2

for %%i in (1 2) do (
    set "zero="&set m=0
    if !L1_%%i! leq !L2_%%i! (
        set /a m=L2_%%i-L1_%%i
        if !m! neq 0 (
            for /l %%a in (1 1 !m!) do set zero=!zero!0
        )
        if "%%i" equ "1" (set a_%%i=!zero!!a_%%i!) else set a_%%i=!a_%%i!!zero!
        set Len_%%i=!L2_%%i!
    ) else (
        set /a m=L1_%%i-L2_%%i
        for /l %%a in (1 1 !m!) do set zero=!zero!0
        if "%%i" equ "1" (set b_%%i=!zero!!b_%%i!) else set b_%%i=!b_%%i!!zero!
        set Len_%%i=!L1_%%i!
    )
)

set /a Len=Len_1+Len_2+1
if "%~2" equ "+" (
    if "!a:~,1!" neq "-" (
        if "!b:~,1!" neq "-" (
            call :jia %a_1%.%a_2% %b_1%.%b_2% %Len% s
            set "%~4=!s!"
        ) else (
            call :jian %a_1%.%a_2% %b_1%.%b_2% %Len% s
            if "%a_1%.%a_2%" gtr "%b_1%.%b_2%" (set "%~4=!s!") else set "%~4=-!s!"
        )
    ) else (
        if "!b:~,1!" neq "-" (
            call :jian %a_1%.%a_2% %b_1%.%b_2% %Len% s
            if "%a_1%.%a_2%" gtr "%b_1%.%b_2%" (set "%~4=-!s!") else set "%~4=!s!"
        ) else (
            call :jia %a_1%.%a_2% %b_1%.%b_2% %Len% s
            set "%~4=-!s!"
        )
    )
) else (
    if "!a:~,1!" neq "-" (
        if "!b:~,1!" neq "-" (
            call :jian %a_1%.%a_2% %b_1%.%b_2% %Len% s
            if "%a_1%.%a_2%" lss "%b_1%.%b_2%" (set "%~4=-!s!") else set "%~4=!s!"
        ) else (
            call :jia %a_1%.%a_2% %b_1%.%b_2% %Len% s
            set "%~4=!s!"
        )
    ) else (
        if "!b:~,1!" neq "-" (
            call :jia %a_1%.%a_2% %b_1%.%b_2% %Len% s
            set "%~4=-!s!"
        ) else (
            call :jian %a_1%.%a_2% %b_1%.%b_2% %Len% s
            if "%a_1%.%a_2%" lss "%b_1%.%b_2%" (set "%~4=!s!") else set "%~4=-!s!"
        )
    )
)
goto :eof

:strlen
setlocal
set "$=%1#"
set len=&for %%a in (4000 2048 1024 512 256 128 64 32 16)do if !$:~%%a!. neq . set/a len+=%%a&set $=!$:~%%a!
set $=!$!fedcba9876543210&set/a len+=0x!$:~16,1!
endlocal&set %2=%len%&goto :eof

:jia
setlocal
set a=%~1
set b=%~2
set t=0
set "s="
for /l %%a in (-1 -1 -%~3) do (
    if "!a:~%%a,1!" equ "." (
      set s=.!s!
    ) else (
        set /a "c=t+!a:~%%a,1!+!b:~%%a,1!"
        if !c! geq 10 (set t=1) else set t=0
        set s=!c:~-1!!s!
    )
)
if %t% equ 1 (set s=1!s!)
for /f "tokens=1,2 delims=." %%a in ("%s%") do (
    for /f "tokens=1* delims=0" %%c in (".%%b") do if "%%c%%d" equ "." set s=%%a
)
endlocal&set %~4=%s%&goto :eof

:jian
setlocal
if "%~1" lss "%~2" (
    set a=%~2
    set b=%~1
) else (
    set a=%~1
    set b=%~2
)
set t=0
set "s="
for /l %%a in (-1 -1 -%~3) do (
    if "!a:~%%a,1!" equ "." (
      set s=.!s!
    ) else (
        set /a "c=10+!a:~%%a,1!-!b:~%%a,1!-t"
        if !c! lss 10 (set t=1) else set t=0
        set s=!c:~-1!!s!
    )
)
for /f "tokens=1,2 delims=." %%a in ("%s%") do (
    for /f "tokens=* delims=0" %%c in ("%%a") do if "%%c" equ "" (set pre=0) else set pre=%%c
    for /f "tokens=* delims=0" %%c in ("%%b") do if "%%c" equ "" (set s=!pre!) else set s=!pre!.%%b
)
endlocal&set %~4=%s%&goto :eof

:etime <begin_time> <end_time> <return>
setlocal&set be=%~1:%~2&set cc=(%%d-%%a)*360000+(1%%e-1%%b)*6000+1%%f-1%%c&set dy=-8640000
for /f "delims=: tokens=1-6" %%a in ("%be:.=%")do endlocal&set/a %3=%cc%,%3+=%dy%*("%3>>31")&exit/b
