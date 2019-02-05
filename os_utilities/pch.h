#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <vector>
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Streams.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include <winrt/Windows.Foundation.h>
#include <ppltasks.h>
// pplawait.h allows the use of co_await on concurrency::task types
#include <pplawait.h>