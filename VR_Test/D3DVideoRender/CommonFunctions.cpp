/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#include "stdafx.h"
#include "CommonFunctions.h"

extern "C" void __cdecl memcpyMMX(void *Dest, void *Src, size_t nBytes)
{
    __asm 
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest]
        mov     ecx, nBytes
        shr     ecx, 6                      // nBytes / 64
		cmp     ecx, 0
		je      Myloop 
align 8
CopyLoop:
        movq    mm0, qword ptr[esi]
        movq    mm1, qword ptr[esi+8*1]
        movq    mm2, qword ptr[esi+8*2]
        movq    mm3, qword ptr[esi+8*3]
        movq    mm4, qword ptr[esi+8*4]
        movq    mm5, qword ptr[esi+8*5]
        movq    mm6, qword ptr[esi+8*6]
        movq    mm7, qword ptr[esi+8*7]
        movq    qword ptr[edi], mm0
        movq    qword ptr[edi+8*1], mm1
        movq    qword ptr[edi+8*2], mm2
        movq    qword ptr[edi+8*3], mm3
        movq    qword ptr[edi+8*4], mm4
        movq    qword ptr[edi+8*5], mm5
        movq    qword ptr[edi+8*6], mm6
        movq    qword ptr[edi+8*7], mm7
        add     esi, 64
        add     edi, 64
        loop CopyLoop
//		emms
align 8
Myloop:
        mov     ecx, nBytes
        and     ecx, 63
        cmp     ecx, 0
        je EndCopyLoop
;align 8
;CopyLoop2:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
;        inc esi
;        inc edi
;        dec ecx
;        jne CopyLoop2
		 rep movsb
EndCopyLoop:
    }
}