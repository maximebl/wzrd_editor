//
// pch.h
// Header for platform projection include files
//

#pragma once

#include <unknwn.h>
#include "winrt/Windows.Foundation.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXColors.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include "../wzrd_editor/MathHelper.h"
#include "../os_utilities/os_utilities.h"
#include <ppltasks.h>
// pplawait.h allows the use of co_await on concurrency::task types
#include <pplawait.h>