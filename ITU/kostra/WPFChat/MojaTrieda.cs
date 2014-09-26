using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Threading;
using System.Collections.ObjectModel;

namespace WPFChat
{
    class MojaTrieda
    {
        public ObservableCollection<ChatEntry> Entries { get; private set; }
        public MojaTrieda()
        {
            Entries = new ObservableCollection<ChatEntry>();
        }
    }
}
