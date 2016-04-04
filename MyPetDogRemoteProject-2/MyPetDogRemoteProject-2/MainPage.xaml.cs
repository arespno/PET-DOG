using Microsoft.Maker.Firmata;
using Microsoft.Maker.RemoteWiring;
using Microsoft.Maker.Serial;
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace MyPetDogRemoteProject_2
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        IStream connection;
        UwpFirmata firmata;
        RemoteDevice arduino;

        public MainPage()
        {
            this.InitializeComponent();

            RemoteIP.Text = "192.168.1.100";
        }

        private void OnConnectionFailed(string message)
        {
            // enable the buttons on the UI thread!
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
            {
                freeResources();

                ConnectButton.IsEnabled = true;
                SendMessage.Text = message;
            }));
        }

        private void OnConnectionEstablished()
        {
            // enable the buttons on the UI thread!
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(()=>
            {
                changeButtonStatus(true);
                // change button content to "disconnect"
                ConnectButton.IsEnabled = true;
                ConnectButton.Content = "Disconnect";
                ReceiveMessage.Items.Clear();
            }));
        }

        private void OnConnectionLost(string message)
        {
            // disable the buttons on the UI thread!
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
            {
                freeResources();

                changeButtonStatus(false);
                // restore button content to "connect"
                ConnectButton.IsEnabled = true;
                ConnectButton.Content = "Connect";
                SendMessage.Text = message;
            }));
        }

        private void freeResources()
        {
            arduino.Dispose();
            firmata.finish();
            firmata.Dispose();
            connection.end();
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (ConnectButton.Content.Equals("Connect"))
            {
                connection = new NetworkSerial(new Windows.Networking.HostName(RemoteIP.Text), 3030);
                firmata = new UwpFirmata();
                arduino = new RemoteDevice(firmata);
                firmata.begin(connection);
                connection.ConnectionEstablished += OnConnectionEstablished;
                connection.ConnectionFailed += OnConnectionFailed;
                connection.ConnectionLost += OnConnectionLost;
                connection.begin(115200, SerialConfig.SERIAL_8N1);
                firmata.StringMessageReceived += OnStringMessageReceived;
                ConnectButton.IsEnabled = false;
            }
            else
            {
                freeResources();

                var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
                {
                    changeButtonStatus(false);
                    // restore button content to "connect"
                    ConnectButton.Content = "Connect";
                }));
            }
        }

        private void OnStringMessageReceived(UwpFirmata caller, StringCallbackEventArgs argv)
        {
            // change the textbox value on the UI thread!
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
            {
                ReceiveMessage.Items.Add(argv.getString());
                ReceiveMessage.ScrollIntoView(ReceiveMessage.Items.Last());
                ReceiveMessage.UpdateLayout();
            }));
        }

        private void changeButtonStatus(Boolean enabled)
        {
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
            {
                LeftHandButton.IsEnabled = enabled;
                ForwardButton.IsEnabled = enabled;
                RightHandButton.IsEnabled = enabled;
                LeftButton.IsEnabled = enabled;
                StopButton.IsEnabled = enabled;
                RightButton.IsEnabled = enabled;
                SitDownButton.IsEnabled = enabled;
                BackButton.IsEnabled = enabled;
                GoProneButton.IsEnabled = enabled;
                HeadUpButton.IsEnabled = enabled;
                HeadLeftButton.IsEnabled = enabled;
                HeadButton.IsEnabled = enabled;
                HeadRightButton.IsEnabled = enabled;
                HeadDownButton.IsEnabled = enabled;
                HeadShakeButton.IsEnabled = enabled;
                HeadNodButton.IsEnabled = enabled;
                SendButton.IsEnabled = enabled;
            }));
        }

        private void LeftHand_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#hand-left%");
        }

        private void Forward_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#forward%");
        }

        private void RightHand_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#hand-right%");
        }

        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#stop%");
        }

        private void Left_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#left%");
        }

        private void Right_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#right%");
        }

        private void SitDown_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#sit-down%");
        }

        private void Back_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#back%");
        }

        private void GoProne_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#go-prone%");
        }

        private void HeadUpButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-up%");
        }

        private void HeadLeftButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-left%");
        }

        private void HeadButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-forward%");
        }

        private void HeadRightButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-right%");
        }

        private void HeadDownButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-down%");
        }

        private void HeadShakeButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-shake%");
        }

        private void HeadNodButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString("#head-nod%");
        }

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            firmata.sendString(SendMessage.Text);
        }
    }
}
