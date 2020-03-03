/* *********************************************************************** *
 * File   : mplex.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: The host that connects to the server and lets people connect.  *
 *                                                                         *
 * Bugs   : -                                                              *
 *                                                                         *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

#ifndef _MUD_MPLEX_H
#define _MUD_MPLEX_H

#include "network.h"
#include "protocol.h"
#include "essential.h"
#include "queue.h"
#include "select.h"

class cConHook : public cHook
{
  public:
   cConHook(void);
   ~cConHook(void);

   void Close(int bNotifyMud);
   char AddInputChar(ubit8 c);
   void AddString(char *str);
   void ParseInput(void);
   void SendCon(const char *str);
   void WriteCon(const char *str);
   char *IndentText(const char *source, char *dest, int dest_size, int width);
   const char *ParseOutput(const char *text);
   void PromptErase(void);
   void PromptRedraw(const char *prompt);
   void TransmitCommand(const char *text);
   void ShowChunk(void);
   void ProcessPaged(void);
   void PressReturn(const char *cmd);
   void PlayLoop(const char *cmd);
   void MudDown(const char *cmd);
   void MenuSelect(const char *cmd);
   void SequenceCompare(ubit8 *pBuf, int *pnLen);

   void Input(int nFlags);
   void getLine(ubit8 buf[], int *size);
   void testChar(ubit8 c);

   ubit16 m_nId;
   int    m_nFirst;
   ubit8  m_nLine;
   int    m_nPromptMode;  /* 0 none, 1 press return */
   int    m_nPromptLen;   /* Prompt length          */
   int    m_nSequenceCompare;

   int    m_nState;
   int    m_nEscapeCode;  /* Very simplified state diagram assistance */
   char   m_aOutput[4096];
   char   m_aInputBuf[2*MAX_INPUT_LENGTH];
   char   m_aHost[50];
   void (*m_pFptr) (class cConHook *, const char *cmd);

   cConHook *m_pNext;

   struct terminal_setup_type m_sSetup;

   ubit8 m_nBgColor;       /* Stupid bitching ANSI   */

   cQueue m_qInput;         /* Input from user        */
   cQueue m_qPaged;         /* Paged text output      */
};

#endif
