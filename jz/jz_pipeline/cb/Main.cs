// #define CLIENT_USAGE

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace jz.cb
{
    public static class Program
    {
        public const string kInArg = "-in";
        public const string kLogFile = "jz_cb.log";
        public const string kDefaultInPath = "..\\..\\media";

        #region Private members
        [DllImport("user32.dll")]
        private static extern int SetForegroundWindow(IntPtr hWnd);
            
        private static void Go(string aInDir)
        {
            Process cur = Process.GetCurrentProcess();
            IntPtr curHandle = cur.MainWindowHandle;

            Process[] p = Process.GetProcessesByName(cur.ProcessName);
            if (p.Length <= 1)
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new MainForm(aInDir));
            }
            else
            {
                for (int i = 0; i < p.Length; i++)
                {
                    if (p[i].MainWindowHandle != curHandle)
                    {
                        SetForegroundWindow(p[i].MainWindowHandle);
                        break;
                    }
                }
            }
        }
        #endregion

        [STAThread]
        public static void Main(string[] aArgs)
        {
#if !DEBUG || CLIENT_USAGE
            try
            {
#endif

            string inDir = kDefaultInPath;

            int count = (aArgs.Length - 1);
            for (int i = 0; i < count; i++)
            {
                if (aArgs[i].ToLower().Trim() == kInArg)
                {
                    inDir = aArgs[i + 1];
                    break;
                }
            }

            Go(inDir);
            #if !DEBUG || CLIENT_USAGE
            }
            catch (Exception e)
            {
                string caption = "Exception: Please send \"" + kLogFile + "\" to the appropriate parties.";
                string msg = "Exception: \"" + e.Message + "\"" + System.Environment.NewLine;
                msg += "Source: \"" + e.Source + "\"" + System.Environment.NewLine;
                msg += "Target site: \"" + e.TargetSite + "\"" + System.Environment.NewLine;
                msg += "Stack trace:" + System.Environment.NewLine + e.StackTrace;

                using (System.IO.StreamWriter errorWriter = new StreamWriter(kLogFile))
                {
                    errorWriter.Write(msg);
                }

                System.Windows.Forms.MessageBox.Show(msg, caption, 
                    System.Windows.Forms.MessageBoxButtons.OK, 
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
#endif
            }
    }
}
