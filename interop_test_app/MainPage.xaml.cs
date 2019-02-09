using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using graphics;
using System.Threading.Tasks;

namespace interop_test_app
{
    public sealed partial class MainPage : Page
    {
        //public List<Customer> tester;
        public MainPage()
        {
            this.InitializeComponent();
            test_abi_calls();
        }

        async Task test_abi_calls()
        {
            //tester = Customer.Customers();
            graphics.renderer renderer = new graphics.renderer();
            renderer.enable_debug_layer();
            renderer.initialize_textures_showcase(swapChainPanel);
            renderer.start_render_loop();

            //graphics.shader new_shader = new graphics.shader("default_vs", shader_type.vertex);
            //new_shader.is_loading = true;
            //var result = await renderer.pick_and_compile_shader(new_shader.shader_name, "VS", "vs_5_0");

            //graphics.buffer new_buffer = new graphics.buffer();
        }
    }

    public class Customer
    {
        public String FirstName { get; set; }
        public String LastName { get; set; }
        public String Address { get; set; }
        public Boolean IsNew { get; set; }

        public Customer(String firstName, String lastName,
            String address, Boolean isNew)
        {
            this.FirstName = firstName;
            this.LastName = lastName;
            this.Address = address;
            this.IsNew = isNew;
        }

        public static List<Customer> Customers()
        {
            return new List<Customer>(new Customer[4] {
            new Customer("A.", "Zero",
                "12 North Third Street, Apartment 45",
                false),
            new Customer("B.", "One",
                "34 West Fifth Street, Apartment 67",
                false),
            new Customer("C.", "Two",
                "56 East Seventh Street, Apartment 89",
                true),
            new Customer("D.", "Three",
                "78 South Ninth Street, Apartment 10",
                true)
        });
        }
    }
}
