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
        NetworkSerial connection;
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
                ConnectButton.IsEnabled = true;
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
            }));
        }

        private void OnConnectionLost(string message)
        {
            // disable the buttons on the UI thread!
            var action = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
            {
                changeButtonStatus(false);
                // restore button content to "connect"
                ConnectButton.IsEnabled = true;
                ConnectButton.Content = "Connect";
            }));
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (ConnectButton.Content.Equals("Connect"))
            {
                connection = new NetworkSerial(new Windows.Networking.HostName(RemoteIP.Text), 3030);
                arduino = new RemoteDevice(connection);
                connection.ConnectionEstablished += OnConnectionEstablished;
                connection.ConnectionFailed += OnConnectionFailed;
                connection.ConnectionLost += OnConnectionLost;

                arduino.StringMessageReceived += Arduino_StringMessageReceived;

                connection.begin(115200, SerialConfig.SERIAL_8N1);
                ConnectButton.IsEnabled = false;
            }
            else
            {
                connection.end();


                changeButtonStatus(false);
                // restore button content to "connect"
                ConnectButton.Content = "Connect";
            }
        }

        private void changeButtonStatus(Boolean enabled)
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
        }

        private void Arduino_StringMessageReceived(string message)
        {
            ConnectButton.Content = message;
        }

        private void sendTwo7Bytes(string str)
        {
            byte[] message = new byte[str.Length * 2];
            for (int i = 0; i < str.Length; i ++)
            {
                message[i * 2] = (byte)(str[i] & 0xff);
                message[i * 2 + 1] = 0x00;
            }
            connection.write(0xf0);
            connection.write(0x71);
            connection.write(message);
            connection.write(0xf7);
        }

        private void LeftHand_Click(object sender, RoutedEventArgs e)
        {
            string str = "#hand-left%";
            sendTwo7Bytes(str);
        }

        private void Forward_Click(object sender, RoutedEventArgs e)
        {
            string str = "#forward%";
            sendTwo7Bytes(str);
        }

        private void RightHand_Click(object sender, RoutedEventArgs e)
        {
            string str = "#hand-right%";
            sendTwo7Bytes(str);
        }

        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            string str = "#stop%";
            sendTwo7Bytes(str);
        }

        private void Left_Click(object sender, RoutedEventArgs e)
        {
            string str = "#left%";
            sendTwo7Bytes(str);
        }

        private void Right_Click(object sender, RoutedEventArgs e)
        {
            string str = "#right%";
            sendTwo7Bytes(str);
        }

        private void SitDown_Click(object sender, RoutedEventArgs e)
        {
            string str = "#sit-down%";
            sendTwo7Bytes(str);
        }

        private void Back_Click(object sender, RoutedEventArgs e)
        {
            string str = "#back%";
            sendTwo7Bytes(str);
        }

        private void GoProne_Click(object sender, RoutedEventArgs e)
        {
            string str = "#go-prone%";
            sendTwo7Bytes(str);
        }

        private void HeadUpButton_Click(object sender, RoutedEventArgs e)
        {
            string str = "#head-up%";
            sendTwo7Bytes(str);
        }

        private void HeadLeftButton_Click(object sender, RoutedEventArgs e)
        {
            string str = "#head-left%";
            sendTwo7Bytes(str);
        }

        private void HeadButton_Click(object sender, RoutedEventArgs e)
        {
            string str = "#head-forward%";
            sendTwo7Bytes(str);
        }

        private void HeadRightButton_Click(object sender, RoutedEventArgs e)
        {
            string str = "#head-right%";
            sendTwo7Bytes(str);
        }

        private void HeadDownButton_Click(object sender, RoutedEventArgs e)
        {
            string str = "#head-down%";
            sendTwo7Bytes(str);
        }
    }
}
