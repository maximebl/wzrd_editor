#include "pch.h"
#include "transforms_showcase_page.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
    transforms_showcase_page::transforms_showcase_page()
    {
        InitializeComponent();
    }

	Windows::Foundation::IAsyncAction transforms_showcase_page::onclick_test(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		co_return;
	}

	void transforms_showcase_page::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs e)
	{
		Windows::UI::Xaml::Media::Animation::ConnectedAnimationService::GetForCurrentView().PrepareToAnimate(L"forward", source());
	}

	void transforms_showcase_page::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs e)
	{
		using namespace Windows::UI::Xaml::Media::Animation;

		auto anim = ConnectedAnimationService::GetForCurrentView().GetAnimation(L"backward");

		if (anim)
		{
			anim.TryStart(source());
		}
	}
}
