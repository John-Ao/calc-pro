@echo off
setlocal enabledelayedexpansion
set "title=Calc Standard - Ao"
title %title%
for /l %%i in (1,1,1000) do @set ".%%i=%%i"
set /a "varreplace=1,timing=1"
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
set "in.%in.acc%=!in.%in.acc%: =!"
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
			if "!%1.%%i!"=="*" set "%1.%%i.type=mlt"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="/" set "%1.%%i.type=div"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="\" set "%1.%%i.type=div2"&set "%1.%%i.prv=6"
			if "!%1.%%i!"=="^" set "%1.%%i.type=pow"&set "%1.%%i.prv=7"
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
	echo,Error:!%1.ematch! ='s are found!
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
	echo,Multi- or Invalid input!
	set "%2=(!%1.nums.1!"
	for /l %%i in (2,1,!%1.nums.length!) do (
		set "%2=!%2!,!%1.nums.%%i!"
	)
	set "%2=!%2!)"
) else (
	echo,Invalid input!
))
exit /b

:test _______________________________________________________________
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

:concatnum __________________________________________________________
set "ccn.now="
set "ccn.flag.old="
for /l %%A in (1,1,!%1.length!) do (
	set "%1.now=!%1.%%A!"
	set "ccn.flag=0"
	for %%B in (.=0=1=2=3=4=5=6=7=8=9) do (
		if "!%1.now!"=="%%B" set "ccn.flag=num"
	)
	for %%B in (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) do (
		if /i "!%1.now!"=="%%B" set "ccn.flag=alp"
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
)
call :push %2 ccn.now
exit /b

:push _______________________________________________________________
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

:pop ________________________________________________________________
set /a "pop.len=!%1.length!+0"
if %pop.len% equ 0 set "%2="&exit /b
set "%2=!%1.%pop.len%!"
set /a "%1.length-=1"
exit /b

:return _____________________________________________________________
set "%1.re="
if "!%1.length!"=="0" exit /b
set "return="
for /l %%i in (1,1,!%1.length!) do (
set "%1.re=!%1.re!!return!!%1.%%i![!%1.%%i.type!]"
set "return= "
)
echo,%1=!%1.re!
exit /b

:arrto [v.arr] [v.str] [v.start] [v.end] [delims]____________________
set "arrto="
set "delims="
for /l %%i in (!%3!,1,!%4!) do (
set "arrto=!arrto!!delims!!%1.%%i!"
set "delims=%5"
)
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
call :arrto func.!%1.1!.known.arr.!lf.acc! lf.gklist .1 func.!%1.1!.known.arr.!lf.acc!.length ,
set "func.!%1.1!.known.gklist.!lf.gklist!=!lf.acc!"
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
call :arrto %4.fapply.toknow fapply.gklist .1 %4.fapply.toknow.length ,
set "func.%1.known.gklist.!fapply.gklist!=!lf.acc!"
)
exit /b

:getknown [function] [arr toknow] [return] [host]____________________
set "%3="
call :arrto %2 gklisttoknow .1 %2.length ,
call :myset gk.match func.%1.known.gklist.!gklisttoknow!
if "%gk.match%"=="" (
for /l %%i in (1,1,!func.%1.known.length!) do (
	call :equarr !func.%1.known.%%i! %2 gk
	if "!gk!"=="1" set "gk.match=%%i"
)
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
				set "%3=0"
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





































::________________________Scientific Calculation_________________________
:add
set /a "%3=%1+%2"
exit /b

:mnu
set /a "%3=%1-%2"
exit /b

:mlt
set /a "%3=%1*%2"
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

:pow
set "pow=1"
set "topow=%1"
set "powto=%2"
:pow.l
set /a "powmod=powto%%2"
if %powto% equ 0 (
	set "%3=!pow!"
goto :pow.d
)
if %powmod% neq 0 set /a "pow*=topow"
set /a "powto>>=1,topow*=topow"
goto :pow.l
:pow.d
exit /b

:etime <begin_time> <end_time> <return>
setlocal&set be=%~1:%~2&set cc=(%%d-%%a)*360000+(1%%e-1%%b)*6000+1%%f-1%%c&set dy=-8640000
for /f "delims=: tokens=1-6" %%a in ("%be:.=%")do endlocal&set/a %3=%cc%,%3+=%dy%*("%3>>31")&exit/b
