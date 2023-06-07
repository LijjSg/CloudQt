#include "protocol.h"
#include <stdlib.h>
#include <cstring>


// 产生一个结构体
PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;
//    PDU *pdu = new PDU[sizeof(PDU)+ uiMsgLen];
    PDU *pdu = (PDU*)malloc(uiPDULen);

    if(pdu == NULL)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,uiPDULen);
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;

    return pdu;
}
