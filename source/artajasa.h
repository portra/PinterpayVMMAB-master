#ifndef _ARTAJASA_H
#define _ARTAJASA_H



int MenuAtmbArtajasa(void);
int MenuAtmbArtajasaFunction(void);




int TransAjPurchase(void);
int TransAjPurchaseInq(void);
int TransAjPurchasePay(void);

int TransLogonAj(void);




int InputUangAj(void);




////////////////////////////////////////
int MenuPinterpay(void);
int MenuPinterpayPulsaElektrik(void);
int MenuPinterpayPulsaElektrikPulsa(void);
int MenuPinterpayPpob(void);



////////////////
void StringPadRight(char *string, int padded_len, char *pad);
int InputNoRekeningAj(void);





//////////////////////////////
void setPackAjPurchaseInq(void);
void setPackLogonAj(void);



////////
uchar unPackAtmbAj(void);


//////
int CetakStrukAj(uchar ucPrnFlag);
void PrnFooterAj(void);
void PrnPurchaseAj(void);


void MiniATMBSummaryReport(uchar FlagDate);
void MiniATMBDetailReport(uchar FlagDate);

void PrnHeaderTraceRespCode(void);
















#endif


