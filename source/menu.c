#include "global.h"
#include "menu.h"
#include "Vector.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/


/********************** Internal variables declaration *********************/

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/
extern int  PaymentOptionsMenu(void);


int DispNama(){
	int ucKey;
	uchar szSaldo[15];
	uchar nilai[12];
	uchar szBuff[50];

	ScrCls();
	DispTransName();
	memset(nilai,'0',sizeof(nilai));
	memset(szSaldo,' ',sizeof(szSaldo));
	sprintf((char *)szSaldo, "MERCHANT ");
	PubDispString(szSaldo, 2|DISP_LINE_CENTER);
	memset(szBuff, 0, sizeof(szBuff));
	sprintf(szBuff,"%s",glProcInfo.stTranLog.szNamaPelanggan);
	PubDispString(szBuff, 4|DISP_LINE_CENTER);
	ucKey = PubWaitKey(USER_OPER_TIMEOUT);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		return ERR_NO_DISP;
	}
	return ucKey;
}

int DispInquery(){
	int ucKey;
	uchar szBuff[50];

	ScrCls();
	DispTransName();
	memset(szBuff, ' ', sizeof(szBuff));
	sprintf(szBuff,"%s",glProcInfo.stTranLog.szNamaPelanggan);
	PubShowMsg(2,szBuff);
	ucKey = PubWaitKey(USER_OPER_TIMEOUT);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		return ERR_NO_DISP;
	}
	return ucKey;
}

int DispOtherPackage(){
	int ucKey;
	ScrCls();
	DispTransName();
	PubDispString(_T_NOOP("ENTER TO ADD"), 2|DISP_LINE_CENTER);
	PubDispString(_T_NOOP("PACKAGE"), 4|DISP_LINE_CENTER);
	PubDispString(_T_NOOP("CANCLE TO EXIT"), 6|DISP_LINE_CENTER);
	ucKey = PubWaitKey(USER_OPER_TIMEOUT);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		counterPackage=0;
		memset(glPackageList,0,sizeof(glPackageList));
		return ERR_NO_DISP;
	}
	return TransInqPinterpay();
}



int DispInqueryPpob(){
		uchar szName[22+1];
		uchar szParam[22+1];
		uchar szBuffer[50];
		uchar szBuff[50];
		int		ucKey,n,a=0,i=0,b=0,ii=0;
		int 	iRet, iMenuNo;
		static	MenuItem stTranMenu[50], menuItm;
		if (1)
		{
			   CommOnHook(FALSE);
		}
		ScrCls();
		memset(stTranMenu,0,sizeof(stTranMenu));
		memset(glProcInfo.stTranLog.szParamName,'0',sizeof(glProcInfo.stTranLog.szParamName));
		for(n=0;n<responseCounter;n++){
			memset(szBuffer,' ',sizeof(szBuffer));
			memset(szName,' ',sizeof(szName));
			memset(szParam,' ',sizeof(szParam));
			while(1){
				if(memcmp(&glRecvPack.szBit62[5+i],";",1)==0){
					i++;
					a=0;
					break;
				}
				memcpy(&szName[a],&glRecvPack.szBit62[5+i],1);
				a++;
				i++;
			}
			if(glProcInfo.stTranLog.szParamName==NULL){
				sprintf((char *)szBuffer, "%.18s", szName);
			}else{
				sprintf((char *)szBuffer, "%.17s:", szName);
			}
			menuItm.bVisible = TRUE;
			strcpy(menuItm.szMenuName,szBuffer);
			menuItm.pfMenuFunc = NULL;
			stTranMenu[n*2]= menuItm;
			while(1){
				if(memcmp(&glRecvPack.szBit63[5+ii],";",1)==0){
					memset(szBuff,'0',sizeof(szBuff));
					sprintf((char *)szBuff, "%.17s", szParam);
					b=0;
					ii++;
					break;
				}
				memcpy(&szParam[b],&glRecvPack.szBit63[5+ii],1);
				b++;
				ii++;
			}
			menuItm.bVisible = TRUE;
			strcpy(menuItm.szMenuName,szBuff);
			menuItm.pfMenuFunc = NULL;
			stTranMenu[(n*2)+1]= menuItm;
		}
		iMenuNo = PubGetMenu((uchar *)_T("INQUERY PPOB"), stTranMenu, MENU_NOSPEC|MENU_ASCII, USER_OPER_TIMEOUT);
		switch( iMenuNo )
			{
			
			default:
				if(getkey()==KEYCANCEL){
					iRet=PpobMenu();
				}else{
					iRet=0;
				}
			}
		//iRet=iMenuNo;
		return iRet;
}

int DispErrMessage(){
		uchar szName[22+1];
		uchar szParam[22+1];
		uchar szBuffer[50];
		int		ucKey,n;
		int 	iRet, iMenuNo;
		static	MenuItem stTranMenu[50], menuItm;
		if (1)
		{
			   CommOnHook(FALSE);
		}
		ScrCls();
		//menuItm=EmptyMenuItm;
		memset(stTranMenu,0,sizeof(stTranMenu));
		for(n=0;n<responseCounter;n++){
			memset(szBuffer,'0',sizeof(szBuffer));
			memset(szName,'0',sizeof(szName));
			memset(szParam,'0',sizeof(szParam));
			memcpy(glProcInfo.stTranLog.szParamName,&glRecvPack.szBit48[5+n*22],22);
			sprintf((char *)szBuffer, "%.22s", glProcInfo.stTranLog.szParamName);
			menuItm.bVisible = TRUE;
			strcpy(menuItm.szMenuName,szBuffer);
			menuItm.pfMenuFunc = NULL;
			stTranMenu[n]= menuItm;
		}
		iMenuNo = PubGetMenu((uchar *)_T("ERROR MESSAGE"), stTranMenu, MENU_NOSPEC|MENU_ASCII, USER_OPER_TIMEOUT);
		switch( iMenuNo )
			{
			
			default:
				if(getkey()==KEYCANCEL)iRet=PpobMenu();
			}
		return 0;
}


int DispInqueryPPay(){
		uchar szBuff[50];
		uchar szBuffer[50];
		int		ucKey;
		int 	iRet, iMenuNo;
		static	MenuItem stTranMenu[4], menuItm;
		if (1)
		{
			   CommOnHook(FALSE);
		}
		ScrCls();
		DispTransName();
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "%s", glProcInfo.stTranLog.szMerchName);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[0]= menuItm;
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "%s", glProcInfo.stTranLog.szMemberName);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[1]= menuItm;
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "Package : %s", glProcInfo.stTranLog.szPackageName);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[2]= menuItm;
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "Price : %s", glProcInfo.stTranLog.szPackagePrice);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[3]= menuItm;
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "Poin AVB : %s", glProcInfo.stTranLog.szPackageDiscount);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[4]= menuItm;
		memset(szBuffer,'0',sizeof(szBuffer));
		sprintf((char *)szBuffer, "Poin Paket : %s", glProcInfo.stTranLog.szPackagePoin);
		menuItm.bVisible = TRUE;
		strcpy(menuItm.szMenuName,szBuffer);
		menuItm.pfMenuFunc = NULL;
		stTranMenu[5]= menuItm;
		iMenuNo = PubGetMenu((uchar *)_T("INQUERYPOINT"), stTranMenu, MENU_NOSPEC|MENU_ASCII, 0);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( iMenuNo==KEYCANCEL || iMenuNo==NOKEY )
		{
			counterPackage=0;
			return ERR_NO_DISP;
		}
		return 0;
}


int PpobMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("TELEPON    "),		NULL},
		{TRUE, _T_NOOP("TV KABEL "),		NULL},
		{TRUE, _T_NOOP("INTERNET "),		NULL},		
		{TRUE, _T_NOOP("MULTIFINANCE "),	NULL},
		{TRUE, _T_NOOP("PAM/PDAM "),		NULL},
		{TRUE, _T_NOOP("LISTRIK PLN"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = PpobTeleponMenu();
		break;

	case 1:
		iRet = PpobTvKabelMenu();
		break;

	case 2:
		iRet = PpobInternetMenu();
		break;

	case 3:
		iRet = PpobMultifinanceMenu();
		break;
	case 4:
		iRet = PpobPAMMenu();
		break;
	case 5:
		iRet = PpobPLNMenu();
		break;

	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}


int PpobTeleponMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("Telepon Telkom    "),		NULL},
		{TRUE, _T_NOOP("Halo Telkomsel "),		NULL},
		{TRUE, _T_NOOP("Xplor XL "),		NULL},		
		{TRUE, _T_NOOP("Matrix Indosat "),	NULL},
		{TRUE, _T_NOOP("Esia Pasca Payar"),		NULL},

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInqPpob("TLP","telepon");
		break;

	case 1:
		iRet = TransInqPpob("TLP","halo");
		break;

	case 2:
		iRet = TransInqPpob("TLP","xplor");
		break;

	case 3:
		iRet = TransInqPpob("TLP","matrix");
		break;
	case 4:
		iRet = TransInqPpob("TLP","easia");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PpobTvKabelMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("Aora TV    "),		NULL},
		{TRUE, _T_NOOP("Telkomvision "),		NULL},
		{TRUE, _T_NOOP("Indovison "),		NULL},		
		{TRUE, _T_NOOP("Yes TV "),	NULL},
		{TRUE, _T_NOOP("Top TV "),		NULL},
		{TRUE, _T_NOOP("Okevision"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInqPpob("TVK","Aora");
		break;

	case 1:
		iRet = TransInqPpob("TVK","Telkomvision");
		break;

	case 2:
		iRet = TransInqPpob("TVK","Indovision");
		break;

	case 3:
		iRet = TransInqPpob("TVK","Yestv");
		break;
	case 4:
		iRet = TransInqPpob("TVK","Toptv");
		break;
	case 5:
		iRet = TransInqPpob("TVK","Okevision");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PpobInternetMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[2] =
	{
		{TRUE, _T_NOOP("Speedy    "),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInqPpob("NET","speedy");
		break;

	default:		
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PpobMultifinanceMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[5] =
	{
		{TRUE, _T_NOOP("WOM Finance    "),		NULL},
		{TRUE, _T_NOOP("Mega Central Finance "),		NULL},
		{TRUE, _T_NOOP("Mega Auto Finance "),		NULL},		
		{TRUE, _T_NOOP("Adira Finance "),	NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInqPpob("MFN","wom");
		break;

	case 1:
		iRet = TransInqPpob("MFN","megacentral");
		break;

	case 2:
		iRet = TransInqPpob("MFN","meagaauto");
		break;

	case 3:
		iRet = TransInqPpob("MFN","adira");
		break;
	default:		
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PpobPAMMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[61] =
	{
		
		{TRUE, _T_NOOP("WAAETRA"),NULL},
		{TRUE, _T_NOOP("WAPLYJ"),NULL},
		{TRUE, _T_NOOP("WABEKASI"),NULL},
		{TRUE, _T_NOOP("WAPLMBNG"),NULL},
		{TRUE, _T_NOOP("WASBY"),NULL},
		{TRUE, _T_NOOP("WABGK"),NULL},
		{TRUE, _T_NOOP("WABONDO"),NULL},
		{TRUE, _T_NOOP("WAJPR"),NULL},
		{TRUE, _T_NOOP("WABJN"),NULL},
		{TRUE, _T_NOOP("WAKOBGR"),NULL},
		{TRUE, _T_NOOP("WACLCP"),NULL},
		{TRUE, _T_NOOP("WASLMN"),NULL},
		{TRUE, _T_NOOP("WABANJAR"),NULL},
		{TRUE, _T_NOOP("WASDA"),NULL},
		{TRUE, _T_NOOP("WABDG"),NULL},
		{TRUE, _T_NOOP("WAMLG"),NULL},
		{TRUE, _T_NOOP("WAJAMBI"),NULL},
		{TRUE, _T_NOOP("WALMPNG"),NULL},
		{TRUE, _T_NOOP("WAMJK"),NULL},
		{TRUE, _T_NOOP("WATAPIN"),NULL},
		{TRUE, _T_NOOP("WABALIKPPN"),NULL},
		{TRUE, _T_NOOP("WABOGOR"),NULL},
		{TRUE, _T_NOOP("WAGROBGAN"),NULL},
		{TRUE, _T_NOOP("WAKABMLG"),NULL},
		{TRUE, _T_NOOP("WAKUBURAYA"),NULL},
		{TRUE, _T_NOOP("WAMAKASAR"),NULL},
		{TRUE, _T_NOOP("WAMANADO"),NULL},
		{TRUE, _T_NOOP("WAPONTI"),NULL},
		{TRUE, _T_NOOP("WASMG"),NULL},
		{TRUE, _T_NOOP("WAMEDAN"),NULL},
		{TRUE, _T_NOOP("WABAL"),NULL},
		{TRUE, _T_NOOP("WAJMBR"),NULL},
		{TRUE, _T_NOOP("WASITU"),NULL},
		{TRUE, _T_NOOP("WALOMBOKT"),NULL},
		{TRUE, _T_NOOP("WAGIRIMM"),NULL},
		{TRUE, _T_NOOP("WAKNDL"),NULL},
		{TRUE, _T_NOOP("WABYMS"),NULL},
		{TRUE, _T_NOOP("WABYL"),NULL},
		{TRUE, _T_NOOP("WABREBES"),NULL},
		{TRUE, _T_NOOP("WABULELENG"),NULL},
		{TRUE, _T_NOOP("WADPSR"),NULL},
		{TRUE, _T_NOOP("WADEPOK"),NULL},
		{TRUE, _T_NOOP("WAKABBDG"),NULL},
		{TRUE, _T_NOOP("WAKBMN"),NULL},
		{TRUE, _T_NOOP("WAKPKLNGAN"),NULL},
		{TRUE, _T_NOOP("WAPROLING"),NULL},
		{TRUE, _T_NOOP("WAKABSMG"),NULL},
		{TRUE, _T_NOOP("WASRAGEN"),NULL},
		{TRUE, _T_NOOP("WAKRWNG"),NULL},
		{TRUE, _T_NOOP("WAKARANGA"),NULL},
		{TRUE, _T_NOOP("WAKLATEN"),NULL},
		{TRUE, _T_NOOP("WAPBLINGGA"),NULL},
		{TRUE, _T_NOOP("WAPURWORE"),NULL},
		{TRUE, _T_NOOP("WAREMBANG"),NULL},
		{TRUE, _T_NOOP("WATEMANGG"),NULL},
		{TRUE, _T_NOOP("WAWONOGIRI"),NULL},
		{TRUE, _T_NOOP("WAWONOSB"),NULL},
		{TRUE, _T_NOOP("WASLTIGA"),NULL},
		{TRUE, _T_NOOP("WAMADIUN"),NULL},
		{TRUE, _T_NOOP("WABERAU"),NULL}
		

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{		
		case 0:
			iRet = TransInqPpob("PAM","WAAETRA");
		break;
		case 1:
			iRet = TransInqPpob("PAM","WAPLYJ");
		break;
		case 2:
			iRet = TransInqPpob("PAM","WABEKASI");
		break;
		case 3:
			iRet = TransInqPpob("PAM","WAPLMBNG");
		break;
		case 4:
			iRet = TransInqPpob("PAM","WASBY");
		break;
		case 5:
			iRet = TransInqPpob("PAM","WABGK");
		break;
		case 6:
			iRet = TransInqPpob("PAM","WABONDO");
		break;
		case 7:
			iRet = TransInqPpob("PAM","WAJPR");
		break;
		case 8:
			iRet = TransInqPpob("PAM","WABJN");
		break;
		case 9:
			iRet = TransInqPpob("PAM","WAKOBGR");
		break;
		case 10:
			iRet = TransInqPpob("PAM","WACLCP");
		break;
		case 11:
			iRet = TransInqPpob("PAM","WASLMN");
		break;
		case 12:
			iRet = TransInqPpob("PAM","WABANJAR");
		break;
		case 13:
			iRet = TransInqPpob("PAM","WASDA");
		break;
		case 14:
			iRet = TransInqPpob("PAM","WABDG");
		break;
		case 15:
			iRet = TransInqPpob("PAM","WAMLG");
		break;
		case 16:
			iRet = TransInqPpob("PAM","WAJAMBI");
		break;
		case 17:
			iRet = TransInqPpob("PAM","WALMPNG");
		break;
		case 18:
			iRet = TransInqPpob("PAM","WAMJK");
		break;
		case 19:
			iRet = TransInqPpob("PAM","WATAPIN");
		break;
		case 20:
			iRet = TransInqPpob("PAM","WABALIKPPN");
		break;
		case 21:
			iRet = TransInqPpob("PAM","WABOGOR");
		break;
		case 22:
			iRet = TransInqPpob("PAM","WAGROBGAN");
		break;
		case 23:
			iRet = TransInqPpob("PAM","WAKABMLG");
		break;
		case 24:
			iRet = TransInqPpob("PAM","WAKUBURAYA");
		break;
		case 25:
			iRet = TransInqPpob("PAM","WAMAKASAR");
		break;
		case 26:
			iRet = TransInqPpob("PAM","WAMANADO");
		break;
		case 27:
			iRet = TransInqPpob("PAM","WAPONTI");
		break;
		case 28:
			iRet = TransInqPpob("PAM","WASMG");
		break;
		case 29:
			iRet = TransInqPpob("PAM","WAMEDAN");
		break;
		case 30:
			iRet = TransInqPpob("PAM","WABAL");
		break;
		case 31:
			iRet = TransInqPpob("PAM","WAJMBR");
		break;
		case 32:
			iRet = TransInqPpob("PAM","WASITU");
		break;
		case 33:
			iRet = TransInqPpob("PAM","WALOMBOKT");
		break;
		case 34:
			iRet = TransInqPpob("PAM","WAGIRIMM");
		break;
		case 35:
			iRet = TransInqPpob("PAM","WAKNDL");
		break;
		case 36:
			iRet = TransInqPpob("PAM","WABYMS");
		break;
		case 37:
			iRet = TransInqPpob("PAM","WABYL");
		break;
		case 38:
			iRet = TransInqPpob("PAM","WABREBES");
		break;
		case 39:
			iRet = TransInqPpob("PAM","WABULELENG");
		break;
		case 40:
			iRet = TransInqPpob("PAM","WADPSR");
		break;
		case 41:
			iRet = TransInqPpob("PAM","WADEPOK");
		break;
		case 42:
			iRet = TransInqPpob("PAM","WAKABBDG");
		break;
		case 43:
			iRet = TransInqPpob("PAM","WAKBMN");
		break;
		case 44:
			iRet = TransInqPpob("PAM","WAKPKLNGAN");
		break;
		case 45:
			iRet = TransInqPpob("PAM","WAPROLING");
		break;
		case 46:
			iRet = TransInqPpob("PAM","WAKABSMG");
		break;
		case 47:
			iRet = TransInqPpob("PAM","WASRAGEN");
		break;
		case 48:
			iRet = TransInqPpob("PAM","WAKRWNG");
		break;
		case 49:
			iRet = TransInqPpob("PAM","WAKARANGA");
		break;
		case 50:
			iRet = TransInqPpob("PAM","WAKLATEN");
		break;
		case 51:
			iRet = TransInqPpob("PAM","WAPBLINGGA");
		break;
		case 52:
			iRet = TransInqPpob("PAM","WAPURWORE");
		break;
		case 53:
			iRet = TransInqPpob("PAM","WAREMBANG");
		break;
		case 54:
			iRet = TransInqPpob("PAM","WATEMANGG");
		break;
		case 55:
			iRet = TransInqPpob("PAM","WAWONOGIRI");
		break;
		case 56:
			iRet = TransInqPpob("PAM","WAWONOSB");
		break;
		case 57:
			iRet = TransInqPpob("PAM","WASLTIGA");
		break;
		case 58:
			iRet = TransInqPpob("PAM","WAMADIUN");
		break;
		case 59:
			iRet = TransInqPpob("PAM","WABERAU");
		break;
		
		default:			
			if(getkey()==KEYCANCEL)iRet=PpobMenu();
			return ERR_NO_DISP;
	}

	return iRet;
}


int PpobPLNPraMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("20.000"),		NULL},
		{TRUE, _T_NOOP("50.000"),		NULL},
		{TRUE, _T_NOOP("100.000"),		NULL},
		{TRUE, _T_NOOP("200.000"),		NULL},
		{TRUE, _T_NOOP("500.000"),		NULL},
		{TRUE, _T_NOOP("1.000.000"),	NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA20");
		iRet = TransInqPpob("PLN","PRA");
		break;

	case 1:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA50");
		iRet = TransInqPpob("PLN","PRA");
		break;
	case 2:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA100");
		iRet = TransInqPpob("PLN","PRA");
		break;

	case 3:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA200");
		iRet = TransInqPpob("PLN","PRA");
		break;
	case 4:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA500");
		iRet = TransInqPpob("PLN","PRA");
		break;

	case 5:
		strcpy(glProcInfo.stTranLog.szVoucher,"PLA1000");
		iRet = TransInqPpob("PLN","PRA");
		break;

	default:
		
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PpobPLNMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[3] =
	{
		{TRUE, _T_NOOP("PLN PRA    "),		NULL},
		{TRUE, _T_NOOP("PLN PASCA "),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = PpobPLNPraMenu();
		break;

	case 1:
		iRet = TransInqPpob("PLN","PAS");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PpobMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}


int PulsaPurchaseMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[9] =
	{
		{TRUE, _T_NOOP("Axis    "),		NULL},
		{TRUE, _T_NOOP("Esia  "),		NULL},
		{TRUE, _T_NOOP("Indosat IM3 "),		NULL},
		{TRUE, _T_NOOP("Indosat Mentari "),		NULL},
		{TRUE, _T_NOOP("Smartfren  "),		NULL},
		{TRUE, _T_NOOP("Three  "),		NULL},
		{TRUE, _T_NOOP("Xl  "),		NULL},
		{TRUE, _T_NOOP("Simpati"),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = AxisMenu();
		break;

	case 1:
		iRet = EsiaMenu();
		break;

	case 2:
		iRet = IndosatM2Menu();
		break;
		
	case 3:
		iRet = IndosatMentariMenu();
		break;

	case 4:
		iRet = FrenMenu();
		break;

	case 5:
		iRet = ThreeMenu();
			break;

	case 6:
		iRet = XlMenu();
		break;
	case 7:
		iRet = TelkomselMenu();
		break;
		
	default:
		return ERR_NO_DISP;
	}

	return iRet;
}

int UnderCons(){
	ScrCls();
	PubDispString(_T("UNDER"), 2|DISP_LINE_CENTER);
	PubDispString(_T("CONSTRACTION"), 4|DISP_LINE_CENTER);
	PubWaitKey(3);
	return 0;

}

int CheckSaldoStat(){
	uchar szSaldo[15];
	uchar nilai[12];
	uchar szBuff[50];
	ScrCls();
	DispTransName();
	memset(nilai,'0',sizeof(nilai));
	memset(szSaldo,' ',sizeof(szSaldo));
	sprintf((char *)szSaldo, "SALDO REK %s", glProcInfo.stTranLog.szRekening);
	PubDispString(szSaldo, 2|DISP_LINE_CENTER);
	memset(szBuff, 0, sizeof(szBuff));
	App_ConvAmountTran(glProcInfo.stTranLog.szNilai, szBuff, 0);
	PubDispString(szBuff, 4|DISP_LINE_CENTER);
	PubWaitKey(-1);
	return 0;

}


int CheckPackageList(){
	uchar szBuffer[50];
	int		iRet, iMenuNo, n;
	static	MenuItem menuzz;
	//static  MenuItem *m,menuzz[28];
	static  MenuItem m[28];
	//menuzz=EmptyMenuItm;
	memset(m,0,sizeof(m));
	ScrCls();
	DispTransName();
	//m=menuzz;	

	for(n=0;n<counterPackage;n++){
		memset(szBuffer,0,sizeof(szBuffer));
		sprintf((char *)szBuffer, "Paket : %s (%d)", glPackageList[n].sPackageCode, glPackageList[n].uiTotal);
		menuzz.bVisible = TRUE;
		strcpy(menuzz.szMenuName,_T_NOOP(szBuffer));
		menuzz.pfMenuFunc = NULL;
		m[n]=menuzz;
	}
	if(n==1){
		sprintf((char *)szBuffer, "             ");
		menuzz.bVisible = TRUE;
		strcpy(menuzz.szMenuName,_T_NOOP(szBuffer));
		menuzz.pfMenuFunc = NULL;
		m[1]=menuzz;
	}
	iMenuNo = PubGetMenu((uchar *)_T("DAFTAR PAKET : "), m, MENU_NOSPEC|MENU_ASCII, 0);

	iRet = PubWaitKey(USER_OPER_TIMEOUT);
	if( iRet==KEYCANCEL || iRet==NOKEY )
	{
		return DispOtherPackage();
	}
	for(n=0;n<counterPackage;n++){
		if(memcmp(glProcInfo.stTranLog.szTrxReff,glPackageList[n].sPackageCode,8)==0){
				glPackageList[n].price=atoi(glRecvPack.szBit4);
				break;
		}
	}

	return PaymentOptionsMenu();

}


int CheckBakiStat(){
	uchar szBuff[50];
	uchar szBuffer[50];
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[4], menuItm;
	if (1)
	{
		   CommOnHook(FALSE);
	}
	ScrCls();
	DispTransName();
	memset(szBuffer,'0',sizeof(szBuffer));
	sprintf((char *)szBuffer, "REKENING : %s", glProcInfo.stTranLog.szRekening);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[0]= menuItm;
	memset(szBuff, 0, sizeof(szBuff));
	App_ConvAmountTran(glProcInfo.stTranLog.szNilai, szBuff, 0);
	sprintf((char *)szBuffer, "Nilai : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[1]= menuItm;
	App_ConvAmountTran(glProcInfo.stTranLog.szBaki, szBuff, 0);
	sprintf((char *)szBuffer, "Baki : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[2]= menuItm;
	iMenuNo = PubGetMenu((uchar *)_T("CHECKBAKI"), stTranMenu, MENU_NOSPEC|MENU_ASCII, USER_OPER_TIMEOUT);
	return 0;

}

int CheckAngsuranStat(){
	uchar szBuff[50];
	uchar szBuffer[50];
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[4], menuItm;
	if (1)
	{
		   CommOnHook(FALSE);
	}
	ScrCls();
	DispTransName();
	memset(szBuffer,'0',sizeof(szBuffer));
	//ScrFontSet(1);
	sprintf((char *)szBuffer, "REKENING : %s", glProcInfo.stTranLog.szRekening);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[0]= menuItm;
	memset(szBuff, 0, sizeof(szBuff));
	App_ConvAmountTran(glProcInfo.stTranLog.szNilai, szBuff, 0);
	sprintf((char *)szBuffer, "Nilai : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[1]= menuItm;
	App_ConvAmountTran(glProcInfo.stTranLog.szPokok, szBuff, 0);
	sprintf((char *)szBuffer, "Pokok : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[2]= menuItm;
	App_ConvAmountTran(glProcInfo.stTranLog.szBunga, szBuff, 0);
	sprintf((char *)szBuffer, "Bunga : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[3]= menuItm;
	App_ConvAmountTran(glProcInfo.stTranLog.szDenda, szBuff, 0);
	sprintf((char *)szBuffer, "Denda : %s", szBuff);
	menuItm.bVisible = TRUE;
	strcpy(menuItm.szMenuName,szBuffer);
	menuItm.pfMenuFunc = NULL;
	stTranMenu[4]= menuItm;
	iMenuNo = PubGetMenu((uchar *)_T("CHECKANGSURAN"), stTranMenu, MENU_NOSPEC|MENU_ASCII, USER_OPER_TIMEOUT);
	return 0;

}

/*int TabunganMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[3] =
	{
		{TRUE, _T_NOOP("Cek Saldo    "),		NULL},
		{TRUE, _T_NOOP("Setoran "),		NULL},
		{TRUE, _T_NOOP("Tarikan  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransCheckSaldo();
		break;

	case 1:
		iRet = TransSetoran();
		break;

	case 2:
		iRet = TransTarikan();
		break;


	default:
		return ERR_NO_DISP;
	}

	return iRet;
}*/

int PinterPayMenu(void)
	
{
	int		iRet=0, iMenuNo;
	uchar	ucTranType;
	static	MenuItem stTranMenu[4] =
	{
		{TRUE, _T_NOOP("PoinTrans"),	NULL},
		{TRUE, _T_NOOP("Pulsa Elektrik"),	NULL},
		{TRUE, _T_NOOP("PPOB"),	NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		TransInqPinterpay();
		break;

	case 1:
		iRet = TransactionInqueryMenu();
		break;

	case 2:
		iRet = PpobMenu();
		break;

	//case 3:
		/*TransMicro();
		break;*/

	//case 4:
		/*TransMicro();
		break;*/

	default:
		return ERR_NO_DISP;
	}

	CommOnHook(FALSE);

	return iRet;
}


int TransactionInqueryMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[3] =
	{
		{TRUE, _T_NOOP("INQUERY TRANSACTION"),		NULL},
		{TRUE, _T_NOOP("PULSA TRANSACTION"),		NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInqPulsa();
		break;

	case 1:
		iRet = PulsaPurchaseMenu();
		break;


	default:
		return ERR_NO_DISP;
	}

	return iRet;
}


int PaymentOptionsMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[5] =
	{
		{TRUE, _T_NOOP("Tunai"),		NULL},
		{TRUE, _T_NOOP("Debit SAP"),		NULL},
		{TRUE, _T_NOOP("ATM Bersama"),		NULL},
		{TRUE, _T_NOOP("Kartu Kredit"),		NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPinterpay("PINTERPAY");
		memset(glPackageList,0,sizeof(glPackageList));
		counterPackage=0;
		break;

	case 1:
		iRet = TransPinterpay("PINTERPAY");
		memset(glPackageList,0,sizeof(glPackageList));
		counterPackage=0;
		break;

	case 2:
		glProcInfo.stTranLog.ppaj=1;

		/*
		iRet = Inquery_AJ(); // karena purchase AJ setelah inquery
		if (iRet == 0) {

			if( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
			{
				break;
			}
			

			iRet = Purchase_AJ();			
		}
		*/


		iRet = TransAjPurchaseInq(); // karena purchase AJ setelah inquery
		if (iRet == 0) {
		if( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			break;
		}
		
		iRet = TransAjPurchasePay();

		}
		
	
		glProcInfo.stTranLog.ppaj=0;
		break;
		
	case 3:
		///UnderCons();
		TransSale();
		break;

	default:
		memset(glPackageList,0,sizeof(glPackageList));
		counterPackage=0;
		return ERR_NO_DISP;
	}

	return iRet;
}


/*int KreditMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[3] =
	{
		{TRUE, _T_NOOP("Cek BAKI    "),		NULL},
		{TRUE, _T_NOOP("Cek TUNGGAKAN "),		NULL},
		{TRUE, _T_NOOP("ANGSURAN  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransCheckBaki();
		break;

	case 1:
		iRet = TransCheckAngsuran();
		break;

	case 2:
		iRet = TransAngsuran();
		break;

	default:
		return ERR_NO_DISP;
	}

	return iRet;
}
*/

int AxisMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BAX100","PUL","005");
		break;

	case 1:
		iRet = TransPurchaseItem("BAX50","PUL","005");
		break;

	case 2:
		iRet = TransPurchaseItem("BAX25","PUL","005");
		break;

	case 3:
		iRet = TransPurchaseItem("BAX10","PUL","005");
		break;

	case 4:
		iRet = TransPurchaseItem("BAX5","PUL","005");
			break;

	/*case 5:
		iRet = TransPurchaseItem("BAX1","PUL","005");
		break;*/

	default:
	
	if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int CeriaMenu(void)
{
	int		iRet=0, iMenuNo;
	/*static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("10.000 "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("50.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
		{TRUE, _T_NOOP("1.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
			iRet = TransPurchaseItem("C100");
			break;

		case 1:
			iRet = TransPurchaseItem("C10");
			break;

		case 2:
			iRet = TransPurchaseItem("C20");
			break;

		case 3:
			iRet = TransPurchaseItem("C50");
			break;

		case 4:
			iRet = TransPurchaseItem("C5");
				break;

		case 5:
			iRet = TransPurchaseItem("C1");
			break;

	default:
		return ERR_NO_DISP;
	}
*/
	return iRet;
}

int EsiaMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[8] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("15.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
		{TRUE, _T_NOOP("1.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
			case 0:
				iRet = TransPurchaseItem("BE100","PUL","3");
				break;

			case 1:
				iRet = TransPurchaseItem("BE50","PUL","3");
				break;

			case 2:
				iRet = TransPurchaseItem("BE25","PUL","3");
				break;

			case 3:
				iRet = TransPurchaseItem("BE20","PUL","3");
				break;

			case 4:
				iRet = TransPurchaseItem("BE15","PUL","3");
					break;

			case 5:
				iRet = TransPurchaseItem("BE10","PUL","3");
				break;
				
			case 6:
				iRet = TransPurchaseItem("BE5","PUL","3");
				break;

	default:
		
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int FlexyMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	/*static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("10.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("50.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
				iRet = TransPurchaseItem("F100");
				break;

			case 1:
				iRet = TransPurchaseItem("F10");
				break;

			case 2:
				iRet = TransPurchaseItem("E25");
				break;

			case 3:
				iRet = TransPurchaseItem("E50");
				break;

			case 4:
				iRet = TransPurchaseItem("E5");
					break;


	default:
		return ERR_NO_DISP;
	}*/

	return iRet;
}

int FrenMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BSF100","PUL","3");
		break;

	case 1:
		iRet = TransPurchaseItem("BSF50","PUL","3");
		break;

	case 2:
		iRet = TransPurchaseItem("BSF25","PUL","3");
		break;

	case 3:
		iRet = TransPurchaseItem("BSF20","PUL","3");
		break;
	case 4:
		iRet = TransPurchaseItem("BSF10","PUL","3");
		break;
	case 5:
		iRet = TransPurchaseItem("BSF5","PUL","3");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int HepiMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	/*static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("10.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("50.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("H100");
		break;

	case 1:
		iRet = TransPurchaseItem("H10");
		break;

	case 2:
		iRet = TransPurchaseItem("H25");
		break;

	case 3:
		iRet = TransPurchaseItem("H50");
		break;

	case 4:
		iRet = TransPurchaseItem("H5");
			break;

	default:
		return ERR_NO_DISP;
	}
*/
	return iRet;
}

int IndosatM2Menu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[9] =
	{
		{TRUE, _T_NOOP("REG 100.000"),		NULL},
		{TRUE, _T_NOOP("REG 50.000 "),		NULL},
		{TRUE, _T_NOOP("REG 25.000 "),		NULL},
		{TRUE, _T_NOOP("SMS 25.000 "),		NULL},
		{TRUE, _T_NOOP("REG 10.000 "),		NULL},
		{TRUE, _T_NOOP("SMS 10.000"),		NULL},
		{TRUE, _T_NOOP("REG 5.000  "),		NULL},
		{TRUE, _T_NOOP("SMS 5.000  "),		NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BI100","PUL","4");
		break;

	case 1:
		iRet = TransPurchaseItem("BI50","PUL","4");
		break;

	case 2:
		iRet = TransPurchaseItem("BI25","PUL","4");
		break;

	case 3:
		iRet = TransPurchaseItem("BIS25","PUL","4");
		break;

	case 4:
		iRet = TransPurchaseItem("BI10","PUL","4");
		break;

	case 5:
		iRet = TransPurchaseItem("BIS10","PUL","4");
		break;

	case 6:
		iRet = TransPurchaseItem("BI5","PUL","4");
		break;
	
	case 7:
		iRet = TransPurchaseItem("BIS5","PUL","4");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int IndosatMentariMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[9] =
	{
		{TRUE, _T_NOOP("REG 100.000"),		NULL},
		{TRUE, _T_NOOP("REG 50.000 "),		NULL},
		{TRUE, _T_NOOP("REG 25.000 "),		NULL},
		{TRUE, _T_NOOP("SMS 25.000 "),		NULL},
		{TRUE, _T_NOOP("REG 10.000 "),		NULL},
		{TRUE, _T_NOOP("SMS 10.000"),		NULL},
		{TRUE, _T_NOOP("REG 5.000  "),		NULL},
		{TRUE, _T_NOOP("SMS 5.000  "),		NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BM100","PUL","4");
		break;

	case 1:
		iRet = TransPurchaseItem("BM50","PUL","4");
		break;

	case 2:
		iRet = TransPurchaseItem("BM25","PUL","4");
		break;

	case 3:
		iRet = TransPurchaseItem("BMS25","PUL","4");
		break;

	case 4:
		iRet = TransPurchaseItem("BM10","PUL","4");
		break;

	case 5:
		iRet = TransPurchaseItem("BMS10","PUL","4");
		break;

	case 6:
		iRet = TransPurchaseItem("BM5","PUL","4");
		break;
	
	case 7:
		iRet = TransPurchaseItem("BMS5","PUL","4");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int SmartMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	/*static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("20.000    "),		NULL},
		{TRUE, _T_NOOP("10.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("50.000  "),		NULL},
		{TRUE, _T_NOOP("100.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("CM20");
		break;

	case 1:
		iRet = TransPurchaseItem("CM10");
		break;

	case 2:
		iRet = TransPurchaseItem("CM25");
		break;

	case 3:
		iRet = TransPurchaseItem("CM50");
		break;

	case 4:
		iRet = TransPurchaseItem("CM100");
			break;

	default:
		return ERR_NO_DISP;
	}*/

	return iRet;
}

int StarOneMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	/*static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("10.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("50.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("O100");
		break;

	case 1:
		iRet = TransPurchaseItem("O10");
		break;

	case 2:
		iRet = TransPurchaseItem("O25");
		break;

	case 3:
		iRet = TransPurchaseItem("O50");
		break;

	case 4:
		iRet = TransPurchaseItem("O5");
			break;

	default:
		return ERR_NO_DISP;
	}
*/
	return iRet;
}

int TelkomselMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[6] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BS100","PUL","7");
		break;

	case 1:
		iRet = TransPurchaseItem("BS50","PUL","7");
		break;

	case 2:
		iRet = TransPurchaseItem("BS25","PUL","7");
		break;

	case 3:
		iRet = TransPurchaseItem("BS20","PUL","7");
		break;

	case 4:
		iRet = TransPurchaseItem("BS10","PUL","7");
			break;
	case 5:
		iRet = TransPurchaseItem("BS5","PUL","7");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int ThreeMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("30.000 "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL},
		{TRUE, _T_NOOP("1.000  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BT100","PUL","6");
		break;

	case 1:
		iRet = TransPurchaseItem("BT50","PUL","6");
		break;

	case 2:
		iRet = TransPurchaseItem("BT30","PUL","6");
		break;

	case 3:
		iRet = TransPurchaseItem("BT20","PUL","6");
		break;

	case 4:
		iRet = TransPurchaseItem("BT10","PUL","6");
			break;

	case 5:
		iRet = TransPurchaseItem("BT5","PUL","6");
		break;
		
	case 6:
		iRet = TransPurchaseItem("BT1","PUL","6");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int XlMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("100.000    "),		NULL},
		{TRUE, _T_NOOP("50.000 "),		NULL},
		{TRUE, _T_NOOP("25.000  "),		NULL},
		{TRUE, _T_NOOP("20.000  "),		NULL},
		{TRUE, _T_NOOP("10.000  "),		NULL},
		{TRUE, _T_NOOP("5.000  "),		NULL}
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPurchaseItem("BX100","PUL","1");
		break;

	case 1:
		iRet = TransPurchaseItem("BX50","PUL","1");
		break;

	case 2:
		iRet = TransPurchaseItem("BX25","PUL","1");
		break;

	case 3:
		iRet = TransPurchaseItem("BX20","PUL","1");
		break;

	case 4:
		iRet = TransPurchaseItem("BX10","PUL","1");
			break;

	case 5:
		iRet = TransPurchaseItem("BX5","PUL","1");
		break;

	default:
		if(getkey()==KEYCANCEL)iRet=PulsaPurchaseMenu();
		return ERR_NO_DISP;
	}

	return iRet;
}

int GamePurchaseMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[3] =
	{
		{TRUE, _T_NOOP("Game School    "),		NULL},
		{TRUE, _T_NOOP("Lyto "),		NULL},
		{TRUE, _T_NOOP("Megaxus  "),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = UnderCons();
		break;

	case 1:
		iRet = UnderCons();
		break;

	case 2:
		iRet = UnderCons();
		break;

	return iRet;
	}
}

int PLNPurchaseMenu(void)
{
	int		iRet, iMenuNo;
//uchar	ucTranType;
	static	MenuItem stTranMenu[2] =
	{
		{TRUE, _T_NOOP("Prabayar (Token)"),		NULL},
	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransPLNPra();
		break;

	default:
		return ERR_NO_DISP;
	}

	return iRet;
}

