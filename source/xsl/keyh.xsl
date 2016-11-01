<?xml version="1.0"?>

<xsl:transform version = "1.0" id="keyh"
    extension-element-prefixes="uta"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
    xmlns:uta = "http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>

<xsl:template match="/">
    <xsl:text>#ifndef __KEY_H&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>#include "sys.h"&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>//This file is automatically generated from "dbs.xml" file&#010;</xsl:text>
    <xsl:text>//Don't modify it manually!&#010;</xsl:text>
    <xsl:text>//It is to be included into "str.h"&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>/** \defgroup strgroup STR Structure components&#010;</xsl:text>
    <xsl:text> *&#010;</xsl:text>
    <xsl:text> * The most of structure components are generated automatically from dbs.xml file&#010;</xsl:text>
    <xsl:text> * These files are:&#010;</xsl:text>
    <xsl:text> *  - KEY.H: header file containing the keys to access data elements and automatically generated functions declarations&#010;</xsl:text>
    <xsl:text> *  - MAP.H: header file containing internal functions and structures declarations&#010;</xsl:text>
    <xsl:text> *  - DFT.H: header file containing default values to initialize data elements&#010;</xsl:text>
    <xsl:text> *  - STR.H: All structure component declarations that are not generated automatically&#010;</xsl:text>
    <xsl:text> *  - MAP.C: Data access functions and descriptor constructors&#010;</xsl:text>
    <xsl:text> *  - DFT.C: Data base reset functions&#010;</xsl:text>
    <xsl:text> *  - RPT.C: receipt layout&#010;</xsl:text>
    <xsl:text> *  - SCN.C: graphical scenes descriptors&#010;</xsl:text>
    <xsl:text> *  - SCR.C: screen processing functions&#010;</xsl:text>
    <xsl:text> *  - STR.C: functions that are not generated autiomatically&#010;</xsl:text>
    <xsl:text> * @{&#010;</xsl:text>
    <xsl:text>*/&#010;</xsl:text>

    <xsl:text>/** Dimension macro, the number of elements between sentinels&#010;</xsl:text>
    <xsl:text> * \param BEG Starting sentinel&#010;</xsl:text>
    <xsl:text> * \param END Ending sentinel&#010;</xsl:text>
    <xsl:text> * \header str\\key.h&#010;</xsl:text>
    <xsl:text> *&#010;</xsl:text>
    <xsl:text> * The macro is used calculate a dimension of a data structure:&#010;</xsl:text>
    <xsl:text> *  - number of fields in a record&#010;</xsl:text>
    <xsl:text> *  - number of rows in a table&#010;</xsl:text>
    <xsl:text> *  - etc&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>#define DIM(BEG,END) ((END)-(BEG)-1)&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates/>    
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>/** \weakgroup mapgroup MAP&#010;</xsl:text>
    <xsl:text>MAP component contains functions generated automatically.&#010;</xsl:text>
    <xsl:text>* They are used by logical components&#010;</xsl:text>
    <xsl:text>* to manipulate with data elements&#010;</xsl:text>
    <xsl:text>* @{&#010;</xsl:text>
    <xsl:text>*/&#010;</xsl:text>

    <xsl:text>int mapInit(void); ///&lt;initialise data base descriptors&#010;</xsl:text>
    <xsl:text>int mapReset(word key); ///&lt;reset non-volatile tables and records (all if key keyBeg)&#010;</xsl:text>
    <xsl:text>int mapMove(word key,word idx); ///&lt;move cursor for subspace containing key to row idx&#010;</xsl:text>
    <xsl:text>int mapCache(int key); ///&lt;enable/disable cache&#010;</xsl:text>
    <xsl:text>int mapLoad(word key); ///&lt;load a cache buffer from DFS section / load an index into RAM&#010;</xsl:text>
    <xsl:text>int mapSave(word key); ///&lt;save a cache buffer into DFS section / save index into file&#010;</xsl:text>
    <xsl:text>int mapFlush(word key); ///&lt;flush cache buffer into DFS section&#010;</xsl:text>
    <xsl:text>int mapIns(word key); ///&lt;insert an row into an index&#010;</xsl:text>
    <xsl:text>int mapDel(word key); ///&lt;delete a row from an index&#010;</xsl:text>
    <xsl:text>int mapFind(word key,const char *pat,word beg,word end);&#010; ///&lt;perform a search operation in a multirecord&#010;</xsl:text>
    <xsl:text>int mapExport(int key,byte sec); ///&lt;export a data element into a DFS section&#010;</xsl:text>
    <xsl:text>int mapGet(word key,void *ptr,word len); ///&lt;retrieve data element&#010;</xsl:text>
    <xsl:text>int mapPut(word key,const void *ptr,word len); ///&lt;save data element&#010;</xsl:text>
    <xsl:text>word mapDatLen(word key); ///&lt;get data element length&#010;</xsl:text>
    <xsl:text>char mapDatFmt(word key); ///&lt;get data element format&#010;</xsl:text>
    <xsl:text>/** @} */&#010;</xsl:text>
    
    <xsl:text>/** \name Shortcut macros&#010;</xsl:text>
    <xsl:text>* The macros provide shortcuts to call map functions&#010;</xsl:text>
    <xsl:text>* to reduce the number of arguments&#010;</xsl:text>
    <xsl:text>* @{&#010;</xsl:text>
    <xsl:text>*/&#010;</xsl:text>

    <xsl:text>#define mapGetByte(KEY,DST) mapGet(KEY,&amp;DST,sizeof(byte))&#010;</xsl:text>
    <xsl:text>#define mapPutByte(KEY,SRC) mapPut(KEY,bPtr(SRC),sizeof(byte))&#010;</xsl:text>
    <xsl:text>#define mapGetWord(KEY,DST) mapGet(KEY,&amp;DST,sizeof(word))&#010;</xsl:text>
    <xsl:text>#define mapPutWord(KEY,SRC) mapPut(KEY,wPtr(SRC),sizeof(word))&#010;</xsl:text>
    <xsl:text>#define mapGetCard(KEY,DST) mapGet(KEY,&amp;DST,sizeof(card))&#010;</xsl:text>
    <xsl:text>#define mapPutCard(KEY,SRC) mapPut(KEY,cPtr(SRC),sizeof(card))&#010;</xsl:text>
    <xsl:text>//#define mapGetStr(KEY,DST) not to be used, it is risky!&#010;</xsl:text>
    <xsl:text>#define mapPutStr(KEY,SRC) mapPut(KEY,SRC,0)&#010;</xsl:text>
    <xsl:text>/** @} */&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>/** \weakgroup dftgroup DFT&#010;</xsl:text>
    <xsl:text>* DFT component contains functions are generated automatically.&#010;</xsl:text>
    <xsl:text>* They are used to fill the data base by default values.&#010;</xsl:text>
    <xsl:text>* The functions getCmpDat() and getCmpTim() are used to detect&#010;</xsl:text>
    <xsl:text>* the data base initialisation condition.&#010;</xsl:text>
    <xsl:text>* @{&#010;</xsl:text>
    <xsl:text>*/&#010;</xsl:text>

    <xsl:text>const char *getCmpDat(void); ///&lt;return compile date of the file dft.c&#010;</xsl:text>
    <xsl:text>const char *getCmpTim(void); ///&lt;return compile time of the file dft.c&#010;</xsl:text>
    <xsl:text>int dftReset(word key); ///&lt;fill application data base by default values&#010;</xsl:text>
    <xsl:text>/** @} */&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="declare"/>

    <xsl:text>#define __KEY_H&#010;</xsl:text>
    <xsl:text>#endif&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>/** @} */&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk"/>
<xsl:template match="uta:dbk" mode="declare"/>

<xsl:template match="uta:derivated" mode="declare"><xsl:apply-templates mode="declare"/></xsl:template>
<xsl:template match="uta:combos" mode="declare"/>
<xsl:template match="uta:converters" mode="declare"/>

<xsl:template match="uta:rptLayout" mode="declare">
    <xsl:text>/** \weakgroup rptgroup RPT&#010;</xsl:text>
    <xsl:text>* The RPT component contains functions are generated automatically&#010;</xsl:text>
    <xsl:text>* They are used to print receipts according to receipt layout.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>/** @{*/&#010;</xsl:text>

    <xsl:text>int rptReceipt(word rlo); ///&lt;print a receipt according to a given layout&#010;</xsl:text>
    
    <xsl:if test="@uta:default!=''">
      <xsl:text>/** Print a receipt layout block directly.&#010;</xsl:text>
      <xsl:text> * \param  rlo (I) block identifier&#010;</xsl:text>
      <xsl:text> * \return&#010;</xsl:text>
      <xsl:text> * - 1 if OK&#010;</xsl:text>
      <xsl:text> * - negative if failure&#010;</xsl:text>
      <xsl:text> * &#010;</xsl:text>
      <xsl:text> * If rlo key is out of the receipt layout range the function is called &#010;</xsl:text>
      <xsl:text> * defined in uta:default attribute.&#010;</xsl:text>
      <xsl:text> * The header of this function is generated automatically.&#010;</xsl:text>
      <xsl:text> * However it should be implemented somewhere else.&#010;</xsl:text>
      <xsl:text> * Any processing can be performed in this function.&#010;</xsl:text>
      <xsl:text> * The parameter value allows to identify when it is called.&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int </xsl:text>
      <xsl:value-of select="@uta:default"/>
      <xsl:text>(word rlo); ///&lt;direct receipt printing&#010;</xsl:text>
    </xsl:if>
    <xsl:text>/** @} */&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:enum">
    <xsl:text>enum e</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>{ ///&lt;</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
        
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg, ///&lt;start sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>
        
    <xsl:apply-templates/>
        
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End ///&lt;end sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>};&#010;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:enumKey">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:if test="@uta:data!=''">
        <xsl:text>= </xsl:text>
        <xsl:value-of select="@uta:data"/>
    </xsl:if>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:keySpace">
    <xsl:text>/** The key enum is used to access data elements using the functions mapGet() and mapPut().&#010;</xsl:text>
    <xsl:text> * It contains all the data elements saved in volatile and non-volatile memory.&#010;</xsl:text>
    <xsl:text> * These data elements can be organized in tables, records or multirecords.&#010;</xsl:text>
    <xsl:text> * The calculated data elements are described using converters and combos.&#010;</xsl:text>
    <xsl:text> * Index and and key arrays can be also accessed using keys in key enum.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>enum eKey{ ///&lt;access keys to all the data in the database&#010;</xsl:text>

    <xsl:text>    </xsl:text>
    <xsl:text>keyBeg, ///&lt;start sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>
        
    <xsl:apply-templates/>
        
    <xsl:text>    </xsl:text>
    <xsl:text>keyEnd ///&lt;end sentinel</xsl:text>
    <xsl:text>&#010;};&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="multi"/>
</xsl:template>

<xsl:template match="uta:dfsFile">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="uta:volatile">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="uta:derivated">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="uta:table">
    <xsl:text>        </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>

         <xsl:apply-templates/>
                
    <xsl:text>        </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End</xsl:text>               
    <xsl:if test="@uta:dim!=''">
      <xsl:text>= </xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>Beg+</xsl:text>
      <xsl:value-of select="@uta:dim"/>
      <xsl:text>+1</xsl:text>
    </xsl:if>
    <xsl:text>,</xsl:text>               
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:record|uta:converters|uta:combos|uta:find">
    <xsl:text>        </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
                
    <xsl:apply-templates/>
                
    <xsl:text>        </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End,</xsl:text>               
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:field|uta:row|uta:convert|uta:combo|uta:search|uta:index">
    <xsl:text>            </xsl:text>
    <xsl:choose>
      <xsl:when test="@uta:id!=''"><xsl:value-of select="@uta:id"/></xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="../@uta:id"/>
        <xsl:value-of select="position()-1"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text>///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
    <xsl:if test="@uta:repeat!=''">
        <xsl:text>            </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Repeat=</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>+</xsl:text>
        <xsl:value-of select="@uta:repeat"/>
        <xsl:text>-1, </xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:record" mode="multi">
    <xsl:if test="@uta:multi!=''">
        <xsl:text>enum e</xsl:text>
        <xsl:value-of select="translate(substring(@uta:multi,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:multi,2)"/>
        <xsl:text>{</xsl:text>
        <xsl:text>&#010;</xsl:text>
        
        <xsl:text>    </xsl:text>
        <xsl:value-of select="@uta:multi"/>
        <xsl:text>Beg, ///&lt;start sentinel</xsl:text>
        <xsl:text>&#010;</xsl:text>
                
        <xsl:apply-templates mode="multi"/>

        <xsl:text>    </xsl:text>
        <xsl:value-of select="@uta:multi"/>
        <xsl:text>End ///&lt;end sentinel</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>};&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:data" mode="multi">
    <xsl:apply-templates mode="multi"/>
</xsl:template>

<xsl:template match="uta:instance" mode="multi">
        <xsl:text>    </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>,&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:volatile" mode="multi"/>
<xsl:template match="uta:derivated" mode="multi"/>

<xsl:template match="uta:rptLayout">
    <xsl:text>enum eRlo{ ///&lt;Receipt layouts</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    rloBeg, ///&lt;start sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>
        
    <xsl:apply-templates/>
        
    <xsl:text>    rloEnd ///&lt;end sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>};&#010;&#010;</xsl:text>
</xsl:template>


<xsl:template match="uta:section">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:scenes">
    <xsl:text>/** \weakgroup scngroup SCN&#010;</xsl:text>
    <xsl:text>SCN component contains functions generated automatically.&#010;</xsl:text>
    <xsl:text>* They are used by logical components&#010;</xsl:text>
    <xsl:text>* to draw graphical scenes on the screen&#010;</xsl:text>
    <xsl:text>* and organize Graphical User Interface.&#010;</xsl:text>
    <xsl:text>* @{&#010;</xsl:text>
    <xsl:text>*/&#010;</xsl:text>

    <xsl:apply-templates/>

    <xsl:text>enum eScn{ ///&lt;Drawing scenes</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    scnBeg, ///&lt;start sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>    
    <xsl:apply-templates  mode="scnEnum"/>    
    <xsl:text>    scnEnd ///&lt;end sentinel&#010;</xsl:text>
    <xsl:text>};&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>enum eGui{ ///&lt;Graphical User Interface descriptors</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    guiBeg, ///&lt;start sentinel</xsl:text>
    <xsl:text>&#010;</xsl:text>    
    <xsl:apply-templates  mode="guiEnum"/>    
    <xsl:text>    guiEnd ///&lt;end sentinel&#010;</xsl:text>
    <xsl:text>};&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>int scnInit(void); ///&lt;Initialize shapes, drawings, scenes and gui descriptors&#010;</xsl:text>
    <xsl:text>int scnDraw(word scn,word x, word y); ///&lt;draw a given scene&#010;</xsl:text>
    <xsl:text>int scnPos(word scn,word pos); ///&lt;move composite shape to a given position&#010;</xsl:text>
    <xsl:text>int guiDraw(word gui); ///&lt;draw a given gui background&#010;</xsl:text>
    <xsl:text>int guiWait(word gui,tBuffer *buf); ///&lt;wait for an input&#010;</xsl:text>
    <xsl:text>int guiWaitDly(word gui,tBuffer *buf, byte dly); ///&lt;wait for an input with a given timeout&#010;</xsl:text>
    <xsl:text>int guiDialog(word gui,word key,const char *spc,byte maxLen); ///&lt;input an information&#010;</xsl:text>
    <xsl:text>int guiSelect(word gui,const Pchar * str, word pos); ///&lt;select an item from the list&#010;</xsl:text>
    <xsl:text>int guiBrowse(word gui,const tVmtBrw * brw, card pos); ///&lt;browse a list of items&#010;</xsl:text>
    <xsl:text>void scnHide(word drw,byte idx); ///&lt;hide a shape within a drawing&#010;</xsl:text>
    <xsl:text>void scnShow(word drw,byte idx); ///&lt;show a shape within a drawing&#010;</xsl:text>

    <xsl:text>/** @} */&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:scene"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:shape"/>
<xsl:template match="uta:drawing"/>
<xsl:template match="uta:scene" mode="scnEnum"><xsl:apply-templates mode="scnEnum"/></xsl:template>

<xsl:template match="uta:shape" mode="scnEnum">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:drawing" mode="scnEnum">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:input" mode="guiEnum">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> ///&lt;</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

</xsl:transform>
