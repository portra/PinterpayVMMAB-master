#include "global.h"



int MenuAtmbArtajasa(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[4] =
	{
		{TRUE, _T_NOOP("PURCHASE"),		NULL},
		{TRUE, _T_NOOP("CEK TRANSAKSI"),		NULL},
		{TRUE, _T_NOOP("FUNCTION"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransAjPurchase();
		break;

	case 1:
		iRet = DisplayNotActive();
		//iRet = 0;
		break;

	case 2:
		iRet = MenuAtmbArtajasaFunction();
		break;
		


	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}




int MenuAtmbArtajasaFunction(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("LOG ON"),		NULL},
		{TRUE, _T_NOOP("NO REKENING"),		NULL},
		{TRUE, _T_NOOP("REPRINT LAST TRX"),		NULL},
		{TRUE, _T_NOOP("REPRINT BY TRACE"),		NULL},
		{TRUE, _T_NOOP("SUMMARY HARI INI"),		NULL},
		{TRUE, _T_NOOP("DETAIL HARI INI"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransLogonAj();
		break;

	case 1:
		iRet = InputNoRekeningAj();
		break;

	case 2:
		MiniAtmPrnLastTransBkpn();
		break;

	case 3:
		MiniAtmRePrnSpecTransBkpn();
		break;

	case 4:
		MiniATMBSummaryReport(FALSE);
		break;

	case 5:
		MiniATMBDetailReport(FALSE);
		break;



	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}





int TransAjPurchase(void)
{
	int		iRet;

	iRet = TransAjPurchaseInq(); // karena purchase AJ setelah inquery
	if (iRet == 0) {
		if( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			return 0;
		}
		
		iRet = TransAjPurchasePay();
		if (iRet == 0) {
			
		}	
	}
		
	return 0;
}



int TransAjPurchaseInq(void)
{
	int		iRet;	
	uchar	ucEntryMode=0;

	uchar f63[200];
	uchar	szTableID[2+1];
	uchar	szBenInID[11+1];
	uchar	szBenAcNum[28+1];
	uchar	szBenName[30+1];
	uchar	szCustRef[16+1];	
	uchar	szCustRef2[16+1];
	uchar	szIssInID[11+1];
	uchar	szCardAcNum[28+1];
	uchar	szCardName[30+1];
	uchar	szInfData[5+1];

	

	strcpy(glProcInfo.stTranLog.szTransName,"PURCHASE");
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  PURCHASE  ");

			
	iRet = TransInit(TRANS_TYPE_AJ_PAY_INQ);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	ScrCls();
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  PURCHASE  ");
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = InputUangAj();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  PURCHASE  ");
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}



	///// 63
	/*
	uchar f63[163];
	uchar	szTableID[2+1];
	uchar	szBenInID[11+1];
	uchar	szBenAcNum[28+1];
	uchar	szBenName[30+1];
	uchar	szCustRef[16+1];	
	uchar	szIssInID[11+1];
	uchar	szCardAcNum[28+1];
	uchar	szCardName[30+1];
	uchar	szInfData[5+1];
	*/

	MEM_ZERO(f63);
	MEM_ZERO(szTableID);
	MEM_ZERO(szBenInID);
	MEM_ZERO(szBenAcNum);
	MEM_ZERO(szBenName);
	MEM_ZERO(szCustRef);
	MEM_ZERO(szCustRef2);
	MEM_ZERO(szIssInID);
	MEM_ZERO(szCardAcNum);
	MEM_ZERO(szCardName);
	MEM_ZERO(szInfData);


		sprintf(szTableID, "%s","99");
		sprintf(szBenName, "%s","                              ");
		sprintf(szIssInID, "%s","           ");
		sprintf(szCardAcNum, "%s","                            ");
		sprintf(szCardName, "%s","                              ");
		sprintf(szInfData, "%s","GI001");
		sprintf(szBenInID, "%s","987");
		//sprintf(szBenAcNum, "%s","555000020941887084");
		strcpy(szBenAcNum,glSysCtrl.szNoRekeningAj);
		sprintf(szCustRef, "%s","PURCHASE");
		strcpy(szCustRef2,glCurTlmTmsAcq.acqTID);
		strcat(szCustRef,szCustRef2);

	
		///memcpy(glSendPack.sField63, "\x00\xA1", 2);
		PubAddHeadChars(szTableID, 2, '0');
		PubAddHeadChars(szBenInID, 11, '0');
		//PubAddHeadChars(szBenAcNum, 28, '0');
		StringPadRight(szBenAcNum, 28, " ");
		PubAddHeadChars(szBenName, 30, '0');
		//PubAddHeadChars(szCustRef, 16, '0');
		StringPadRight(szCustRef, 16, " ");
		PubAddHeadChars(szIssInID, 11, '0');
		PubAddHeadChars(szCardAcNum, 28, '0');
		PubAddHeadChars(szCardName, 30, ' ');
		PubAddHeadChars(szInfData, 5, '0'); // length 5 : GI001
		sprintf(f63, "%s%s%s%s%s%s%s%s%s", szTableID,szBenInID,szBenAcNum,szBenName,szCustRef,szIssInID,szCardAcNum,szCardName,szInfData);
		////sprintf((char *)&glSendPack.sField63[2], "%-161.161s", f63);
		strcpy(glProcInfo.szReq63,f63);




	//SET ISO 8583
	////// diki add komen  setPackInqTransferAntarBank();
	setPackAjPurchaseInq();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}


	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}


	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	

	//////displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


	// diki add
	GetNewInvoiceNo();
				
			

	return 0;
}


int TransAjPurchasePay(void)
{
	int		iRet;
	char 	bit63Rsp[200+1];
	ulong 	lenData;

	// TRANS_TYPE_AJ_PAY

	

	strcpy(glProcInfo.stTranLog.szTransName,"PURCHASE");
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  PURCHASE  ");

	iRet = TransInit(TRANS_TYPE_AJ_PAY);
	if( iRet!=0 )
	{
		return iRet;
	}


	MEM_ZERO(bit63Rsp);

	PubChar2Long(glRecvPack.szBit63, 2, &lenData);
	memcpy(bit63Rsp, glRecvPack.szBit63+2, lenData);

	strcpy(glProcInfo.szReq63,bit63Rsp);
	

	////// diki add komen  setPackTransferAntarBank();
	setPackAjPurchaseInq();

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	
	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	////// diki add 
	unPackAtmbAj();


	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
		
	//DISCONNECT FROM HOST
	CommOnHook(FALSE);
	

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukAj(PRN_NORMAL);
	}
	

	return 0;
}






int TransLogonAj(void)
{
	int		iRet;	
	uchar	ucEntryMode=0;
	uchar	ucAcqIndex, szTitle[16+1];
	uchar	ucRet;

	strcpy(glProcInfo.stTranLog.szTransName,"LOG ON");
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  LOG ON  ");

	/*
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
	*/

	ucRet = FindAcqIdxByName("ATMB_AJ", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);
	
			
	iRet = TransInit(LOGON_AJ);
	if( iRet!=0 )
	{
		return iRet;
	}


	ScrCls();
	ScrPrint(0, 0, CFONT|REVER, "%-16.16s", "  LOG ON  ");


	//SET ISO 8583
	////// diki add komen  setPackInqTransferAntarBank();
	setPackLogonAj();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}


	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}


	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}			

	return 0;
}







int InputUangAj(void)
{
	uchar	ucRet;
	ScrCls();
	DispTransName();
	PubDispString("JUMLAH :", DISP_LINE_LEFT|4);
	ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
						0, 9, glProcInfo.stTranLog.szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);

	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	//hex_dump("ttttttttttttt",glProcInfo.stTranLog.szAmount, 15);
	
	return 0;
}



int DisplayNotActive(void)
{

	ScrCls();
	PubBeepErr();
	PubDispStringASCII("TRX TIDAK AKTIF", DISP_LINE_CENTER|2);
	PubWaitKey(3);
	return 0;
}






//////////////////////////////////
///////// MENU PINTER PAY ////////////
//////////////////////////////////

int MenuPinterpay(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[4] =
	{
		{TRUE, _T_NOOP("PointTrans"),		NULL},
		{TRUE, _T_NOOP("Pulsa Elektrik"),		NULL},
		{TRUE, _T_NOOP("PPOB"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = DisplayNotActive();
		break;

	case 1:
		iRet = MenuPinterpayPulsaElektrik();
		//iRet = 0;
		break;

	case 2:
		iRet = MenuPinterpayPpob();
		break;
		


	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}




int MenuPinterpayPulsaElektrik(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[4] =
	{
		{TRUE, _T_NOOP("INQUERY TRANSACTION"),		NULL},
		{TRUE, _T_NOOP("PULSA TRANSACTION"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = DisplayNotActive();
		break;

	case 1:
		//iRet = TransTrfAntarBankBkpn();
		iRet = MenuPinterpayPulsaElektrikPulsa();
		break;


	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}


int MenuPinterpayPulsaElektrikPulsa(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[9] =
	{
		{TRUE, _T_NOOP("AXIS"),		NULL},
			{TRUE, _T_NOOP("ESIA"),		NULL},
			{TRUE, _T_NOOP("INDOSAT IM3"),		NULL},
			{TRUE, _T_NOOP("INDOSAT MENTARI"),		NULL},
			{TRUE, _T_NOOP("SMARTFREN"),		NULL},
			{TRUE, _T_NOOP("THREE"),		NULL},
			{TRUE, _T_NOOP("XL"),		NULL},
			{TRUE, _T_NOOP("TELKOMSEL"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = DisplayNotActive();
		break;

	case 1:
		iRet = DisplayNotActive();
		break;

	case 2:
		iRet = DisplayNotActive();
		break;
		
	case 3:
		iRet = DisplayNotActive();
		break;

	case 4:
		iRet = DisplayNotActive();
		break;

	case 5:
		iRet = DisplayNotActive();
		break;

	case 6:
		iRet = DisplayNotActive();
		break;
		
	case 7:
		iRet = DisplayNotActive();
		break;


	default:
		if(getkey()==KEYCANCEL)return ERR_NO_DISP;
	}

	return iRet;
}



int MenuPinterpayPpob(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stTranMenu[7] =
	{
		{TRUE, _T_NOOP("TELEPON"),		NULL},
			{TRUE, _T_NOOP("TV KABEL"),		NULL},
			{TRUE, _T_NOOP("INTERNET"),		NULL},
			{TRUE, _T_NOOP("MULTIFINANCE"),		NULL},
			{TRUE, _T_NOOP("PAM/PDAM"),		NULL},
			{TRUE, _T_NOOP("LISTRIK PLN"),		NULL}

	};
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:");

	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = DisplayNotActive();
		break;

	case 1:
		iRet = DisplayNotActive();
		break;

	case 2:
		iRet = DisplayNotActive();
		break;
		
	case 3:
		iRet = DisplayNotActive();
		break;

	case 4:
		iRet = DisplayNotActive();
		break;

	case 5:
		iRet = DisplayNotActive();
		break;

		default:
			if(getkey()==KEYCANCEL)return ERR_NO_DISP;
		}
	
	return iRet;
}




// char *StringPadRight(char *string, int padded_len, char *pad) {
void StringPadRight(char *string, int padded_len, char *pad) {	
	int i;
    int len = (int) strlen(string);
    if (len >= padded_len) {
        return string;
    }
    
    for (i = 0; i < padded_len - len; i++) {
        strcat(string, pad);
    }
    return string;
}

int InputNoRekeningAj(void)
{
	int		iRet;
	
	while (1)
	{
		ScrCls();
		PubDispString(_T("NO REK BANK :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 1, 28, glSysCtrl.szNoRekeningAj, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
        break;
	}
	ScrCls();
	SaveSysCtrlBase();
	

	return 0;
}





////////////////////////
//// PACK ISO //////////
////////////////////////


void setPackAjPurchaseInq(void)
{
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	////sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		////memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		//sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	//sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	//sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	//sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	//sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	//memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	//SetPosEntryMode();
	memcpy(&glSendPack.szBit22[0], "0021", 4);
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
		//==================================================================================
		sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
		sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
		//==================================================================================


//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	//memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 48
	//==================================================================================
	//PubLong2Char((ulong)39, 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 39);
	//==================================================================================

	////PubLong2Char((ulong)12, 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 147);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));

	//PubLong2Char((ulong)147, 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,147);

	//memcpy(&glSendPack.szBit48[2], "4848",4);



	//////////////PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	///////////memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
	

//BIT 49
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	////uchar kodecur[3+1];
	////strcpy(kodecur,"360");
	////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
	///////////memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 62, ROC/SOC
		//==================================================================================
		memcpy(glSendPack.szBit62, "\x00\x06", 2);
		sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
		//==================================================================================


//BIT 63
		//==================================================================================
		///PubLong2Char((ulong)SetBit63_AJ(), 2, glSendPack.szBit63);
		///sprintf((char *)&(glSendPack.szBit63[2]), "%s", bit63req);
		PubLong2Char((ulong)strlen(glProcInfo.szReq63), 2, glSendPack.szBit63);
		memcpy(&glSendPack.szBit63[2], glProcInfo.szReq63,strlen(glProcInfo.szReq63));
		//==================================================================================
			


//BIT 64
		//==================================================================================
		////PubLong2Char(8, 2, glSendPack.sMac);
		////sprintf((char *)&(glSendPack.sMac[2]), "%s", "00000000");
		memcpy(&glSendPack.sMac[0], "\x00\x08", 2);
		memcpy(&glSendPack.sMac[2], "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
		//==================================================================================




}




void setPackLogonAj(void)
{

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	////sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
		//==================================================================================
		///sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		////memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		//sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	//sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	//sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	//sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	//sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	//memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	////SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
		//==================================================================================
	///	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	///	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
		//==================================================================================


//BIT 35
	//==================================================================================
	/////sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	//memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================



}



//// UNPACK

uchar unPackAtmbAj(void)
{
	uchar buffer1[999];
	uchar buffer2[28+1];
	uchar buffer3[28+1];
	uchar buffAmt[14+1];
	uchar buffAmt2[14+1];

	//fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total, buffAmt, 14);

	//fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,0,",.");
	memcpy(buffAmt2,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmt,buffAmt2,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PUR_AJ.Total, buffAmt, 12);
	

	return TRUE;
}


//////// CETAK STRUK


int CetakStrukAj(uchar ucPrnFlag)
{
	word 	mnuItm;
	int i =0;
	int i2 =0;
	int i3 =1;
	mapGetWord(traMnuItm, mnuItm);
	
	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return 0;
	}

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SSB:
			PrnReceiptSSB(ucPrnFlag);
			return 0;
			break;
	}

	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return 0;
	}

for (i=0; i<=1; i++)

	{

		PrnInit();
		PrnSetNormal();

		//LOGO
		PrnCustomLogo_T();

		//SITE NAME & ADDRESS
		PrnHeader();

		if(ucPrnFlag)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}	
		//TID MID
		PrnHeaderTidMid();

		PrnHeaderDateTime();
		
		//PrnHeaderCardNumber();
		PrnHeaderCardNumberBkpn();

		PrnHeaderTraceRespCode();

		PrnHeaderBatchRefNum();

		PrnStr("\n");
		if(glProcInfo.stTranLog.ucTranType == TRANS_TYPE_TRFANTARBANK_BKPN)
			{
				// gak dicetak 
			}
		else {
		PrnHeaderTransactionName();
		}

		//switch(glProcInfo.stTranLog.ucTranType)
		//{
		//case TRANS_TYPE_AJ_PAY:
				PrnPurchaseAj();
				PrnFooterAj();
		//		break;				
		//}

		if(i2==i)
		{
			PrnStr("\n");
			PrnStr("       --------- MERCHANT COPY ---------       ");		
		} else if(i3==i)
		{
			PrnStr("\n");
			PrnStr("       --------- CUSTOMER COPY ---------       ");		
		}


		if(ucPrnFlag)
		{
				PrnStr("\n");
				if(ucPrnFlag)
		{
				PrnStr("\n");
				PrnStr("          **********REPRINT**********          ");		
		}		
		}
	
		PrnStr("\n\n\n\n\n\n\n\n");
		StartPrinter();

	}

	return 0;
}	


void PrnHeaderTraceRespCode(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "TRACE : %-25.06lu RESP CODE : %02s", glProcInfo.stTranLog.ulInvoiceNo,  glProcInfo.stTranLog.szRspCode);

	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
}


void PrnFooterAj(void)
{

	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	/*
	PrnStr("\n\n");
	MultiLngPrnStr("HARAP TANDA TERIMA INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
	*/


/*
	MultiLngPrnStr("MENGGUNAKAN TRANSFER\n", DISP_LINE_CENTER);
	MultiLngPrnStr("ANTAR BANK\n", DISP_LINE_CENTER);
	MultiLngPrnStr("BIAYA TRANSAKSI SESUAI\n", DISP_LINE_CENTER);
	MultiLngPrnStr("KETENTUAN BANK PENERBIT\n", DISP_LINE_CENTER);
	MultiLngPrnStr("KARTU ANDA\n", DISP_LINE_CENTER);
	MultiLngPrnStr("*** Terima Kasih ***\n", DISP_LINE_CENTER);
	*/


	MultiLngPrnStr("MENGGUNAKAN TRANSFER ANTAR BANK\n", DISP_LINE_CENTER);
	MultiLngPrnStr("BIAYA TRANSAKSI SESUAI KETENTUAN\n", DISP_LINE_CENTER);
	MultiLngPrnStr("BANK PENERBIT KARTU ANDA\n", DISP_LINE_CENTER);
	MultiLngPrnStr("*** Terima Kasih ***\n", DISP_LINE_CENTER);
	
	
}




void PrnPurchaseAj(void)
{
	uchar	printTotal[48+1];
	
	MEM_ZERO(printTotal);

	sprintf((char *)printTotal,  "Rp. %s", glProcInfo.stTranLog.BRI.PUR_AJ.Total);

	//PrnSetBig();
	PrnStr("\n");
	PrnStr("\n");
	MultiLngPrnStr("AMOUNT :", DISP_LINE_LEFT);
	PrnStr("\n");

	MultiLngPrnStr(printTotal, DISP_LINE_CENTER);
	PrnSetNormal();
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("\n");
	
}


void MiniATMBSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	//ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	ucRet = FindAcqIdxByName("ATMB_AJ", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date,  TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintSummaryBkpn(TotalInfo, TotalCnt, TotalAmt);
}



void MiniATMBDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	//ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	ucRet = FindAcqIdxByName("ATMB_AJ", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintDetailBkpn( date, TRUE);
}


