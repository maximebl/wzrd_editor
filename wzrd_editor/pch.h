//
// pch.h
// Header for platform projection include files
//

#pragma once

#define NOMINMAX
#include <unknwn.h>
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.ApplicationModel.Activation.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "winrt/Windows.UI.Xaml.Data.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Navigation.h"
#include "winrt/Windows.UI.Xaml.Hosting.h"
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>

#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.Popups.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include "d3dx12.h"
#include "winrt/Windows.System.Threading.h"
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Streams.h"
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <d3dcompiler.h>

#include <ppltasks.h>
#include <experimental/resumable>
// pplawait.h allows the use of co_await on concurrency::task types
#include <pplawait.h>

#include "GeometryGenerator.h"
#include <cmath>
#include "GameTimer.h"
#include <DirectXColors.h>

#include <sstream>
