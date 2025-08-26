// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "nya-common/nya_commonhooklib.h"
using namespace NyaHookLib;

#include <d3d9.h>
#include "../packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/include/d3dx9.h"
#include "../packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/include/d3dx9effect.h"
#include "../packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/include/D3dx9tex.h"

//#define GetDepthBuffer
#define device (*(LPDIRECT3DDEVICE9*)0x008DA788)
#define hWnd (*(HWND*)0x008DA79C)
#define RELEASE(x) if (x) x->Release()

LPDIRECT3DTEXTURE9 backBuffer = NULL;
LPDIRECT3DTEXTURE9 depthBuffer = NULL;

static void CreateTextureFromSurface(IDirect3DSurface9* surface, LPDIRECT3DTEXTURE9* out_texture)
{
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);

    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);

    device->CreateTexture(windowRect.right, windowRect.bottom, 1, desc.Usage, desc.Format, desc.Pool, out_texture, NULL);
}

static void CopySurfaceToTexture(IDirect3DSurface9* src, LPDIRECT3DTEXTURE9 dst)
{
    IDirect3DSurface9* dstSurface;
    if (SUCCEEDED(dst->GetSurfaceLevel(0, &dstSurface)))
    {
        device->StretchRect(src, NULL, dstSurface, NULL, D3DTEXF_NONE);
        dstSurface->Release();
    }
}

static bool TextureExists(LPD3DXEFFECT effect, char* handle)
{
    LPDIRECT3DBASETEXTURE9 t;
    return SUCCEEDED(effect->GetTexture(handle, &t));
}

static void CreateTextures()
{
    if (!backBuffer)
    {
        IDirect3DSurface9* srcSurface;
        if (SUCCEEDED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_LEFT, &srcSurface)))
        {
            CreateTextureFromSurface(srcSurface, &backBuffer);
            srcSurface->Release();
        }
    }
}

static void UpdateTextures(LPD3DXEFFECT effect)
{
    char tempHandle[] = "Tex0";

    while (TextureExists(effect, tempHandle))
        // A bit strange but 0-9 are in sequence in ANSI so this works
        tempHandle[3]++;

    tempHandle[3]--;
    effect->SetTexture(tempHandle, backBuffer);
}

static void __stdcall PerFrame()
{    
    IDirect3DSurface9* srcSurface;
    if (SUCCEEDED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_LEFT, &srcSurface)))
    {
        CopySurfaceToTexture(srcSurface, backBuffer);
        srcSurface->Release();
    }
}

auto RenderSky = (void* (__stdcall *)(void*, int))0x00592470;

static void* __stdcall NewRenderSky(void* pEnvironment, int param_2)
{
    __asm {
        PUSH    EAX
        CALL    PerFrame
        POP     EAX

        // The stack needs to be restored manually before the jump
        MOV     ESP, EBP
        POP     EBP

        JMP      RenderSky
    }
}


const char* toReplace[] = {
    "data/shader/pro_car_body.sha"
};

const auto Shader_Shader = (void* (__stdcall*)(char*, int))0x005ACBD0;

static void* __stdcall NewShader_Shader(char* filename, int param_2)
{
    LPD3DXEFFECT effect_EAX;
    void* shader_ESI;

    __asm {
        MOV     effect_EAX, EAX
        MOV     shader_ESI, ESI
    }

    for (auto name : toReplace)
    {
        if (!strcmp(filename, name))
        {
            MessageBoxA(NULL, filename, name, MB_ICONINFORMATION);
            CreateTextures();
            UpdateTextures(effect_EAX);
        }
    }

    __asm {
        MOV     EAX, effect_EAX
        MOV     ESI, shader_ESI

        MOV     ESP, EBP
        POP     EBP

        JMP     Shader_Shader
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        PatchRelative(CALL, 0x004CA0F8, NewRenderSky);
        PatchRelative(CALL, 0x005AC547, NewShader_Shader);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        RELEASE(backBuffer);
        RELEASE(depthBuffer);
        break;
    }
    return TRUE;
}

