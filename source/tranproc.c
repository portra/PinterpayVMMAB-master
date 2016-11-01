
#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static void PackInvoice(STISO8583 *pstSendPack, ulong ulInvoice);
static void ModifyProcessCode(void);
static void SetEntryMode(TRAN_LOG *pstLog);
static void SetAmexEntryMode(TRAN_LOG *pstLog);
static void SetStdEntryMode(TRAN_LOG *pstLog);
static void SetCondCode(void);
static int  TransCashSub(void);
static int  TransSaleManualSub(void);
static int  TransInstallmentSub(void);
static int  TransAuthSub(uchar ucTranType);
static int  GetOfflineTrans(uchar ucTypeFlag);
static int  AdjustInput(void);
static int  TransSettleSub(void);
static int  TransUpLoad(void);

static int TransPinVerifySub(void);	//irf, 20121129

/********************** Internal variables declaration *********************/

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

void PackInvoice(STISO8583 *pstSendPack, ulong ulInvoice)
{
	memcpy(pstSendPack->szBit62, "\x00\x06", 2);
	sprintf((char *)&(pstSendPack->szBit62[2]), "%06lu", ulInvoice);
}



// 设置请求包的公共bits
// set bit content of ISO8583.
// Work as the "IsoInitSendMsg()" did in old style.
void SetCommReqField(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];

//	hex_dump_char("glProcInfo.stTranLog.ucTranType", &glProcInfo.stTranLog.ucTranType, sizeof(glProcInfo.stTranLog.ucTranType));

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:


			if(PrivateLabelCardDetect)
			{
				SetPackSalePrvtLbl();
			}
			else {
			SetPackSale();
			}
			
			return;
#ifdef BANK_BRI
		case TRANS_TYPE_LOGON:
			setPackLogon();
			return;
		case TRANS_TYPE_LOGON_BRI:
			setPackLogon();
			return;
#endif
	}
	
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		memset(&glTMSSend, 0, sizeof(STTMS8583));
		sprintf((char *)glTMSSend.szMsgCode, "%.*s", LEN_MSG_CODE,
				glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
		sprintf((char *)glTMSSend.szProcCode,   "%.*s", LEN_PROC_CODE,
				glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
#ifdef	__TELIUM_PARAM__
		sprintf((char *)glTMSSend.szNii,    "%.*s", LEN_NII,     glCurTlmTmsAcq.acqNII);
		sprintf((char *)glTMSSend.szTermID, "%.*s", LEN_TERM_ID, glCurTlmTmsAcq.acqTID);
#else
		sprintf((char *)glTMSSend.szNii,    "%.*s", LEN_NII,     glCurAcq.szNii);
		sprintf((char *)glTMSSend.szTermID, "%.*s", LEN_TERM_ID, glCurAcq.szTermID);
#endif
		sprintf((char *)glTMSSend.szSTAN, "%06lu", glSysCtrl.ulSTAN);

		return;
	}

	//ISO8583使用说明
	//Step3: 在这里设置打包时需要的数据，该域赋值则送，为空时则不送(默认的初始值全为空)
	//所有的报文都需要用到的域可在//set common bits下进行统一赋值
	//其它根据交易不同条件不同时可送可不送的分别处理
	//另注意sz型与s型开头成员变量,sz型直接赋所需要上送的值，s型需要在前两位赋值为该域的长度
	//例如:	// bit 62, ROC/SOC定义为  szBit62
	//则先赋长度值memcpy(glSendPack.szBit62, "\x00\x06", 2);
	//再赋数据值sprintf((char *)&glSendPack.szBit62[2], "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	// Usage of ISO8583 module (For step 2, see in st8583.h)
	// Step 3: To set the data need in packing here.
	// If the glSendPack.xxxxx is filled with value, this bit will be sent, and vice versa.

	// Note that there're 2 kinds of member in glSendPack:
	//   glSendPack.szxxxxxx : this type means "string end with zero", the actual length can be determined by strlen()
	//   glSendPack.sxxxxxx  : the first 2 bytes contains length infomation, if length=0x01A0, should be "\x01\xA0"
	//     e.g. : bit62
	//             memcpy(glSendPack.szBit62, "\x00\x06", 2);
	//             sprintf((char *)&glSendPack.szBit62[2], "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	// set common bits

	//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

	//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

	//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	// modify bit 3, process code
	ModifyProcessCode();
	//==================================================================================
	
	
#ifdef	__TELIUM_PARAM__
	//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

	//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================
	
	//BIT 42
		//==================================================================================
		sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
		//sprintf((char *)glSendPack.szBit42, "%-15s", glCurTlmTmsAcq.acqMID+5);
		//==================================================================================

#else
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurAcq.szNii);
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurAcq.szTermID);
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurAcq.szMerchantID);
#endif

	//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
	
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BATCH_UPLOAD ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOGON )
	{
		return;
	}


	// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		//PubAddHeadChars(szTotalAmt, 12, '0');		no need: already 12 digits
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
	}
	else
	{
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	}
	//==================================================================================
	
	if( (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFFLINE_SEND) ||
		(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TC_SEND) )
	{

	//BIT 12
	//==================================================================================
		sprintf((char *)glSendPack.szBit12, "%.6s",  &glProcInfo.stTranLog.szDateTime[8]);
	//==================================================================================

	//BIT 13
	//==================================================================================
		sprintf((char *)glSendPack.szBit13, "%.4s",  &glProcInfo.stTranLog.szDateTime[4]);
	//==================================================================================

	//BIT 23
	//==================================================================================
	if(PrivateLabelCardDetect)
	{
			///SetPackSalePrvtLbl();
	}
	else {
		/////}
			
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
		{	
		if( glProcInfo.stTranLog.bPanSeqOK )
			{	
				sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
			}
		}
	}
	//==================================================================================

	//BIT 37
	//==================================================================================
		sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	//==================================================================================

	//BIT 38
	//==================================================================================
		sprintf((char *)glSendPack.szBit38,  "%.6s",  glProcInfo.stTranLog.szAuthCode);
	//==================================================================================
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID)
	{
	//BIT 12
	//==================================================================================
		sprintf((char *)glSendPack.szBit12, "%.6s",  &glProcInfo.stTranLog.szDateTime[8]);
	//==================================================================================

	//BIT 13
	//==================================================================================
		sprintf((char *)glSendPack.szBit13, "%.4s",  &glProcInfo.stTranLog.szDateTime[4]);
	//==================================================================================
	
	
	//BIT 23
	//==================================================================================	
	if (glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_REDEEMPTION )
	{		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
		{	
		if( glProcInfo.stTranLog.bPanSeqOK )
			{	
				sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
			}
		}
	}
	//==================================================================================

	//BIT 37
	//==================================================================================
		sprintf((char *)glSendPack.szBit37,       "%.12s", glProcInfo.stTranLog.szRRN);       // jiale 2006.12.12
	//==================================================================================
	

	//BIT 38
	//==================================================================================
		sprintf((char *)glSendPack.szBit38,  "%.6s",  glProcInfo.stTranLog.szAuthCode);	// jiale for void send 37.38field
	//==================================================================================
		}
	else {
	//BIT 23
	//==================================================================================	
			if( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) ||
						 (glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS) )
			{	
			if( glProcInfo.stTranLog.bPanSeqOK )
				{	
					sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
				}
			}
	//==================================================================================
		}
	// PAN/track 1/2/3/expiry etc
	if( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) ||
						 (glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS) )
	{
		if( (glProcInfo.szTrack2[0]==0) ||
			(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFFLINE_SEND) ||
			(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TC_SEND) )
		{

	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
			 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFFLINE_SEND || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID )
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
			sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
			 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}

	//BIT 22
	//==================================================================================
	if(PrivateLabelCardDetect)
	{
			///SetPackSalePrvtLbl();
			memcpy(&glSendPack.szBit22[0], "0051", 4);
	}
	else {
		/////}
	SetEntryMode(&glProcInfo.stTranLog);		// bit 22, entry mode
		}
	//==================================================================================

	//BIT 25
	//==================================================================================
	SetCondCode();		// bit 25, service condition code
	//==================================================================================

	//BIT 48
	//==================================================================================
	// bit 48 or 55, CVV2 or 4DBC
	if( ChkIfNeedSecurityCode())
	{
		memcpy(glSendPack.szBit48, "\x00\x03", 2);
		sprintf((char *)&glSendPack.szBit48[2], "%-3.3s", glProcInfo.szSecurityCode);
	}
	
	if( glProcInfo.stTranLog.ucTranType==INQPINTERPAY)
		{
			uchar f48[20];
			uchar rekening[12+1];
			uchar szlen[4];
			int len=0;
			len=strlen(glProcInfo.stTranLog.szRekening);
			sprintf(szlen,"%d",len);
			PubAddHeadChars(szlen, 4, '0');
			if (len <= 0xFFFF)
			{
				sprintf(&szlen[0], "%04x", len);
			}
			PubAsc2Bcd(szlen, 4, glSendPack.szBit48);
			strcpy(rekening,glProcInfo.stTranLog.szRekening);
			PubAddHeadChars(rekening, len, '0');
			sprintf(f48, "%s", rekening);
			sprintf((char *)&glSendPack.szBit48[2], "%-*.*s",len, len,f48);
		}
	
		
		if( glProcInfo.stTranLog.ucTranType==PINTERPAY)
		{
			uchar szBuffer[999],szQty[3],szPackageCode[8];
			int len = 0;
			int n=0;
			uchar szlen[4];
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf((char *)glSendPack.szBit12, "%.6s",  &glProcInfo.stTranLog.szDateTime[8]);
			sprintf((char *)glSendPack.szBit13, "%.4s",  &glProcInfo.stTranLog.szDateTime[4]);
			//field 63
			for(n;n<counterPackage;n++){
				memset(szQty, 0, sizeof(szQty));
				memset(szPackageCode, 0, sizeof(szPackageCode));
				sprintf(szPackageCode,"%8.8s",glPackageList[n].sPackageCode);
				sprintf(szQty,"%3.3d",glPackageList[n].uiTotal);
				PubAddHeadChars(szPackageCode, strlen(szPackageCode), '0');
				PubAddHeadChars(szQty, strlen(szQty), '0'); 
				sprintf(szBuffer,"%s%8.8s%3.3s",szBuffer,szPackageCode,szQty);
			}
			len=counterPackage*11;
			sprintf(szlen,"%d",len);
			PubAddHeadChars(szlen, 4, '0');
			if (len <= 0xFFFF)
			{
				sprintf(&szlen[0], "%04x", len);
			}
			PubAsc2Bcd(szlen, 4, glSendPack.szBit63);
			sprintf((char *)&glSendPack.szBit63[2], "%-*.*s",len, len,szBuffer);
		}
	
		if( glProcInfo.stTranLog.ucTranType==INQPPOB)
		{
			uchar f48[66];
			uchar rekening[22];
			uchar product[22];
			uchar productCode[22];
			uchar szlen[4];
			int len=22;
			memset(rekening, 0, sizeof(rekening));
			memset(product, 0, sizeof(product));
			memset(productCode, 0, sizeof(productCode));
			//memcpy(rekening,glProcInfo.stTranLog.szAccount, sizeof(rekening));
			//memcpy(product,glProcInfo.stTranLog.szKodePpob, sizeof(product));
			//memcpy(productCode,glProcInfo.stTranLog.szKodeProduk, sizeof(productCode));
			strcpy(rekening,glProcInfo.stTranLog.szAccount);
			strcpy(product,glProcInfo.stTranLog.szKodePpob);
			strcpy(productCode,glProcInfo.stTranLog.szKodeProduk);
			//PubAddHeadChars(rekening, 22, ' ');
			//PubAddHeadChars(product, 22, ' '); 
			//PubAddHeadChars(productCode, 22, ' ');
			sprintf(f48,"%22.22s%22.22s%22.22s",rekening,product,productCode);
			//sprintf(f48,"%s%s%s",rekening,product,productCode);
			/*len=strlen(f48);
			sprintf(szlen,"%d",len);
			PubAddHeadChars(szlen, 4, '0');
			if (len <= 0xFFFF)
			{
				sprintf(&szlen[0], "%04x", len);
			}
			PubAsc2Bcd(szlen, 4, glSendPack.szBit48);*/
			memcpy(glSendPack.szBit48, "\x00\x42", 2);
			sprintf((char *)&glSendPack.szBit48[2], "%-*.*s",66, 66,f48);
			//sprintf((char *)&glSendPack.szBit48[2],"%-22.22s%-22.22s%-22.22s",rekening,product,productCode);
			//sprintf((char *)&glSendPack.szBit48[2],"%22.22s%22.22s%22.22s",rekening,product,productCode);
		}
	
		if( glProcInfo.stTranLog.ucTranType==PPOB)
		{
			uchar f48[88];
			uchar rekening[22];
			uchar product[22];
			uchar productCode[22];
			uchar reff[22];
			uchar szlen[4];
			int len=66;
			memset(rekening, 0, sizeof(rekening));
			memset(product, 0, sizeof(product));
			memset(productCode, 0, sizeof(productCode));
			strcpy(rekening,glProcInfo.stTranLog.szAccount);			
			strcpy(product,glProcInfo.stTranLog.szKodePpob);
			strcpy(productCode,glProcInfo.stTranLog.szKodeProduk);
			if(memcmp(product,"PLN",3)==0&&memcmp(productCode,"PRA",3)==0){
				strcpy(reff,glProcInfo.stTranLog.szVoucher);
			}else{
				strcpy(reff,glProcInfo.stTranLog.szTrxReff);
			}
			//PubAddHeadChars(rekening, 22, ' ');
			//PubAddHeadChars(product, 22, ' '); 
			//PubAddHeadChars(productCode, 22, ' ');
			//PubAddHeadChars(reff, 22, ' ');
			sprintf(f48,"%22.22s%22.22s%22.22s",rekening,product,glProcInfo.stTranLog.szKodeProduk);
			/*len=strlen(f48);
			sprintf(szlen,"%d",len);
			PubAddHeadChars(szlen, 4, '0');
			if (len <= 0xFFFF)
			{
				sprintf(&szlen[0], "%04x", len);
			}
			PubAsc2Bcd(szlen, 4, glSendPack.szBit48);*/
			memcpy(glSendPack.szBit48, "\x00\x42", 2);
			sprintf((char *)&glSendPack.szBit48[2], "%-*.*s",len, len,f48);
			memcpy(glSendPack.szBit63, "\x00\x22", 2);
			sprintf((char *)&glSendPack.szBit63[2], "%-*.*s",22, 22,reff);
		}
		
		if( glProcInfo.stTranLog.ucTranType==PULSA )
		{
			uchar f48[88];
			uchar noHp[22];
			uchar kodeProduk[22];
			uchar kodePpob[22];
			uchar szlen[4];
			uchar provider[22];
			int len=88;
			strcpy(provider,glProcInfo.stTranLog.szProvider);
			strcpy(kodePpob,glProcInfo.stTranLog.szKodePpob);
			strcpy(kodeProduk,glProcInfo.stTranLog.szKodeProduk);
			strcpy(noHp,glProcInfo.stTranLog.szNoHP);
			//PubAddHeadChars(provider, 22, ' ');
			//PubAddHeadChars(kodePpob, 22, ' ');
			//PubAddHeadChars(kodeProduk, 22, ' ');
			//PubAddHeadChars(noHp, 22, ' ');
			sprintf(f48, "%22.22s%22.22s%22.22s%22.22s", kodePpob,kodeProduk,noHp, provider);
			len=strlen(f48);
			sprintf(szlen,"%d",len);
			/*PubAddHeadChars(szlen, 4, '0');
			if (len <= 0xFFFF)
			{
				sprintf(&szlen[0], "%04x", len);
			}
			PubAsc2Bcd(szlen, 4, glSendPack.szBit48);*/
			memcpy(glSendPack.szBit48, "\x00\x58", 2);
			sprintf((char *)&glSendPack.szBit48[2], "%-*.*s",len, len,f48);
			/*strcpy(kodeProduk,glProcInfo.stTranLog.szKodeProduk);
			strcpy(noHp,glProcInfo.stTranLog.szNoHP);
			memcpy(glSendPack.szBit48, "\x00\x25\x01", 3);
			PubAddHeadChars(kodeProduk, 12, ' ');
			PubAddHeadChars(noHp, 12, ' ');
			strncpy(temp, noHp,12);
			sprintf(f48, "%s%s", kodeProduk,temp);
			sprintf((char *)&glSendPack.szBit48[3], "%-24.24s", f48);*/
		}
		
		//PLNPRA
			if( glProcInfo.stTranLog.ucTranType==PLNPRA )
			{
				uchar f48[20];
				uchar nohp[12+1];
				uchar rekening[12+1];
				uchar nilai[12+1];
				strcpy(nilai,glProcInfo.stTranLog.szAccount);
				strcpy(rekening,glProcInfo.stTranLog.szNilai);
				strcpy(nohp,glProcInfo.stTranLog.szNoHP);
				memcpy(glSendPack.szBit48, "\x00\x24", 2);
				PubAddHeadChars(rekening, 12, '0');
				PubAddHeadChars(nilai, 12, ' ');
				PubAddHeadChars(nohp, 12, '0');
				sprintf(f48, "%s%s%s", rekening,nilai,nohp);
				sprintf((char *)&glSendPack.szBit48[2], "%-36.36s", f48);
			}
	//==================================================================================

	

	//BIT 49
	//==================================================================================
//	memcpy(glSendPack.szBit49, "360", 3);//irf, 20121129	//IDR code
	//==================================================================================

	//55
	//==================================================================================
	//hex_dump_char("BIT 55", "00", 2);
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CASH    ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT||glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE_REDEEMPTION)
	{


	if(PrivateLabelCardDetect)
		{
			
		}else {
		////}
			
#ifdef ENABLE_EMV
		if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
		}



	}
	//==================================================================================

//BIT 57
#ifdef BANK_BRI
	if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		PubLong2Char((ulong)8, 2, glSendPack.szBit57);
		memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	}
#endif

	//BIT 62, ROC/SOC
	//==================================================================================
	PackInvoice(&glSendPack, glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

	// bit 63
	// ...

	if( ChkIfNeedMac() )
	{
		PubLong2Char((ulong)LEN_MAC, 2, glSendPack.sMac);
	}


if (glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_REDEEMPTION )
	{	
	//BIT 63
	//==================================================================================
	PubLong2Char((ulong)105, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2],glProcInfo.stTranLog.BRI.SaleRedeemption.Buff63, sizeof (105));

	//BIT 63
	//==================================================================================
	//PubLong2Char((ulong)105, 2, glSendPack.szBit63);
	//memcpy(&glSendPack.szBit63[2],glProcInfo.szReq63SaleRedeemption, 105);
	//==================================================================================
	//==================================================================================	
	//hex_dump("TEST", glProcInfo.stTranLog.BRI.SaleRedeemption.Buff63, 105);
	
	}
}
// Bit 3 definition:
// AMEX 0200:
// For a sale transaction									00 40 0x
// For a refund transaction									20 40 0x
// For an on line void of on line sale						02 40 0x
// For an on line void of on line refund					22 40 0x
// For an on line void of off line sale seen by host		02 40 0x
// For an on line void of off line sale not seen by host	00 40 0x
//
// AMEX 0220:
// For a sale transaction (referred/sale comp or off line sale)
//													00 40 0x
// For an sale adjustment (i.e. add tip or void of sale):
//		When original sale not seen by host			00 40 0x
//		When original sale seen by host				02 40 0x
// For an off line refund transaction				20 40 0x
// For a void refund transaction:
//		When Trickle fed refund to host				22 40 0x
//		When Void off line refund not trickle fed	20 40 0x
void ModifyProcessCode(void)
{
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFFLINE_SEND )
		{
			if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE     ||
				glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_OFF_SALE ||
				glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_COMP )
			{
				if( glProcInfo.stTranLog.uiStatus & (TS_ADJ|TS_VOID) )
				{
					if( glProcInfo.stTranLog.szRRN[0]!=0 )
					{
						glSendPack.szBit3[1] = '2';
					}
				}
			}
			else if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_REFUND )
			{
				glSendPack.szBit3[0] = '2';
				if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
				{
					if( glProcInfo.stTranLog.szRRN[0]!=0 )
					{
						glSendPack.szBit3[1] = '2';
					}
				}
			}
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID)
		{
			if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE )
			{
				glSendPack.szBit3[1] = '2';
			}
			else if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_REFUND )
			{
				glSendPack.szBit3[0] = '2';
				glSendPack.szBit3[1] = '2';
			} 
			else if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_REDEEMPTION)
			{
				glSendPack.szBit3[2] = '0';
			} 
		}

        // Processing code 3rd digit
		// ...

    PubStrUpper(glSendPack.szBit3);
}

void SetEntryMode(TRAN_LOG *pstLog)
{
	sprintf((char *)glSendPack.szBit22, "0000");
	SetStdEntryMode(pstLog);
}
//Pinterpay{
int SetAmt()
{
	uint n;
	ulong price;
	while (1)
	{
		ScrCls();
		price=0;
		for(n=0;n<counterPackage;n++){
			price+=glPackageList[n].price*glPackageList[n].uiTotal;
		}
		if((glProcInfo.stTranLog.ppaj<=0&&glProcInfo.stTranLog.ucTranType!=PPAYAJ)||(glProcInfo.stTranLog.ppaj<=0&&glProcInfo.stTranLog.ucTranType!=PINTERPAY)||glProcInfo.stTranLog.ucTranType!=PPAYAJ){
			if(glProcInfo.stTranLog.ucTranType==PPAYAJ)sprintf(glProcInfo.stTranLog.szAmount,"%d",price);
			PubDispString(_T("Addition :"), 2|DISP_LINE_LEFT);
		}else{
			PubDispString(_T("Amount :"), 2|DISP_LINE_LEFT);
		}
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 12, glProcInfo.stTranLog.szAmount, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
		//StringPadRight(glProcInfo.stTranLog.szAmount, 13, "0");
		PubAddHeadChars(glProcInfo.stTranLog.szAmount, 13, '0');
        break;
	}
	ScrCls();
	return 0;
}

int SetAcc(){
	uchar szBuff[12];
	memset(szBuff,0,12);
	while (1)
	{
		ScrCls();
		PubDispString(_T("Kode Paket :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 8, szBuff, USER_OPER_TIMEOUT)!=0)
        {
        	counterPackage=0;
            return ERR_USERCANCEL;
        }
        break;
	}
	strcpy(glProcInfo.stTranLog.szRekening,szBuff);
	//strcpy(glPackageList[counterPackage].sPackageCode,glProcInfo.stTranLog.szRekening);
	ScrCls();
	return 0;
}


int SetTransNo()
{
	while (1)
	{
		ScrCls();
		PubDispString(_T("Transaction No :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 22, glProcInfo.stTranLog.szTransNo, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
        break;
	}
	ScrCls();
	return 0;
}

int SetNilai()
{
	while (1)
	{
		ScrCls();
		PubDispString(_T("Nilai :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 12, glProcInfo.stTranLog.szNilai, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
        break;
	}
	ScrCls();
	return 0;
}

int SetNoHp()
{
	while (1)
	{
		ScrCls();//081807366329 -> lenght 12
		PubDispString(_T("NO HP :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 12, glProcInfo.stTranLog.szNoHP, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
        break;
	}
	ScrCls();
	return 0;
}

int SetNoPelanggan()
{
	while (1)
	{
		ScrCls();
		PubDispString(_T("NO/ID Pelanggan :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 12, glProcInfo.stTranLog.szAccount, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
        break;
	}
	ScrCls();
	return 0;
}

int SetPackageQty()
{
	uint n=0,count=0;
	uchar szBuff[3];
	memset(szBuff,0,3);
	sprintf((char *)szBuff,"1");
	while (1)
	{
		ScrCls();
		PubDispString(_T("Jumlah :"), 2|DISP_LINE_LEFT);
        if (PubGetString(ALPHA_IN|ECHO_IN, 0, 12, szBuff, USER_OPER_TIMEOUT)!=0)
        {
        	counterPackage=0;
            return ERR_USERCANCEL;
        }
        break;
	}
	count=counterPackage;
	for(n=0;n<=count;n++){
		if(strcmp(glPackageList[n].sPackageCode,glProcInfo.stTranLog.szRekening)==0){
			glPackageList[n].uiTotal+=atoi(szBuff);
			ScrCls();
			return 0;
		}
	}
	strcpy(glPackageList[counterPackage].sPackageCode,glProcInfo.stTranLog.szRekening);			
	glPackageList[counterPackage].uiTotal=atoi(szBuff);
	counterPackage++;
	ScrCls();
	return 0;
}


int AfterTranPinterpay(){
	if (1)
	    {
	        CommOnHook(FALSE);
	    }

		if( ChkIfPrnReceipt() )
		{
			DispTransName();
			/*if( glProcInfo.stTranLog.ucTranType!=VOID )
			{
				GetNewInvoiceNo();
			}*/
			PrintReceipt(PRN_NORMAL);
		}

		DispResult(0);
		return 0;
}



//PinterPay}

void SetAmexEntryMode(TRAN_LOG *pstLog)		//not use in telium
{
	glSendPack.szBit22[3] = '2';
	if (ChkIfPinReqdAllIssuer())
	{
		glSendPack.szBit22[3] = '1';	// pin capable
	}

#ifdef ENABLE_EMV	//not use in telium
	if( ChkAcqOption(ACQ_EMV_FEATURE) )
	{
		EMVGetParameter(&glEmvParam);
		if (glEmvParam.Capability[1] & 0x40)
		{
			glSendPack.szBit22[3] = '1';	// pin capable
		}
		if (glEmvParam.Capability[1] & 0x90)
		{
			glSendPack.szBit22[3] = '3';	// offline pin capable
		}
	}
#endif

#ifdef ENABLE_EMV	//not use in telium
	if( ChkAcqOption(ACQ_EMV_FEATURE) )
	{
		glSendPack.szBit22[1] = '5';
	}
#endif

	if( pstLog->uiEntryMode & MODE_SWIPE_INPUT )
	{
		if (glProcInfo.stTranLog.uiEntryMode & MODE_SECURITYCODE)
		{
			glSendPack.szBit22[2] = '6';
		}
		else
		{
			glSendPack.szBit22[2] = '2';
		}
	}
#ifdef ENABLE_EMV
	else if( pstLog->uiEntryMode & MODE_CHIP_INPUT )
	{
		glSendPack.szBit22[2] = '5';
	}
	else if( pstLog->uiEntryMode & MODE_FALLBACK_SWIPE )
	{
		glSendPack.szBit22[1] = '6';
		glSendPack.szBit22[2] = (glProcInfo.szSecurityCode[0]!=0) ? '6' : '2';
	}
	else if( pstLog->uiEntryMode & MODE_FALLBACK_MANUAL )
	{
		// ????;
	}
#endif
	else if( pstLog->uiEntryMode & MODE_MANUAL_INPUT )
	{
		glSendPack.szBit22[2] = (glProcInfo.szSecurityCode[0]!=0) ? '7' : '1';
	}
}

void SetStdEntryMode(TRAN_LOG *pstLog)
{
	//------------------------------------------------------------------------------
	// Entry mode digit 2 and digit 3
	if( pstLog->uiEntryMode & MODE_MANUAL_INPUT )
	{
		memcpy(&glSendPack.szBit22[1], "01", 2);
	}
	else if( pstLog->uiEntryMode & MODE_SWIPE_INPUT )
	{
		memcpy(&glSendPack.szBit22[1], "02", 2);
	}
	else if( (pstLog->uiEntryMode & MODE_CHIP_INPUT) || (pstLog->uiEntryMode & MODE_CONTACTLESS) )	
	{
		memcpy(&glSendPack.szBit22[1], "05", 2);
	}
	else if( pstLog->uiEntryMode & MODE_FALLBACK_SWIPE )
	{
		memcpy(&glSendPack.szBit22[1], "80", 2);
	}
	else if( pstLog->uiEntryMode & MODE_FALLBACK_MANUAL )
	{
	}

	//------------------------------------------------------------------------------
	// Entry mode digit 4
#ifdef ENABLE_EMV
	if( ChkAcqOption(ACQ_EMV_FEATURE) )
	{
		glSendPack.szBit22[3] = '1';    // default : support offline-PIN
	}
#endif
}

// set bit 25
void SetCondCode(void)
{
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH )
	{
		sprintf((char *)glProcInfo.stTranLog.szCondCode, "06");
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFFLINE_SEND )
	{
		if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_COMP )
		{
			sprintf((char *)glProcInfo.stTranLog.szCondCode, "06");
		}
	}

	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
}


// 交易初始化:检查交易是否允许,显示交易标题
// Init transaction, check allowance, display title. 
int TransInit(uchar ucTranType)
{
	ScrCls();
	glProcInfo.stTranLog.ucTranType = ucTranType;
	DispTransName();

#ifndef	__TELIUM_PARAM__
	if( !ChkIfTranAllow(ucTranType) )
	{
		return ERR_NO_DISP;
	}
#endif

	return 0;
}

int TransCapture(void)
{
	int	iRet;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CASH )
	{
		iRet = TransCashSub();
	}
	else if ( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE)
	{
		//iRet = TransSaleSub(); ditutup zaki 5 feb 2014
		iRet = TransSubSaleNew();
	}
	else if ( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT)
	{
		iRet = TransInstallmentSub();
	}
	else
	{
		if( ChkEdcOption(EDC_AUTH_PREAUTH) )
		{
			iRet = TransAuthSub(TRANS_TYPE_AUTH);
		}
		else
		{
			iRet = TransAuthSub(TRANS_TYPE_PREAUTH);
		}
	}

	return iRet;
}

int InstallmentMenu(void)
{
	int		iRet, iMenuNo;
	static	MenuItem stInstMenu[10] =
	{
		{TRUE, _T_NOOP("INSTAL SALE "),	NULL},
		{TRUE, _T_NOOP("INSTAL VOID "),	NULL},
		{TRUE, "", NULL},
	};


	iMenuNo = PubGetMenu((uchar *)_T("PLS SELECT:"), stInstMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransInstallment();
		break;

	case 1:
		iRet = TransVoid();
		break;

	default:
		return ERR_NO_DISP;
	}

	CommOnHook(FALSE);
	return iRet;
}

int TransCash(void)
{
	int		iRet;

	iRet = TransInit(TRANS_TYPE_CASH);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetCard(CARD_INSERTED|CARD_SWIPED|CARD_KEYIN);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransSaleSub();
}

int TransCashSub(void)
{
	int		iRet;

	if( !ChkIfTranAllow(glProcInfo.stTranLog.ucTranType) )
	{
		return ERR_NO_DISP;
	}
	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

#ifndef	__TELIUM_PARAM__
	iRet = GetDescriptor();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetAddlPrompt();
	if( iRet!=0 )
	{
		return iRet;
	}
#endif

	ScrCls();
	DispTransName();
	iRet = GetPIN(FALSE);
	if( iRet!=0 )
	{
		return iRet;
	}

	SetCommReqField();
	iRet = TranProcess();
	if( iRet!=ERR_NEED_FALLBACK )
	{
		return iRet;
	}

	// continue fallback process
	glProcInfo.bIsFallBack = TRUE;
	glProcInfo.stTranLog.uiEntryMode &= 0xF0;
	iRet = GetCard(FALLBACK_SWIPE|CARD_SWIPED);
	if( iRet!=0 )
	{
		return iRet;
	}

	SetCommReqField();
	return TranProcess();
}

#if 0
// 普通消费、分期消费
//int TransSale(uchar ucInstallment)
int TransSale(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransSaleSub();
}



// SALE
int TransSaleSub(void)
{
	int		iRet;

	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();

	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	if( ChkIfBelowMagFloor() && !glProcInfo.bExpiryError && !ChkIfIccTran(glProcInfo.stTranLog.uiEntryMode) )
	{
		sprintf((char *)glProcInfo.stTranLog.szAuthCode, "%02ld", glSysCtrl.ulSTAN % 100);
		glProcInfo.stTranLog.uiStatus |= TS_CHANGE_APPV|TS_FLOOR_LIMIT;
		return FinishOffLine();
	}

	SetCommReqField();	//set iso8583
	return TranProcess();
}

#endif


//int TransManual
int TransManual(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode = CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransSaleSub();
}


// SALE
int TransSaleManualSub(void)
{
	int		iRet;

	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();

	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	if( ChkIfBelowMagFloor() && !glProcInfo.bExpiryError && !ChkIfIccTran(glProcInfo.stTranLog.uiEntryMode) )
	{
		sprintf((char *)glProcInfo.stTranLog.szAuthCode, "%02ld", glSysCtrl.ulSTAN % 100);
		glProcInfo.stTranLog.uiStatus |= TS_CHANGE_APPV|TS_FLOOR_LIMIT;
		return FinishOffLine();
	}

	SetCommReqField();	//set iso8583
	return TranProcess();
}

// 
int TransInstallment(void)
{
	int		iRet;
	uchar	ucEntryMode;

	iRet = TransInit(TRANS_TYPE_INSTALMENT);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransInstallmentSub();
}

// 负责SALE和INSTALLMENT
int TransInstallmentSub(void)
{
	int		iRet;

	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();

	SetCommReqField();
	return TranProcess();
}


// 授权/预授权交易
int TransAuth(uchar ucTranType)
{
	int		iRet;

	PubASSERT(ucTranType==TRANS_TYPE_AUTH || ucTranType==TRANS_TYPE_PREAUTH);

	iRet = TransInit(ucTranType);
	if( iRet!=0 )
	{
		return iRet;
	}

	
	iRet = GetCard(CARD_INSERTED|CARD_SWIPED|CARD_KEYIN|CARD_TAP);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransAuthSub(ucTranType);
}

int TransAuthSub(uchar ucTranType)
{
	int		iRet;

	if( !ChkIfTranAllow(ucTranType) )
	{
		return ERR_NO_DISP;
	}
	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issPreauth!=TRUE)
	{
	/*//
		glCurTlmTmsIss.issPreauth=TRUE; //for test 4212
	//*/
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#else
	if( ChkIssuerOption(ISSUER_NO_PREAUTH) )
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#endif	

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

#ifndef	__TELIUM_PARAM__
		iRet = GetDescriptor();
		if( iRet!=0 )
		{
			return iRet;
		}

		iRet = GetAddlPrompt();
		if( iRet!=0 )
		{
			return iRet;
		}
#endif

	ScrCls();
	DispTransName();


	if(PrivateLabelCardDetect)
		{
			iRet = GetPIN_ManualPrvtLbl(FALSE);	//get pin
	
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		else {
			iRet = GetPIN_Manual(FALSE);	//get pin
	
			if( iRet!=0 )
			{
			return iRet;
			}
		}
		/*
	iRet = GetPIN_Manual(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}
	*/

	SetCommReqField();
	iRet = TranProcess();
	if( iRet!=ERR_NEED_FALLBACK )
	{
		return iRet;
	}

	// continue fallback process
	glProcInfo.bIsFallBack = TRUE;
	glProcInfo.stTranLog.uiEntryMode &= 0xF0;
	iRet = GetCard(FALLBACK_SWIPE|CARD_SWIPED);
	if( iRet!=0 )
	{
		return iRet;
	}

	SetCommReqField();
	return TranProcess();
}

int FinishOffLine(void)
{
	uchar	ucTranAct;

#if defined(_P60_S1_) || defined(_S_SERIES_)
	SetOffBase(OffBaseDisplay);
#endif

	DispTransName();
	DispProcess();

	if( !(glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_SALE_COMP) )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "00");
	}
	glProcInfo.stTranLog.ulInvoiceNo = glSysCtrl.ulInvoiceNo;
	ucTranAct = glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct;

	if (ucTranAct & ACT_INC_TRACE)
	{
		GetNewTraceNo();
	}

	if( ucTranAct & WRT_RECORD )
	{
		glProcInfo.stTranLog.uiStatus |= TS_NOSEND;
		SaveTranLog(&glProcInfo.stTranLog);
	}

	if( ucTranAct & PRN_RECEIPT )	// print slip
	{
		CommOnHook(FALSE);
		GetNewInvoiceNo();
		PrintReceipt(PRN_NORMAL);
	}

	DispResult(0);
#ifdef	__TELIUM_PARAM__
	PubWaitKey(glTlmSysParam.stEdcInfo.ucAcceptTimeout);
#else
	PubWaitKey(glSysParam.stEdcInfo.ucAcceptTimeout);
#endif
	return 0;
}

int TranReversal(void)
{
	int	iRet;
	SYS_PROC_INFO	stProcInfoBak;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA  ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_ECHO_TEST  ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM || 
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFF_SALE)
	{
		return 0;
	}

#ifdef	__TELIUM_PARAM__
	if( !glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal )
	{
		return 0;
	}
#else
	if( !glSysCtrl.stRevInfo[glCurAcq.ucIndex].bNeedReversal )
	{
		return 0;
	}
#endif

	// backup current process information
	memcpy(&glProcInfo.stSendPack, &glSendPack, sizeof(STISO8583));
	memcpy(&stProcInfoBak, &glProcInfo, sizeof(SYS_PROC_INFO));
	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_REVERSAL;

	ScrCls();
	DispTransName();

#ifdef	__TELIUM_PARAM__
	memcpy(&glSendPack, &glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].stRevPack, sizeof(STISO8583));
#else
	memcpy(&glSendPack, &glSysCtrl.stRevInfo[glCurAcq.ucIndex].stRevPack, sizeof(STISO8583));
#endif

	sprintf((char *)glSendPack.szMsgCode, "0400");
	
	memset(glSendPack.szBit52, 0, sizeof(glSendPack.szBit52));	// erase PIN block

	while( 1 )
	{
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			break;
		}
		if( memcmp(glRecvPack.szBit39, "00", 2)==0 )
		{
			break;
		}
		
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		DispResult(ERR_HOST_REJ);
// 		iRet = ERR_NO_DISP;
		iRet = ERR_TRAN_FAIL;
		break;
	}
	if( iRet==0 )
	{	// clear reversal flag
		SaveRevInfo(FALSE);
	}

	// restore process information
	memcpy(&glProcInfo, &stProcInfoBak, sizeof(SYS_PROC_INFO));
	memcpy(&glSendPack, &glProcInfo.stSendPack, sizeof(STISO8583));

	ScrCls();
	DispTransName();

	return iRet;
}

int GetOfflineTrans(uchar ucTypeFlag)
{
	int		iRet;
	ushort	uiCnt;

	for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
	{
#ifdef	__TELIUM_PARAM__
		if( glSysCtrl.sAcqKeyList[uiCnt]!=glCurTlmTmsAcq.acqID )
		{
			continue;
			}
#else
		if( glSysCtrl.sAcqKeyList[uiCnt]!=glCurAcq.ucKey )
		{
			continue;
			}
#endif

		memset(&glProcInfo.stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTranLog(&glProcInfo.stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return FALSE;
			}
		if ( (ucTypeFlag & OFFSEND_TC) && (glProcInfo.stTranLog.uiStatus & TS_NEED_TC) )
		{
			glProcInfo.uiRecNo = uiCnt;
			return TRUE;
			}
		else if((ucTypeFlag & OFFSEND_VOID) && (glProcInfo.stTranLog.uiStatus & TS_VOID) && (glProcInfo.stTranLog.uiStatus & TS_NOSEND))
		{
			glProcInfo.uiRecNo = uiCnt;
			return TRUE;
			}
		else if ((ucTypeFlag & OFFSEND_TRAN) && (glProcInfo.stTranLog.uiStatus & TS_NOSEND)) //4212 
		{
			if((glProcInfo.stTranLog.uiStatus & TS_VOID ))
				continue;
			else
				{
				glProcInfo.uiRecNo = uiCnt;
				return TRUE;
				}
			}
		}
		return FALSE;
	}

int OfflineSend(uchar ucTypeFlag)
{
#ifdef ENABLE_EMV
	int iLength;
#endif
	int	iRet;
	SYS_PROC_INFO	stProcInfoBak;

	if( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_SETTLEMENT )
	{	// the time to load 400 txn log is about 1-2 seconds
		if(!glCurTlmTmsAcq.acqOpt2)	//TRICKLE_FEED 0x00 = disable, 0x10=enable
		{
			return 0;
		}
	}

	memcpy(&glProcInfo.stSendPack, &glSendPack, sizeof(STISO8583));
	memcpy(&stProcInfoBak, &glProcInfo,  sizeof(SYS_PROC_INFO));
	while( 1 )
	{
		InitTransInfo();
		if( !GetOfflineTrans(ucTypeFlag) )
		{
			iRet = 0;
			break;
		}

		if (glProcInfo.stTranLog.uiStatus & TS_NEED_TC)
		{
			glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
			glProcInfo.stTranLog.ucTranType    = TRANS_TYPE_TC_SEND;

			DispTransName();
			SetCommReqField();

			if (0)
			{
				sprintf((char *)glSendPack.szMsgCode, "0220");
			}

			// bit39
			//sprintf((char *)glSendPack.szBit39, "%.2s", glProcInfo.stTranLog.szRspCode);

#ifdef ENABLE_EMV
			// bit55
			//SetDE55(DE55_TC, glSendPack.sICCData+2, &iLength);
            SetTCDE55(&glProcInfo.stTranLog, glSendPack.szBit55+2, &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
#endif

			iRet = SendRecvPacket();
			if( iRet!=0 )
			{
				break;
			}

			// update txn status
			glProcInfo.stTranLog.uiStatus &= ~TS_NEED_TC;
			glProcInfo.stTranLog.ucTranType = glProcInfo.stTranLog.ucOrgTranType;
			UpdateTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);

			if( stProcInfoBak.stTranLog.ucTranType!=TRANS_TYPE_SETTLEMENT )
			{
				break;
			}

			continue;
		} 

		glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
		glProcInfo.stTranLog.ucTranType    = TRANS_TYPE_OFFLINE_SEND;
		DispTransName();

		SetCommReqField();

		// bit 4
		if( glProcInfo.stTranLog.uiStatus & TS_VOID )
		{
			glSendPack.szBit4[0] = 0;
		}

		// bit 37, 39
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			sprintf((char *)glSendPack.szBit39, "%.2s", glProcInfo.stTranLog.szRspCode);
			if( glProcInfo.stTranLog.szRRN[0]!=0 )	// seen by host
			{
				glSendPack.szBit39[0] = 0;
			}
		}

		if( ChkAcqOption(ACQ_DBS_FEATURE) )	//not use in telium
		{	// 香港星展银行,sale_complete上送时, VISA/MASTER需要上送 BIT37 BIT39
			if( glProcInfo.stTranLog.ucOrgTranType==TRANS_TYPE_SALE_COMP &&
				glProcInfo.stTranLog.szRRN[0]==0 )
			{
				sprintf((char *)glSendPack.szBit37, "%.12s", &glProcInfo.stTranLog.szRRN[1]);
			}
		}

		// bit 55
		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			// only send ICC sale below floor
			
		//if( glProcInfo.stTranLog.szRRN[0]==0 )
		//	{
				PubLong2Char((ulong)glProcInfo.stTranLog.uiIccDataLen, 2, glSendPack.szBit55);
				memcpy(&glSendPack.szBit55[2], glProcInfo.stTranLog.sIccData, glProcInfo.stTranLog.uiIccDataLen);
		//	}
		}

		// bit 60
		if(glProcInfo.stTranLog.uiStatus & (TS_ADJ|TS_VOID)) 
		{
			if( glProcInfo.stTranLog.uiStatus & TS_VOID )
			{
//				memcpy((char *)glSendPack.szBit60, "\x00\x12", 2);
//				sprintf((char *)&glSendPack.szBit60[2], "%.12s", glProcInfo.stTranLog.szAmount);

				sprintf((char *)glSendPack.szBit60, "%.12s", glProcInfo.stTranLog.szAmount);

			}
			else
			{
//				memcpy((char *)glSendPack.szBit60, "\x00\x12", 2);
//				sprintf((char *)&glSendPack.szBit60[2], "%.12s", glProcInfo.stTranLog.szOrgAmount);

				sprintf((char *)glSendPack.szBit60, "%.12s", glProcInfo.stTranLog.szOrgAmount);

			}
		}
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			break;
		}

		// update txn status
		glProcInfo.stTranLog.uiStatus &= ~TS_NOSEND;
		glProcInfo.stTranLog.uiStatus |= TS_OFFLINE_SEND;
		glProcInfo.stTranLog.ucTranType = glProcInfo.stTranLog.ucOrgTranType;
		UpdateTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);

		if( stProcInfoBak.stTranLog.ucTranType!=TRANS_TYPE_SETTLEMENT )
		{	// is trickle feed, only need send one txn
			break;
		}
	}
	memcpy(&glProcInfo, &stProcInfoBak, sizeof(SYS_PROC_INFO));
	memcpy(&glSendPack, &glProcInfo.stSendPack, sizeof(STISO8583));

	return iRet;
}

// transaction complete for voice referral
int TranSaleComplete(void)
{
	int		iRet;
	uchar	ucKey;

	if( !ChkIssuerOption(ISSUER_EN_VOICE_REFERRAL) )
	{
		return ERR_TRAN_FAIL;
	}

	if( (glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_PREAUTH) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_AUTH) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_SALE) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_CASH) )
	{
		return ERR_TRAN_FAIL;
	}

	if( glProcInfo.stTranLog.ucTranType>=TRANS_TYPE_SALE )
	{
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE_COMP;
	}

	CommOnHook(FALSE);
#ifdef	__TELIUM_PARAM__
	PubDispString(_T("PLS CALL BANK"), 4|DISP_LINE_LEFT);

	ucKey = PubWaitKey(USER_OPER_TIMEOUT);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		ScrClrLine(4, 7);
		PubDispString(_T("TRANS CANCELED"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

#else
	ScrPrint(0, 0, CFONT|REVER, "CALL:%-11.11s", glCurIssuer.szRefTelNo);
	PubDispString(_T("PLS CALL BANK"), 4|DISP_LINE_LEFT);

	if( ChkEdcOption(EDC_REFERRAL_DIAL) )
	{
		iRet = ReferralDial(glCurIssuer.szRefTelNo);
		if( iRet!=0 )
		{
			return iRet;
		}
	}
	else
	{
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			ScrClrLine(4, 7);
			PubDispString(_T("TRANS CANCELED"), 4|DISP_LINE_LEFT);
			PubWaitKey(3);
			return ERR_NO_DISP;
		}
	}
#endif

	iRet = GetPreAuthCode();
	if( iRet!=0 )
	{
		ScrClrLine(4, 7);
		PubDispString(_T("TRANS CANCELED"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

	// 香港星展银行,sale_complete上送时, VISA/MASTER需要上送 BIT37 BIT39
	// For DBS in HongKong.
	memmove(&glProcInfo.stTranLog.szRRN[1], glProcInfo.stTranLog.szRRN, 12);
	glProcInfo.stTranLog.szRRN[0] = 0;
//	sprintf((char *)glProcInfo.stTranLog.szCondCode, "06");


	return FinishOffLine();
}

// 退货
int TransRefund(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_REFUND);
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_REFUND_PWD) )
	{
		if( PasswordRefund()!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	ScrCls();
	DispTransName();

	if(glTlmSysParam.Con.appEMRefund& EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMRefund & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMRefund & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issRefund != TRUE)
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#else
	if( ChkIssuerOption(ISSUER_NO_REFUND) )
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#endif

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}
#ifndef	__TELIUM_PARAM__
	iRet = GetDescriptor();
	if( iRet!=0 )
	{
		return iRet;
	}


	iRet = GetAddlPrompt();
	if( iRet!=0 )
	{
		return iRet;
	}
	
#endif
	//GET PIN
		ScrCls();
		DispTransName();

		if(PrivateLabelCardDetect)
		{
			iRet = GetPIN_ManualPrvtLbl(FALSE);	//get pin
	
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		else {
			iRet = GetPIN_Manual(FALSE);	//get pin
	
			if( iRet!=0 )
			{
			return iRet;
			}
		}
		/*
	iRet = GetPIN_Manual(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}
	*/


#ifdef	__TELIUM_PARAM__
	//always online
	SetCommReqField();

	return TranProcess();
#else
	if( ChkAcqOption(ACQ_ONLINE_REFUND) )
	{
		SetCommReqField();
		return TranProcess();
	}
	else
	{
		return FinishOffLine();
	}
#endif	
}

// 离线
int TransOffSale(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	
#ifdef ENABLE_EMV
//	int	iLength;
#endif

	iRet = TransInit(TRANS_TYPE_OFF_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	//*// 4212
	if(glTlmSysParam.Con.appEMOffline& EM_CHIP_INITIAL)
			ucEntryMode |= CARD_INSERTED;
		if(glTlmSysParam.Con.appEMOffline & EM_MAGNETIC_STRIPE_INITIAL)
			ucEntryMode |= CARD_SWIPED;
		if(glTlmSysParam.Con.appEMOffline & EM_MANUAL_INITIAL)
			ucEntryMode |= CARD_KEYIN;
		
		if(!ucEntryMode)
		{
			return ERR_NO_DISP;
		}
		
	iRet = GetCard(ucEntryMode);
	//*/
	//iRet = GetCard(SKIP_CHECK_ICC|CARD_SWIPED|CARD_KEYIN);	
	if( iRet!=0 )
	{
		return iRet;
	}
	//CHECK BATCH
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issOffline != TRUE)
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#else
	if( !ChkIssuerOption(ISSUER_EN_OFFLINE) )
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#endif

	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}
	
	ScrCls();
	DispTransName();
	
#ifndef	__TELIUM_PARAM__
	iRet = GetDescriptor();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetAddlPrompt();
	if( iRet!=0 )
	{
		return iRet;
	}
#endif
	iRet = GetPreAuthCode();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	/*// 4212
	iRet = GetPIN(FALSE);
	if( iRet!=0 )
	{
		return iRet;
	}
	/*/
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//*/
	//SET ISO 8583

	if(PrivateLabelCardDetect)
			{
				SetPackSalePrvtLbl();
			}
			else {
			SetPackSale();
			}
			
	////SetPackSale();
	//hex_dump_char("IccData 01", glProcInfo.stTranLog.sIccData, glProcInfo.stTranLog.uiIccDataLen);
	
#ifdef ENABLE_EMV
	//glProcInfo.ucOnlineStatus=ST_OFFLINE;
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
		FinishEmvTran();
	SaveRevInfo(FALSE);	// update reversal information
	//hex_dump_char("IccData 02", glProcInfo.stTranLog.sIccData, glProcInfo.stTranLog.uiIccDataLen);
#endif
	//*/

	
	return FinishOffLine();
}

int TransVoid(void)
{
	int			iRet;
	uchar		ucTranAct, bOnlineFlag, szTempAmt[12+1];

	iRet = TransInit(TRANS_TYPE_VOID);
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_VOID_PWD) )
	{
		if( PasswordVoid()!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	while( 1 )
	{
		iRet = GetRecord(TS_OK|TS_NOSEND|TS_ADJ, &glProcInfo.stTranLog);
		if( iRet!=0 )
		{
			return iRet;
		}

		ucTranAct = glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct;
		if( ucTranAct & VOID_ALLOW )
		{
			glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
			glProcInfo.stTranLog.ucTranType    = TRANS_TYPE_VOID;
			break;
		}

		PubDispString(_T("NOT ALLOW VOID"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	ScrCls();
	DispTransName();

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issVoid != TRUE)
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#else	
	if( ChkIssuerOption(ISSUER_NO_VOID) )
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}
#endif

	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	//always online
	ScrCls();
	DispTransName();
	DispWait();
	PreDial();
#else
	if( ChkAcqOption(ACQ_ONLINE_VOID) )
	{
		ScrCls();
		DispTransName();
		DispWait();
		PreDial();
	}
#endif
	PubDispString(_T("TOTAL"), 4|DISP_LINE_LEFT);
	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTempAmt);
	DispAmount(4, szTempAmt);	// show total amount
	PubDispString(_T("VOID ? Y/N"), DISP_LINE_LEFT|6);
	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return ERR_USERCANCEL;
	}

	ScrCls();
	DispTransName();

 	bOnlineFlag = TRUE;
  	if( glProcInfo.stTranLog.uiStatus & TS_NOSEND )
 	{	// offsale/sale below/sale comp.../adjust
 		if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
 		{	// 该调整还未上送,作废调整后的金额
			PubAscSub(glProcInfo.stTranLog.szOrgAmount, glProcInfo.stTranLog.szAmount, 12, szTempAmt);
			//PubAddHeadChars(szTempAmt, 12, '0');		no need: already 12 digits
			sprintf((char *)glProcInfo.stTranLog.szTipAmount, "%.12s", szTempAmt);
 		}
		if( glProcInfo.stTranLog.szRRN[0]==0 )
		{
			bOnlineFlag = FALSE;
		}
 	}

#ifdef	__TELIUM_PARAM__
	//always online
	//bOnlineFlag = TRUE;
#else
 	if( !ChkAcqOption(ACQ_ONLINE_VOID) )
	{
		bOnlineFlag = FALSE;
	}
#endif

	if( bOnlineFlag )
	{
		SetCommReqField();
		return TranProcess();
	}
	else	// offline void
	{
		glProcInfo.stTranLog.uiStatus |= (TS_VOID|TS_NOSEND);
		//if( glProcInfo.stTranLog.szRRN[0]==0 ) //4212
		//glProcInfo.stTranLog.uiStatus &= ~(TS_NOSEND); // not seen by host(orginal txn is offsale/sale below/salecomp/ ...)
		GetDateTime(glProcInfo.stTranLog.szDateTime);
		UpdateTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);
		PrintReceipt(PRN_NORMAL);
		DispResult(0);

		return 0;
	}
}

int TransOther(void)	//main menu, irf, 20121129
{
	int		iRet, iMenuNo;
	uchar	ucTranType;
	static	MenuItem stTranMenu[10] =
	{
		{TRUE, _T_NOOP("VOID            "),		NULL},	// 0	
		{TRUE, _T_NOOP("OFFLINE         "),		NULL},	// 1
		{TRUE, _T_NOOP("REFUND          "),		NULL},	// 2
		{TRUE, _T_NOOP("ADJUST          "),		NULL},	// 3
		{TRUE, _T_NOOP("SETTLE          "),		NULL},	// 4
		{TRUE, "", NULL},	// reserved for auth/preauth	// 5
		{TRUE, _T_NOOP("PIN VERIFY      "),		NULL},	// 6, added by irf, 20121129 
		{TRUE, _T_NOOP("INSTALLMENT     "),	NULL},	// 7
		{TRUE, "", NULL},
	};
	static	uchar	szPreAuthTitle[] = _T_NOOP("PREAUTH         ");
	static	uchar	szAuthTitle[]    = _T_NOOP("AUTH            ");
	static	uchar	szPrompt[]       = _T_NOOP("PLS SELECT:     ");

	if( ChkEdcOption(EDC_AUTH_PREAUTH) )
	{
		ucTranType = TRANS_TYPE_AUTH;
		sprintf((char *)stTranMenu[5].szMenuName, "%s", szAuthTitle);
	}
	else
	{
		ucTranType = TRANS_TYPE_PREAUTH;
		sprintf((char *)stTranMenu[5].szMenuName, "%s", szPreAuthTitle);
	}

#ifdef	__TELIUM_PARAM__
	if ((glTlmSysParam.ucPlanNum==0) || !ChkEdcOption(EDC_ENABLE_INSTALMENT))
	{
//		stTranMenu[6].szMenuName[0] = 0;	// delete installment item.		//irf, 20121129
		stTranMenu[7].szMenuName[0] = 0;	// delete installment item.			//irf, 20121129
	}
#else
	if ((glSysParam.ucPlanNum==0) || !ChkEdcOption(EDC_ENABLE_INSTALMENT))
	{
//		stTranMenu[6].szMenuName[0] = 0;	// delete installment item.		//irf, 20121129
		stTranMenu[7].szMenuName[0] = 0;	// delete installment item.			//irf, 20121129
	}
#endif

//	iMenuNo = PubGetMenu((uchar *)_T(szPrompt), stTranMenu, MENU_AUTOSNO|MENU_PROMPT, USER_OPER_TIMEOUT);

	iMenuNo = PubGetMenuNew((uchar *)_T(szPrompt), stTranMenu, MENU_NOSPEC, USER_OPER_TIMEOUT);
	switch( iMenuNo )
	{
	case 0:
		iRet = TransVoid();
		break;

	case 1:
		iRet = TransOffSale();
		break;

	case 2:
		iRet = TransRefund();
		break;

	case 3:
		TransAdjust();
		iRet = 0;
		break;

	case 4:
		iRet = TransSettle();
		break;

	case 5:
		iRet = TransAuth(ucTranType);
		break;
		
	case 6:
		iRet = TransPinVerify();//irf, 20121129
		break;	
		
	case 7:
		iRet = InstallmentMenu();	
	default:
		return ERR_NO_DISP;
	}

	CommOnHook(FALSE);

	return iRet;
}

// 调整交易输入
int AdjustInput(void)
{
	int		iRet;

	while( 1 )
	{
		InitTransInfo();
		DispAdjustTitle();
		iRet = GetRecord(TS_NOSEND|TS_ADJ, &glProcInfo.stTranLog);
		if( iRet!=0 )
		{
			return iRet;
		}

		ScrCls();
		DispAdjustTitle();
		if( ChkIfZeroAmt(glProcInfo.stTranLog.szAmount) &&
			ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
		{
			PubDispString(_T("DO NOT ADJUST"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(3);
			continue;
		}

		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

#ifdef	__TELIUM_PARAM__
		if(glCurTlmTmsIss.issAdjust != TRUE)
		{
			DispBlockFunc();
			continue;
		}
#else
		if( !ChkIssuerOption(ISSUER_EN_ADJUST) )
		{
			DispBlockFunc();
			continue;
		}
#endif

		if( ChkSettle() )
		{
			break;
		}
		return ERR_NO_DISP;
	}

	return 0;
}

// 调整
void TransAdjust( void )
{
	int		iRet;
	uchar	ucResult, szCurrName[4+1], szTotalAmt[12+1], szTemp[12+1];
	uchar	szCurTime[14+1];

	PubShowTitle(TRUE, (uchar *)_T("ADJUST          "));
	if( !ChkEdcOption(EDC_NOT_ADJUST_PWD) )
	{
		if( PasswordAdjust()!=0 )
		{
			return;
		}
	}

	if( GetTranLogNum(ACQ_ALL)==0 )
	{
		DispAdjustTitle();
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	while( 1 )
	{
		iRet = AdjustInput();
		if( iRet!=0 )
		{
			return;
		}

		while( 2 )
		{
			ScrCls();
			DispAdjustTitle();
			DispAmount(2, glProcInfo.stTranLog.szAmount);
			PubDispString(_T("NEW TOTAL"), 4|DISP_LINE_LEFT);

#ifdef	__TELIUM_PARAM__
			sprintf((char *)szCurrName, "%.3s", glTlmSysParam.Con.appCurrSign);
#else
			sprintf((char *)szCurrName, "%.3s", glSysParam.stEdcInfo.stLocalCurrency.szName);
#endif
			memset(szTotalAmt, 0, sizeof(szTotalAmt));
			ucResult = PubGetAmount(szCurrName, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
									1, 8, szTotalAmt, USER_OPER_TIMEOUT, 0);
			if( ucResult!=0 )
			{
				return;
			}

			// Use transaction currency to do conversion
			//PubAddHeadChars(szTotalAmt, 12, '0');
			AmtConvToBit4Format(szTotalAmt, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
			if( !ValidAdjustAmount(glProcInfo.stTranLog.szAmount, szTotalAmt) )
			{
				continue;
			}
			if (!ValidBigAmount(glProcInfo.stTranLog.szAmount))
			{
				continue;
			}
			if( !ConfirmAmount(NULL, szTotalAmt) )
			{
				break;
			}

			if( !(glProcInfo.stTranLog.uiStatus & TS_NOSEND) )
			{
				PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTemp);
				//PubAddHeadChars(szTemp, 12, '0');  no need: already 12 digits
				sprintf((char *)glProcInfo.stTranLog.szOrgAmount, "%.12s", szTemp);
				glProcInfo.stTranLog.uiStatus |= (TS_ADJ|TS_NOSEND);
			}

			PubAscSub(szTotalAmt, glProcInfo.stTranLog.szAmount, 12, szTemp);
			//PubAddHeadChars(szTemp, 12, '0');		no need: already 12 digits
			sprintf((char *)glProcInfo.stTranLog.szTipAmount, "%.12s", szTemp);
			GetDateTime(szCurTime);
			sprintf((char *)glProcInfo.stTranLog.szDateTime, "%.14s", szCurTime);

			iRet = UpdateTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);
			if( iRet!=0 )
			{
				return;
			}

//			PrintReceipt(PRN_NORMAL);	// no receipt for adjust
			glSysCtrl.uiLastRecNo = glProcInfo.uiRecNo;		// use reprint to print adjust
			SaveSysCtrlBase();

			DispAdjustTitle();
			DispAccepted();
			PubBeepOk();
			PubWaitKey(3);
			break;	// continue to adjust
		}	// end of while( 2
	}	// end of while( 1
}

// 线路测试 Echo test
void TransEchoTest(void)
{
	int		iRet;
	uchar	ucAcqIndex, szTitle[16+1];

	TransInit(TRANS_TYPE_ECHO_TEST);

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}

	ScrCls();
	DispTransName();

	SetCommReqField();
	iRet = SendRecvPacket();
	DispResult(iRet);
}

int TransInqPpob(uchar * ppobcode, uchar * product_code){
	int		iRet;
	uchar	ucEntryMode, ucAcqIndex, szTitle[16+1];

	iRet = TransInit(INQPPOB);
	if( iRet!=0 )
		{
			return iRet;
		}
	memset(glProcInfo.stTranLog.szKodePpob,0,sizeof(glProcInfo.stTranLog.szKodePpob));
	memset(glProcInfo.stTranLog.szKodeProduk,0,sizeof(glProcInfo.stTranLog.szKodeProduk));
	strcpy(glProcInfo.stTranLog.szKodePpob,ppobcode);
	strcpy(glProcInfo.stTranLog.szKodeProduk,product_code);
	iRet = SetNoPelanggan();
	if( iRet!=0 )
	{
		return iRet;
	}
	iRet = GetCard(CARD_SWIPED|CARD_KEYIN);
		if( iRet!=0 )
		{
			return iRet;
		}
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
		//ScrCls();
		//DispTransName();
		SetCommReqField();
		iRet = SendRecvPacket();
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){;
			if(memcmp(glRecvPack.szBit39, "01", 2)==0){
				DispErrMessage();
				return ERR_NO_DISP;
			}else{
				return ERR_HOST_REJ;
			}
		}
		if( iRet!=0 )
		{
			return iRet;
		}
		iRet=DispInqueryPpob();
		if( iRet!=0 )
		{
			return iRet;
		}
		
		iRet = TransPpob();
		if( iRet!=0 )
		{
			return iRet;
		}
		return iRet;

}

int TransInqPulsa(){
	int		iRet;
	uchar	ucEntryMode, ucAcqIndex, szTitle[16+1];

	iRet = TransInit(INQTRANS);
	if( iRet!=0 )
	{
		return iRet;
	}
	iRet = SetTransNo();
	if( iRet!=0 )
	{
		return iRet;
	}
	iRet = GetCard(CARD_SWIPED|CARD_KEYIN);
	if( iRet!=0 )
	{
		return iRet;
	}
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
		SetCommReqField();
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){;
			if(memcmp(glRecvPack.szBit39, "01", 2)==0){
				DispErrMessage();
				return ERR_NO_DISP;
			}else{
				return ERR_HOST_REJ;
			}
		}
		iRet=AfterTranPinterpay();
		if( iRet!=0 )
		{
			return iRet;
		}
		return iRet;

}


int TransInqPinterpay(){
	int		iRet;
	uchar	ucEntryMode;
	uchar	ucAcqIndex;
	uchar	szTitle[16+1];

	iRet = TransInit(INQPINTERPAY);
	if( iRet!=0 )
		{
			return iRet;
		}
	iRet = SetAcc();
	if( iRet!=0 )
	{
		return iRet;
	}
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}

/*
	ucRet = FindAcqIdxByName("ATMB_AJ", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);
*/

	
		ScrCls();
		DispTransName();
		SetCommReqField();
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){;
			return ERR_NO_DISP;
		}
		iRet = DispInqueryPPay();
		if( iRet!=0 )
		{
			counterPackage=0;
			return iRet;
		}
		SetPackageQty();
		iRet = CheckPackageList();
		return iRet;

}

int TransPinterpay(){
	int		iRet;
	int 	n,price;
	uchar	ucEntryMode, szTitle,ucAcqIndex;

	iRet = TransInit(PINTERPAY);
	glProcInfo.stTranLog.ppaj=1;
	if( iRet!=0 )
		{
			return iRet;
		}
	/*sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}*/
		ScrCls();
		DispTransName();
		/*price=0;
		for(n=0;n<counterPackage;n++){
			price+=glPackageList[n].uiTotal*glPackageList[n].uiTotal;
		}
		sprintf(glProcInfo.stTranLog.szAmount,"%d",price);*/
		iRet = SetAmt();
		if( iRet!=0 )
		{
			glProcInfo.stTranLog.ppaj=0;
			return iRet;
		}
		iRet = GetCard(CARD_SWIPED|CARD_KEYIN);
		if( iRet!=0 )
		{
			glProcInfo.stTranLog.ppaj=0;
			return iRet;
		}
		/*iRet = SetPIN();
		if( iRet!=0 )
		{
			return iRet;
		}else{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		}*/
		ScrCls();
		DispTransName();
		SetCommReqField();
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			DispErr(atoi(glRecvPack.szBit39));
			glProcInfo.stTranLog.ppaj=0;
			return 0;
		}
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){
			glProcInfo.stTranLog.ppaj=0;
			return ERR_HOST_REJ;
		}
		counterPackage=0;
		iRet = AfterTranPinterpay();
		glProcInfo.stTranLog.ppaj=0;
		return iRet;
}

int DispErr(int rc){
	int ucKey;
	uchar message[50];
	uchar szBuff[10];
	uint SUCCESS = 0;
    uint PLEASE_CALL_ISSUER = 1;
    uint REFER_TO_ISSUER_SP = 2;
    uint INVLD_MERCHANT = 3;
    uint PLS_PICK_UP_CARD = 4;
    uint DO_NOT_HONOUR = 5;
    uint ERROR = 6;
    uint Pickup_Card = 7;
    uint VERIFY_ID_AND_SIGN = 8;
    uint REQUEST_IN_PROGRESS = 9;
    uint Appvd_for_Partial_Amt = 10;
    uint Approved_VIP = 11;
    uint INVLD_TRANSACTION = 12;
    uint INVLD_AMT = 13;
    uint INVLD_CARD_NUM = 14;
    uint No_such_Issuer = 15;
    uint Approved = 16;
    uint Customer_Cancellation = 17;
    uint Customer_Dispute = 18;
    uint REENTER_TRANSACTION = 19;
    uint INVALID_RESPONSE = 20;
    uint NO_TRANSACTIONS = 21;
    uint Suspected_Malfunction = 22;
    uint Unaccepted_Trans_Fee = 23;
    uint File_Upd_not_Suported = 24;
    uint ERRFILE_NOT_FOUND = 25;
    uint Dup_Rec = 26;
    uint Field_Edit_Error = 27;
    uint File_Locked_out = 28;
    uint File_Update_Error = 29;
    uint FORMAT_ERROR = 30;
    uint BANK_NOT_SUPPORTED = 31;
    uint Completed_Partially = 32;
    uint EXPIRED_CARD = 33;
    uint Suspected_Fraud = 34;
    uint Contact_Acquirer = 35;
    uint Restricted_Card = 36;
    uint Call_Acq_Security = 37;
    uint PIN_tries_Exceeded = 38;
    uint No_Credit_Account = 39;
    uint FUNC_NOT_SUPPORTED = 40;
    uint LOST_CARD = 41;
    uint No_Universal_Account = 42;
    uint STOLEN_CARD = 43;
    uint No_Investment_Account = 44;
    uint ISO_error_45 = 45;
    uint ISO_error_46 = 46;
    uint ISO_error_47 = 47;
    uint ISO_error_48 = 48;
    uint ISO_error_49 = 49;
    uint ISO_error_50 = 50;
    uint INSUFFICIENT_FUND = 51;
    uint NO_CHEQUE_ACC = 52;
    uint No_Savings_Account = 53;
    uint EXPIRED_CARD2 = 54;
    uint Incorrect_PIN = 55;
    uint No_Card_Record = 56;
    uint Txn_not_Permtd_card = 57;
    uint TRANS_NOT_PERMITTED = 58;
    uint Suspected_Fraud2 = 59;
    uint Contact_Acquirer2 = 60;
    uint EXCEED_LIMIT = 61;
    uint Restricted_Card2 = 62;
    uint Security_Violation = 63;
    uint Org_Amount_Incorrect = 64;
    uint Freq_Limit_Exceed = 65;
    uint call_acq_security = 66;
    uint Hard_Capture = 67;
    uint Resp_Recvd_too_Late = 68;
    uint Unavalible = 69;
    uint ISO_error_70 = 70;
    uint ISO_error_71 = 71;
    uint ISO_error_72 = 72;
    uint ISO_error_73 = 73;
    uint ISO_error_74 = 74;
    uint PIN_TRIES_EXCEED = 75;
    uint KEY_SYNC_ERROR = 76;
    uint Resvd_for_Nat_use1 = 77;
    uint OLD_ROC_NOT_FOUND = 78;
    uint BATCH_ALREADY_OPEN = 79;
    uint Resvd_for_Nat_use2 = 80;
    uint Private_error_81 = 81;
    uint Private_error_82 = 82;
    uint Private_error_83 = 83;
    uint Private_error_84 = 84;
    uint BATCH_NOT_FOUND = 85;
    uint Private_error_86 = 86;
    uint Private_error_87 = 87;
    uint HAVE_CM_CALL_AMEX = 88;
    uint Resvd_for_Nat_use3 = 89;
    uint Cutoff_in_Process = 90;
    uint NETWORK_ERROR = 91;
    uint Trans_cant_be_Routed = 92;
    uint Txn_cant_be_Completed = 93;
    uint DUPL_TRANSMISSION = 94;
    uint RECONCILE_ERROR = 95;
    uint INVALID_MESSAGE = 96;
    uint Resvd_for_Nat_use4 = 97;
    uint Resvd_for_Nat_use5 = 98;
    uint Resvd_for_Nat_use6 = 99;


	ScrCls();
	DispTransName();
	sprintf((char *)szBuff, "ERROR");
	PubDispString(szBuff, 2|DISP_LINE_CENTER);

	if(SUCCESS==rc){
				sprintf(message,"SUCCESS");
			}
			else if( PLEASE_CALL_ISSUER ==rc){
				sprintf(message, "PLEASE_CALL_ISSUER");
				}
			else if( REFER_TO_ISSUER_SP ==rc){
				sprintf(message, "REFER_TO_ISSUER_SP");
				}
			else if( INVLD_MERCHANT==rc){
				sprintf(message, "INVLD_MACHINE");
				}
			else if( PLS_PICK_UP_CARD==rc){
				sprintf(message, "PLS_PICK_UP_CARD");
				}
			else if( DO_NOT_HONOUR==rc){
				sprintf(message, "DO_NOT_HONOUR");
				}
			else if( ERROR==rc){
				sprintf(message, "ERROR");
				}
			else if( Pickup_Card==rc){
				sprintf(message, "Pickup_Card");
				}
			else if( VERIFY_ID_AND_SIGN==rc){
				sprintf(message, "VERIFY_ID_AND_SIGN");
				}
			else if( REQUEST_IN_PROGRESS==rc){
				sprintf(message, "REQUEST_IN_PROGRESS");
				}
			else if( Appvd_for_Partial_Amt==rc){
				sprintf(message, "Appvd_for_Partial_Amt");
				}
			else if( Approved_VIP==rc){
				sprintf(message, "Approved_VIP");
				}
			else if( INVLD_TRANSACTION==rc){
				sprintf(message, "INVLD_TRANSACTION");
				}
			else if( INVLD_AMT==rc){
				sprintf(message, "INVLD_AMT");
				}
			else if( INVLD_CARD_NUM==rc){
				sprintf(message, "INVLD_CARD_NUM");
				}
			else if( No_such_Issuer==rc){
				sprintf(message, "No_such_Issuer");
				}
			else if( Approved==rc){
				sprintf(message, "Approved");
				}
			else if( Customer_Cancellation==rc){
				sprintf(message, "Customer_Cancellation");
				}
			else if( Customer_Dispute==rc){
				sprintf(message, "Customer_Dispute");
				}
			else if( REENTER_TRANSACTION==rc){
				sprintf(message, "REENTER_TRANSACTION");
				}
			else if( INVALID_RESPONSE ==rc){
				sprintf(message, "INVALID_RESPONSE");
				}
			else if( NO_TRANSACTIONS==rc){
				sprintf(message, "NO_TRANSACTIONS");
				}
			else if( Suspected_Malfunction==rc){
				sprintf(message, "Suspected_Malfunction");
				}
			else if( Unaccepted_Trans_Fee==rc){
				sprintf(message, "Unaccepted_Trans_Fee");
				}
			else if( File_Upd_not_Suported==rc){
				sprintf(message, "File_Upd_not_Suported");
				}
			else if( ERRFILE_NOT_FOUND==rc){
				sprintf(message, "ERRFILE_NOT_FOUND");
				}
			else if( Dup_Rec==rc){
				sprintf(message, "Dup_Rec");
				}
			else if( Field_Edit_Error==rc){
				sprintf(message, "Field_Edit_Error");
				}
			else if( File_Locked_out==rc){
				sprintf(message, "File_Locked_out");
				}
			else if( File_Update_Error==rc){
				sprintf(message, "File_Update_Error");
				}
			else if( FORMAT_ERROR==rc){
				sprintf(message, "FORMAT_ERROR");
				}
			else if( BANK_NOT_SUPPORTED==rc){
				sprintf(message, "BANK_NOT_SUPPORTED");
				}
			else if( Completed_Partially==rc){
				sprintf(message, "Completed_Partially");
				}
			else if( EXPIRED_CARD==rc){
				sprintf(message, "EXPIRED_CARD");
				}
			else if( Suspected_Fraud==rc){
				sprintf(message, "Suspected_Fraud");
				}
			else if( Contact_Acquirer==rc){
				sprintf(message, "Contact_Acquirer");
				}
			else if( Restricted_Card==rc){
				sprintf(message, "Restricted_Card");
				}
			else if( Call_Acq_Security==rc){
				sprintf(message, "Call_Acq_Security");
				}
			else if( PIN_tries_Exceeded==rc){
				sprintf(message, "PIN_tries_Exceeded");
				}
			else if( No_Credit_Account==rc){
				sprintf(message, "No_Credit_Account");
				}
			else if( FUNC_NOT_SUPPORTED==rc){
				sprintf(message, "FUNC_NOT_SUPPORTED");
				}
			else if( LOST_CARD==rc){
				sprintf(message, "LOST_CARD");
				}
			else if( No_Universal_Account==rc){
				sprintf(message, "No_Universal_Account");
				}
			else if( STOLEN_CARD==rc){
				sprintf(message, "STOLEN_CARD");
				}
			else if( No_Investment_Account==rc){
				sprintf(message, "No_Investment_Account");
				}
			else if( ISO_error_45==rc){
				sprintf(message, "ISO_error_45");
				}
			else if( ISO_error_46==rc){
				sprintf(message, "ISO_error_46");
				}
			else if( ISO_error_47==rc){
				sprintf(message, "ISO_error_47");
				}
			else if( ISO_error_48==rc){
				sprintf(message, "ISO_error_48");
				}
			else if( ISO_error_49==rc){
				sprintf(message, "ISO_error_49");
				}
			else if( ISO_error_50==rc){
				sprintf(message, "ISO_error_50");
				}
			else if( INSUFFICIENT_FUND==rc){
				sprintf(message, "INSUFFICIENT_FUND");
				}
			else if( NO_CHEQUE_ACC==rc){
				sprintf(message, "NO_CHEQUE_ACC");
				}
			else if( No_Savings_Account==rc){
				sprintf(message, "No_Savings_Account");
				}
			else if( EXPIRED_CARD2==rc){
				sprintf(message, "EXPIRED_CARD2");
				}
			else if( Incorrect_PIN==rc){
				sprintf(message, "Incorrect_PIN");
				}
			else if( No_Card_Record==rc){
				sprintf(message, "No_Card_Record");
				}
			else if( Txn_not_Permtd_card==rc){
				sprintf(message, "Txn_not_Permtd_card");
				}
			else if( TRANS_NOT_PERMITTED==rc){
				sprintf(message, "TRANS_NOT_PERMITTED");
				}
			else if( Suspected_Fraud2==rc){
				sprintf(message, "Suspected_Fraud2");
				}
			else if( Contact_Acquirer2==rc){
				sprintf(message, "Contact_Acquirer2");
				}
			else if( EXCEED_LIMIT==rc){
				sprintf(message, "EXCEED_LIMIT");
				}
			else if( Restricted_Card2==rc){
				sprintf(message, "Restricted_Card2");
				}
			else if( Security_Violation==rc){
				sprintf(message, "Security_Violation");
				}
			else if( Org_Amount_Incorrect==rc){
				sprintf(message, "Org_Amount_Incorrect");
				}
			else if( Freq_Limit_Exceed==rc){
				sprintf(message, "Freq_Limit_Exceed");
				}
			else if( call_acq_security==rc){
				sprintf(message, "call_acq_security");
				}
			else if( Hard_Capture==rc){
				sprintf(message, "Hard_Capture");
				}
			else if( Resp_Recvd_too_Late==rc){
				sprintf(message, "Resp_Recvd_too_Late");
				}
			else if( Unavalible==rc){
				sprintf(message, "Tidak Ditemukan");
				}
			else if( ISO_error_70==rc){
				sprintf(message, "ISO_error_70");
				}
			else if( ISO_error_71==rc){
				sprintf(message, "ISO_error_71");
				}
			else if( ISO_error_72==rc){
				sprintf(message, "ISO_error_72");
				}
			else if( ISO_error_73==rc){
				sprintf(message, "ISO_error_73");
				}
			else if( ISO_error_74==rc){
				sprintf(message, "ISO_error_74");
				}
			else if( PIN_TRIES_EXCEED==rc){
				sprintf(message, "PIN_TRIES_EXCEED");
				}
			else if( KEY_SYNC_ERROR==rc){
				sprintf(message, "KEY_SYNC_ERROR");
				}
			else if( Resvd_for_Nat_use1==rc){
				sprintf(message, "Resvd_for_Nat_use1");
				}
			else if( OLD_ROC_NOT_FOUND==rc){
				sprintf(message, "OLD_ROC_NOT_FOUND");
				}
			else if( BATCH_ALREADY_OPEN==rc){
				sprintf(message, "BATCH_ALREADY_OPEN");
				}
			else if( Resvd_for_Nat_use2==rc){
				sprintf(message, "Resvd_for_Nat_use2");
				}
			else if( Private_error_81==rc){
				sprintf(message, "Private_error_81");
				}
			else if( Private_error_82==rc){
				sprintf(message, "Private_error_82");
				}
			else if( Private_error_83==rc){
				sprintf(message, "Private_error_83");
				}
			else if( Private_error_84==rc){
				sprintf(message, "Private_error_84");
				}
			else if( BATCH_NOT_FOUND==rc){
				sprintf(message, "BATCH_NOT_FOUND");
				}
			else if( Private_error_86==rc){
				sprintf(message, "Private_error_86");
				}
			else if( Private_error_87==rc){
				sprintf(message, "Private_error_87");
				}
			else if( HAVE_CM_CALL_AMEX==rc){
				sprintf(message, "HAVE_CM_CALL_AMEX");
				}
			else if( Resvd_for_Nat_use3==rc){
				sprintf(message, "Resvd_for_Nat_use3");
				}
			else if( Cutoff_in_Process==rc){
				sprintf(message, "Cutoff_in_Process");
				}
			else if( NETWORK_ERROR==rc){
				sprintf(message, "NETWORK_ERROR");
				}
			else if( Trans_cant_be_Routed==rc){
				sprintf(message, "Trans_cant_be_Routed");
				}
			else if( Txn_cant_be_Completed==rc){
				sprintf(message, "Txn_cant_be_Completed");
				}
			else if( DUPL_TRANSMISSION==rc){
				sprintf(message, "DUPL_TRANSMISSION");
				}
			else if( RECONCILE_ERROR==rc){
				sprintf(message, "RECONCILE_ERROR");
				}
			else if( INVALID_MESSAGE==rc){
				sprintf(message, "INVALID_MESSAGE");
				}
			else if( Resvd_for_Nat_use4==rc){
				sprintf(message, "Resvd_for_Nat_use4");
				}
			else if( Resvd_for_Nat_use5==rc){
				sprintf(message, "Resvd_for_Nat_use5");
				}
			else if( Resvd_for_Nat_use6==rc){
				sprintf(message, "Resvd_for_Nat_use6");
			}
	PubDispString(message, 4|DISP_LINE_CENTER);
	ucKey = PubWaitKey(USER_OPER_TIMEOUT);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		return ERR_USERCANCEL;
	}
	return ucKey;
}


int TransPpob(){
	int		iRet;
	uchar	ucEntryMode, szTitle,ucAcqIndex;

	iRet = TransInit(PPOB);
	if( iRet!=0 )
		{
			return iRet;
		}
	/*sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}*/
		//ScrCls();
		//DispTransName();
		/*iRet = GetCard(CARD_INSERTED|CARD_SWIPED|CARD_KEYIN|SKIP_DETECT_ICC|SKIP_CHECK_ICC);
		if( iRet!=0 )
		{
			return iRet;
		}
		iRet = SetPIN();
		if( iRet!=0 )
		{
			return iRet;
		}else{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		}*/
		//ScrCls();
		//DispTransName();
		SetCommReqField();
		iRet = SendRecvPacket();
		
		/*if( iRet!=0 )
		{
			DispErr(atoi(glRecvPack.szBit39));
			return 0;
		}*/
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){;
			if(memcmp(glRecvPack.szBit39, "01", 2)==0){
				DispErrMessage();
				return ERR_NO_DISP;
			}else{
				return ERR_HOST_REJ;
			}
		}
		iRet = AfterTranPinterpay();
		return iRet;
}

int TransPLNPra()
{
	int		iRet;
	uchar	ucEntryMode, ucAcqIndex, szTitle[16+1];
	

	iRet = TransInit(PLNPRA);
	if( iRet!=0 )
		{
			return iRet;
		}
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}

	iRet = SetAcc();
		if( iRet!=0 )
		{
			return iRet;
		}

	iRet = SetNilai();
		if( iRet!=0 )
		{
			return iRet;
		}


	iRet = SetNoHp();
		if( iRet!=0 )
		{
			return iRet;
		}
		ucEntryMode = CARD_SWIPED|CARD_KEYIN;
			iRet = GetCard(ucEntryMode);
			if( iRet!=0 )
			{
				return iRet;
			}
		ScrCls();
		DispTransName();
		SetCommReqField();
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
		iRet = AfterTranPinterpay();
		return iRet;
}

int TransPurchaseItem(uchar * kodeProduk, uchar * codeppob, uchar * provider)
{
	int		iRet;
	uchar	ucEntryMode, ucAcqIndex, szTitle[16+1];

	iRet = TransInit(PULSA);
	if( iRet!=0 )
	{
		return iRet;
		}
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
	strcpy(glProcInfo.stTranLog.szKodeProduk,kodeProduk);
	strcpy(glProcInfo.stTranLog.szKodePpob,codeppob);
	strcpy(glProcInfo.stTranLog.szProvider,provider);
	iRet = SetNoHp();
	if( iRet!=0 )
	{
		return iRet;
	}
	iRet = SetAmt();
		if( iRet!=0 )
		{
			return iRet;
		}
	ucEntryMode = CARD_SWIPED|CARD_KEYIN;
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

		ScrCls();
		DispTransName();
		/*iRet = SetPIN();
		if( iRet!=0 )
		{
			return iRet;
		}else{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		}*/
		SetCommReqField();
		iRet = SendRecvPacket();
		if(memcmp(glRecvPack.szBit39, "00", 2)!=0 ){;
			if(memcmp(glRecvPack.szBit39, "01", 2)==0){
				DispErrMessage();
				return ERR_NO_DISP;
			}else{
				return ERR_HOST_REJ;
			}
		}
		if( iRet!=0 )
		{
			return iRet;
		}
		iRet = AfterTranPinterpay();
		return iRet;
}


// Settle all acquirer.
int TransSettleSubNew(void)
{
	int		iRet, bFirstSettle;
	uchar	szBuff[25];
	uchar acqUploadStatus=0;
	
	// don't need to check zero total. it has been check outside.

	TransInit(TRANS_TYPE_SETTLEMENT);

	sprintf((char*)glProcInfo.szSettleMsg, "CLOSED");

	if( glCurTlmTmsAcq.acqStlRxTO<150 )	
	{	
		glCurTlmTmsAcq.acqStlRxTO += glCurTlmTmsAcq.acqStlRxTO/2;
	}
	else
	{
		glCurTlmTmsAcq.acqStlRxTO = 250;
	}
	
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	} 

	iRet = OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]==TRUE)
	{
		iRet = TransUpLoad();
		if( iRet!=0 )
		{
			return iRet;
		}
		bFirstSettle = FALSE;
	}
	else
		bFirstSettle = TRUE;
	
	while( 1 )
	{
		ScrCls();
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SETTLEMENT;
		
		sprintf(szBuff, "SETTLE%10s", glCurTlmTmsAcq.acqName);
		PubDispString(szBuff, DISP_LINE_CENTER|DISP_LINE_REVER|0);

		SetCommReqField();

		sprintf((char *)glSendPack.szBit3, "%s", bFirstSettle ? "920000" : "960000");

		memset(glSendPack.szBit63, '0', 120);
		
		//if(strstr(glCurTlmTmsAcq.acqName, "DEBIT")!=NULL )  //4212
		//{
			memset(&glSendPack.szBit63[2], '0', 30); 
		//	sprintf((char *)&glSendPack.szBit63[2], "%03d%12.12s%03d%12.12s",
		//					glTransTotal.uiSaleCnt,   glTransTotal.szSaleAmt,
		//					glTransTotal.uiRefundCnt, glTransTotal.szRefundAmt);
		//	memset(&glSendPack.szBit63[30+2], '0', 30);
		//}		
		//else
		//{
			sprintf((char *)&glSendPack.szBit63[2], "%03d%12.12s%03d%12.12s",
							glTransTotal.uiSaleCnt,   glTransTotal.szSaleAmt,
							glTransTotal.uiRefundCnt, glTransTotal.szRefundAmt);
		//	sprintf((char *)&glSendPack.szBit63[32], "%03d%12.12s",
		//					glTransTotal.uiSaleManualCnt,   glTransTotal.szSaleManualAmt);
		//	memset(&glSendPack.szBit63[45+2], '0', 15); //should be 60+2
		//}
		
		PubLong2Char(90L, 2, glSendPack.szBit63);

		sprintf((char *)glSendPack.szBit60, "%06lu", glCurTlmTmsAcq.ulCurBatchNo);

		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
#ifdef BANK_BRI
		displayResponse();
#endif		
		if( !bFirstSettle )
		{

			if( memcmp(glRecvPack.szBit39, "00", 2)==0 )
			{
				glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
				SaveAcqUploadStatus();				
			}				
			else if( memcmp(glRecvPack.szBit39, "95", 2)==0 )
			{
				glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
				SaveAcqUploadStatus();
			}
			else
				return ERR_NO_DISP;
			
			break;
		}

		if( memcmp(glRecvPack.szBit39, "95", 2)!=0 )
		{

			glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
			SaveAcqUploadStatus();
		
			break;
		}

		// display error information
		//DispResult(ERR_HOST_REJ);

		sprintf((char*)glProcInfo.szSettleMsg, "CLOSED (95)");
		glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex] = TRUE;
		SaveAcqUploadStatus();
		
		iRet = TransUpLoad();
		if( iRet!=0 )
		{
			return iRet;
		}
		bFirstSettle = FALSE;
	}
	
	
	if( memcmp(glRecvPack.szBit39, "00", 2)==0 )
	{
		GetNewInvoiceNo();


		// save settle information for reprint
		stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = TRUE;
		stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex]     = glSysCtrl.ulInvoiceNo;
		stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex] = glCurTlmTmsAcq.ulCurBatchNo;
		//sprintf((char *)stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex], "%s", glProcInfo.szSettleMsg);
		memcpy(&stRePrnStlInfo.stAcqTotal[glCurTlmTmsAcq.ucIndex], &glAcqTotal[glCurTlmTmsAcq.ucIndex], sizeof(TOTAL_INFO));
//		memcpy(stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], glIssuerTotal, sizeof(glIssuerTotal));

		return 0;
	}

	return ERR_NO_DISP;
}


int TransSettleNew(void)
{
	int		iRet;
	uchar	ucAcqIndex;

	TransInit(TRANS_TYPE_SETTLEMENT); 

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_SETTLE_PWD) )
	{
		if( PasswordSettle()!=0 )
		{
			return ERR_NO_DISP;
		}
	}


	ScrCls();
	DispTransName();
	DispProcess();

	CalcTotal(ACQ_ALL);

	if (ChkIfZeroTotal(&glTransTotal))
	{
		ScrClrLine(2, 7);
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}


	
	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		memset(&glTransTotal, 0, sizeof(TOTAL_INFO));
		memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

		if (ChkIfZeroTotal(&glTransTotal))
		{
			continue;
		}

		SetCurAcq(ucAcqIndex);
		memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));
		iRet = DispPurchaseTotal(TRUE);
		if( iRet!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	memset(&stRePrnStlInfo, 0, sizeof(REPRN_STL_INFO));

	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		memset(&glTransTotal, 0, sizeof(TOTAL_INFO));
		memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

		if (ChkIfZeroTotal(&glTransTotal))
		{
			continue;
		}

		SetCurAcq(ucAcqIndex);
		iRet = TransSettleSubNew();
		if( iRet!=0 )
		{
			return iRet;
		}
		
		// last error ?
	}

	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		SetCurAcq(ucAcqIndex);
		if(stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex])
		{
			glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex] ;
			glSysCtrl.stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex]     = stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex];
			glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex] = stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex];
			//sprintf((char *)glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex], "%s", (char*)stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex]);
			memcpy(&glSysCtrl.stRePrnStlInfo.stAcqTotal[glCurTlmTmsAcq.ucIndex], &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));
			//memcpy(glSysCtrl.stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], sizeof(glIssuerTotal));
		}
		else
		{
			glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = FALSE;
		}
	}

	memcpy(&glSysCtrl.stRePrnStlInfo.stCardTotal, &glCardTotal, sizeof(glCardTotal));
	SaveRePrnStlInfo();

	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

		if (ChkIfZeroTotal(&glTransTotal))
		{
			continue;
		}

		SetCurAcq(ucAcqIndex);
		glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex] = S_CLR_LOG;
	}

	SaveSysCtrlBase();
	CommOnHook(FALSE);


	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

		if (ChkIfZeroTotal(&glTransTotal))
		{
			continue;
		}

		SetCurAcq(ucAcqIndex);
		ClearRecord(glCurTlmTmsAcq.acqID);
	}
	

	PrintSettleNew(PRN_NORMAL);
	
	return 0;
}


// 结算
int TransSettle(void)
{
	int		iRet;
	uchar	ucAcqIndex, szTitle[16+1];
	uchar	ucForceSettle;

	TransInit(TRANS_TYPE_SETTLEMENT);

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_SETTLE_PWD) )
	{
		if( PasswordSettle()!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
	iRet = SelectAcqCreditDebitOnly(TRUE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return ERR_NO_DISP;
	}

	ScrCls();
	DispTransName();
	DispProcess();
	if( ucAcqIndex==MAX_ACQ )
	{
		CalcTotal(ACQ_ALL);
	}
	else
	{
		CalcTotal(glCurTlmTmsAcq.acqID);
	}

	iRet = DispTransTotal(FALSE);
	if( iRet!=0 )
	{
		return ERR_NO_DISP;
	}

	ucForceSettle = FALSE;
	if( ChkIfZeroTotal(&glTransTotal) )
	{
		ScrCls();
		DispTransName();
		PubDispString(_T("EMPTY BATCH"),     3|DISP_LINE_LEFT);
		PubDispString(_T("SETTLE ANYWAY ?"), 5|DISP_LINE_LEFT);
		if (PubYesNo(USER_OPER_TIMEOUT)!=0)
		{
			return ERR_NO_DISP;
		}
		ucForceSettle = TRUE;
	}

#if 0
	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_SETTLE_PWD) )
	{
		if( PasswordSettle()!=0 )
		{
			return ERR_NO_DISP;
		}
	}
#endif

	if( ucAcqIndex!=MAX_ACQ )
	{
		return TransSettleSub();
	}

	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		if(strstr(glTlmSysParam.Acq[ucAcqIndex].acqName, "CREDIT")!=NULL ) 
		{
		
			SetCurAcq(ucAcqIndex);
			memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

			if (ChkIfZeroTotal(&glTransTotal) && !ucForceSettle)
			{
				continue;
			}

			iRet = TransSettleSub();
			if( iRet!=0 )
			{
				if( iRet!=ERR_NO_DISP )
				{
					return iRet;
				}
			}
		}
		
		else if(strstr(glTlmSysParam.Acq[ucAcqIndex].acqName, "DEBIT")!=NULL ) 
		{
		
			SetCurAcq(ucAcqIndex);
			memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));

			if (ChkIfZeroTotal(&glTransTotal) && !ucForceSettle)
			{
				continue;
			}

			iRet = TransSettleSub();
			if( iRet!=0 )
			{
				if( iRet!=ERR_NO_DISP )
				{
					return iRet;
				}
			}
		}

		// last error ?
	}

	return 0;
}

// 结算当前收单行(glCurAcq)
// Settle current acquirer.
int TransSettleSub(void)
{
#ifdef ENABLE_EMV
//	int		iLength;
#endif
	int		iRet, bFirstSettle;
	uchar	szBuff[20];

	uchar acqUploadStatus=0;
	// don't need to check zero total. it has been check outside.

	TransInit(TRANS_TYPE_SETTLEMENT);

	sprintf((char*)glProcInfo.szSettleMsg, "CLOSED");

#ifdef	__TELIUM_PARAM__
	if( glCurTlmTmsAcq.acqStlRxTO<150 )	// ???? How about Tcp
	{	// 结算时，timeout加长
		glCurTlmTmsAcq.acqStlRxTO += glCurTlmTmsAcq.acqStlRxTO/2;
	}
	else
	{
		glCurTlmTmsAcq.acqStlRxTO = 250;
	}
#else
	if( glCurAcq.ucPhoneTimeOut<150 )	// ???? How about Tcp
	{	// 结算时，timeout加长
		glCurAcq.ucPhoneTimeOut += glCurAcq.ucPhoneTimeOut/2;
	}
	else
	{
		glCurAcq.ucPhoneTimeOut = 250;
	}
#endif
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]==TRUE)
	{
		iRet = TransUpLoad();
		if( iRet!=0 )
		{
			return iRet;
		}
		bFirstSettle = FALSE;
	}
	else
		bFirstSettle = TRUE;
	
	while( 1 )
	{
		TransInit(TRANS_TYPE_SETTLEMENT);
		PubDispString(glCurTlmTmsAcq.acqName, DISP_LINE_LEFT|2);

		SetCommReqField();

		sprintf((char *)glSendPack.szBit3, "%s", bFirstSettle ? "920000" : "960000");

		memset(glSendPack.szBit63, '0', 120);
		if(strstr(glCurTlmTmsAcq.acqName, "DEBIT")!=NULL ) 
		{
			memset(&glSendPack.szBit63[2], '0', 30); 
			sprintf((char *)&glSendPack.szBit63[30+2], "%03d%12.12s%03d%12.12s",
							glTransTotal.uiSaleCnt,   glTransTotal.szSaleAmt,
							glTransTotal.uiRefundCnt, glTransTotal.szRefundAmt);
			memset(&glSendPack.szBit63[45+2], '0', 45);
		}		
		else
		{
			sprintf((char *)&glSendPack.szBit63[2], "%03d%12.12s%03d%12.12s",
							glTransTotal.uiSaleNonManualCnt,   glTransTotal.szSaleNonManualAmt,
							glTransTotal.uiRefundCnt, glTransTotal.szRefundAmt);
			sprintf((char *)&glSendPack.szBit63[32], "%03d%12.12s",
							glTransTotal.uiSaleManualCnt,   glTransTotal.szSaleManualAmt);

			memset(&glSendPack.szBit63[45+2], '0', 30); //should be 60+2
		}
		
		PubLong2Char(90L, 2, glSendPack.szBit63);

		sprintf((char *)glSendPack.szBit60, "%06lu", glCurTlmTmsAcq.ulCurBatchNo);

		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
		if( !bFirstSettle )
		{

			if( memcmp(glRecvPack.szBit39, "00", 2)==0 )
			{
				glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
				SaveAcqUploadStatus();				
			}				
			else if( memcmp(glRecvPack.szBit39, "95", 2)==0 )
			{
//				memcpy(glRecvPack.szBit39, "77", 2);
				glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
				SaveAcqUploadStatus();
			}
			else
				return ERR_NO_DISP;
			
			break;
		}

		if( memcmp(glRecvPack.szBit39, "95", 2)!=0 )
		{

			glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]=FALSE;
			SaveAcqUploadStatus();
		
			break;
		}

		// display error information
		DispResult(ERR_HOST_REJ);

		sprintf((char*)glProcInfo.szSettleMsg, "CLOSED(95)");
		glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex] = TRUE;
		SaveAcqUploadStatus();
		
		iRet = TransUpLoad();
		if( iRet!=0 )
		{
			return iRet;
		}
//		glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex] = FALSE;
//		SaveAcqUploadStatus();
		bFirstSettle = FALSE;
	}
	
	if( memcmp(glRecvPack.szBit39, "77", 2)==0 )
	{
		sprintf((char*)glProcInfo.szSettleMsg, "CLOSED(77 RECONCILE ERROR)");
	}

	if( memcmp(glRecvPack.szBit39, "00", 2)==0 ||
		(memcmp(glRecvPack.szBit39, "77", 2)==0) )
	{
#ifdef	__TELIUM_PARAM__
		glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex] = S_CLR_LOG;
#else
		glSysCtrl.sAcqStatus[glCurAcq.ucIndex] = S_CLR_LOG;
#endif
		SaveSysCtrlBase();
		CommOnHook(FALSE);
#ifdef	__TELIUM_PARAM__
		ClearRecord(glCurTlmTmsAcq.acqID);
#else
		ClearRecord(glCurAcq.ucKey);
#endif
		GetNewInvoiceNo();

		DispPrinting();
		iRet = PrintSettle(PRN_NORMAL);
		if( iRet!=0 )
		{
			PubDispString(_T("PLEASE REPRINT"), 4|DISP_LINE_CENTER);
			PubWaitKey(3);
			iRet = ERR_NO_DISP;
		}

		ScrClrLine(2,7);
		if( memcmp(glRecvPack.szBit39, "77", 2)==0 )
		{
			PubDispString(_T("RECONCILE ERROR"), 4|DISP_LINE_CENTER);
		}
		else if( glTransTotal.uiSaleCnt==0 && glTransTotal.uiRefundCnt==0 )
		{
			PubDispString(_T("NO TXN TOTAL"), 4|DISP_LINE_CENTER);
		}
		else
		{
#ifdef	__TELIUM_PARAM__
			sprintf((char *)szBuff, "BATCH %06ld", glCurTlmTmsAcq.ulCurBatchNo);
#else
			sprintf((char *)szBuff, "BATCH %06ld", glCurAcq.ulCurBatchNo);
#endif
			PubDispString(szBuff,   3|DISP_LINE_CENTER);
			PubDispString("CLOSED", 5|DISP_LINE_CENTER);
		}
		PubBeepOk();
		PubWaitKey(5);
		return iRet;
	}

	return 0;
}

// 批上送
int TransUpLoad(void)
{
	int			iRet;
	ushort		uiIndex, uiTranNum, uiCnt;
	uchar		szTotalAmt[12+1];
	TRAN_LOG	stLog;

	TransInit(TRANS_TYPE_BATCH_UPLOAD);
#ifdef	__TELIUM_PARAM__
	uiTranNum = GetTranLogNum(glCurTlmTmsAcq.acqID);
#else
	uiTranNum = GetTranLogNum(glCurAcq.ucKey);
#endif
	for(uiCnt=uiIndex=0; uiIndex<MAX_TRANLOG; uiIndex++)
	{
#ifdef	__TELIUM_PARAM__
		if( glSysCtrl.sAcqKeyList[uiIndex]!=glCurTlmTmsAcq.acqID )
		{
			continue;
		}
#else
		if( glSysCtrl.sAcqKeyList[uiIndex]!=glCurAcq.ucKey )
		{
			continue;
		}
#endif
		uiCnt++;
		ScrPrint(70, 0, CFONT|REVER, "%03d/%03d", uiCnt, uiTranNum);

		memset(&stLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTranLog(&stLog, uiIndex);
		if( iRet!=0 )
		{
			return iRet;
		}
		if( stLog.ucTranType==TRANS_TYPE_PREAUTH || (stLog.uiStatus & TS_NOT_UPLOAD) )
		{
			continue;
		}
		if(stLog.uiStatus & TS_VOID )
		{	
			continue;
		}

		if(stLog.ucUploadStatus)	//already upload.
			continue;
		
		SetCommReqField();
		sprintf((char *)glSendPack.szBit2, "%.19s", stLog.szPan);
		sprintf((char *)glSendPack.szBit3, "%.*s", LEN_PROC_CODE,
				glTranConfig[stLog.ucTranType].szProcCode);
		glSendPack.szBit3[LEN_PROC_CODE-1] = '1';
		
		PubAscAdd(stLog.szAmount, stLog.szTipAmount, 12, szTotalAmt);
		//PubAddHeadChars(szTotalAmt, 12, '0');  no need: already 12 digits
		sprintf((char *)glSendPack.szBit4, "%.12s", szTotalAmt);
		if( !ChkIfZeroAmt(stLog.szTipAmount) )
		{
			sprintf((char *)glSendPack.szBit54, "%.12s", stLog.szTipAmount);
		}

		sprintf((char *)glSendPack.szBit12, "%.6s", &stLog.szDateTime[8]);
		sprintf((char *)glSendPack.szBit13, "%.4s", &stLog.szDateTime[4]);
		sprintf((char *)glSendPack.szBit14,   "%.4s", stLog.szExpDate);	// [1/11/2007 Tommy]
		SetEntryMode(&stLog);
		sprintf((char *)glSendPack.szBit25, "%.2s",  stLog.szCondCode);
		sprintf((char *)glSendPack.szBit37,      "%.12s", stLog.szRRN);
		sprintf((char *)glSendPack.szBit38, "%.6s",  stLog.szAuthCode);

		sprintf((char *)glSendPack.szBit39, "%.2s", stLog.szRspCode);
		if( stLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PubLong2Char((ulong)stLog.uiIccDataLen, 2, glSendPack.szBit55);
			memcpy(&glSendPack.szBit55[2], stLog.sIccData, stLog.uiIccDataLen);
		}

		memcpy(glSendPack.szBit62, "\x00\x06", 2);
		sprintf((char *)&glSendPack.szBit62[2], "%06ld", stLog.ulInvoiceNo);

//		memcpy(glSendPack.szBit60, "\x00\x04", 2);
//		memcpy((char *)&glSendPack.szBit60[2], glTranConfig[stLog.ucTranType].szTxMsgID, 4);

		memcpy(glSendPack.szBit60, glTranConfig[stLog.ucTranType].szTxMsgID, 4);

		if( stLog.uiStatus & TS_OFFLINE_SEND )
		{
//			memcpy(glSendPack.szBit60, "\x00\x04", 2);
//			memcpy((char *)&glSendPack.szBit60[2], "0220", 4);

			memcpy(glSendPack.szBit60, "0220", 4);
			if( ChkAcqOption(ACQ_DBS_FEATURE) )
			{
				if( stLog.ucTranType!=TRANS_TYPE_OFF_SALE && stLog.ucTranType==TRANS_TYPE_SALE_COMP &&
					!(stLog.uiStatus & TS_FLOOR_LIMIT) )
				{
//					memcpy(glSendPack.szBit60, "\x00\x04", 2);
//					memcpy((char *)&glSendPack.szBit60[2], "0200", 4);

					memcpy(glSendPack.szBit60, "0200", 4);
					
				}
			}
		}
		else if( stLog.uiStatus & TS_VOID )
		{
//			memcpy(glSendPack.szBit60, "\x00\x04", 2);
//			memcpy((char *)&glSendPack.szBit60[2], "0200", 4);

			memcpy(glSendPack.szBit60, "0200", 4);
		}
		sprintf((char *)&glSendPack.szBit60[4], "%06ld%12s", stLog.ulSTAN, "");

		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
		DispResult(0);
		
		stLog.ucUploadStatus=1;	//already upload
		UpdateTranLog(&stLog, uiIndex);
	}

	return 0;
}

// irf, 20121129
int TransPinVerify(void)
{
	int		iRet;

	iRet = TransInit(TRANS_TYPE_PIN_VERIFY);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetCard(CARD_SWIPED);	//mag only
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransPinVerifySub();
}


// irf, 20121129
int TransPinVerifySub(void)
{
	int		iRet;

	if( !ChkIfTranAllow(glProcInfo.stTranLog.ucTranType) )
	{
		return ERR_NO_DISP;
	}

	if( !ChkIfKeysAlreadyLoaded(glProcInfo.stTranLog.ucTranType) )
	{
		return ERR_NO_DISP;
	}

#if 0
	ScrCls();
	DispTransName();
	iRet = GetPIN(FALSE);
	if( iRet!=0 )
	{
		return iRet;
	}
#else
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);
	if( iRet!=0 )
	{
		return iRet;
	}
	
#endif

	SetCommReqField();
	return TranProcess();
}

/*
#ifdef	BANK_MANDIRI
int TransLogon(void)
{
	int iRet;
	word idx;
	uchar logonFlag;
	
	uchar acqId;
	uchar bufTWK[999];

	SYS_PROC_INFO stProcInfoBak;
	
	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		mapMove(acqBeg, idx);
		mapGetByte(acqID, acqId);
		if(acqId==glCurTlmTmsAcq.acqID)
			break;
	}

	if(glCurTlmTmsIss.issPinOpt==0x30)
		return 0;

	mapGetByte(acqLogonFlag, logonFlag);
	
	if((glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct & NEED_LOGON) && (!logonFlag))
	{
			// backup current process information
		memcpy(&glProcInfo.stSendPack, &glSendPack, sizeof(STISO8583));
		memcpy(&stProcInfoBak, &glProcInfo, sizeof(SYS_PROC_INFO));

//		hex_dump_char("stProcInfoBak", (uchar*)&stProcInfoBak, sizeof(TRAN_LOG));
		
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_LOGON;

		ScrCls();
		DispTransName();
	}
	else
		return 0;

	SetCommReqField();
	iRet = SendRecvPacket();
	if(iRet != 0)
	{
		return ERR_NO_DISP;
	}

	if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		DispResult(ERR_HOST_REJ);
		return ERR_NO_DISP;
	}

	mapPutByte(acqLogonFlag, 1);

	MEM_ZERO(bufTWK);
	memcpy(bufTWK, glRecvPack.szBit58+2, PubChar2Long(glRecvPack.szBit58, 2, NULL));
//	hex_dump_char("bufTWK", bufTWK, PubChar2Long(glRecvPack.szBit58, 2, NULL));
	iRet = parseTMWK(bufTWK);
	if(iRet<0)
	{
		return ERR_NO_DISP;
	}	

	iRet = GetPinKeyPerAcq();
	if(iRet<0)
	{
		return ERR_NO_DISP;
	}

	memcpy(&glProcInfo, &stProcInfoBak, sizeof(SYS_PROC_INFO));

	ScrCls();
	DispTransName();

	SetCommReqField();

	return 0;
	
}


int TransLogonAllAcq(void)
{
	int iRet;
	word idx;
	uchar bufTWK[999];


	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		SetCurAcq((uchar)idx);
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_LOGON;

		ScrCls();
		DispTransName();

		ScrPrint(0, 0, CFONT|REVER, "LOGON %10s", glCurTlmTmsAcq.acqName);

		mapPutByte(acqLogonFlag, 0);

		SetCommReqField();
		iRet = SendRecvPacket();
		if(iRet != 0)
		{
			return ERR_NO_DISP;
		}

		if(CheckResponseCode(glRecvPack.szBit39)!=0)
			return ERR_NO_DISP;

		mapPutByte(acqLogonFlag, 1);

		MEM_ZERO(bufTWK);
		memcpy(bufTWK, glRecvPack.szBit58+2, PubChar2Long(glRecvPack.szBit58, 2, NULL));
		iRet = parseTMWK(bufTWK);
		if(iRet<0)
		{
			return ERR_NO_DISP;
		}	

		iRet = GetPinKeyPerAcq();
		if(iRet<0)
		{
			return ERR_NO_DISP;
		}
	}
	return 0;
}

#endif
*/


