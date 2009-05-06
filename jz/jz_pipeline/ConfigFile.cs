using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace jz
{

    public class ConfigFile
    {
        #region Private members
        private Dictionary<string, string> mTable = new Dictionary<string,string>();

        private void _Load(string aFilename)
        {
            mTable.Clear();
            StreamReader reader = new StreamReader(aFilename);
            try
            {
                _Parse(reader);
            }
            catch
            {
                reader.Close();
                throw;
            }
        }

        private void _Save(string aFilename)
        {
            StreamWriter writer = new StreamWriter(aFilename);
            try
            {
                foreach (KeyValuePair<string, string> e in mTable)
                {
                    writer.WriteLine(e.Key + "=" + e.Value);
                }
            }
            catch
            {
                writer.Close();
                throw;
            }
        }

        private void _Parse(StreamReader aReader)
        {
            while (!aReader.EndOfStream)
            {
                string key = string.Empty;
                string value = string.Empty;

                string line = aReader.ReadLine();
                int length = line.Length;
                int i = 0;

                for (; i < length; i++)
                {
                    if (char.IsWhiteSpace(line[i]) || line[i] == '=')
                    {
                        key = line.Substring(0, i);
                        key = key.Trim();
                        key = key.ToLower();
                        break;
                    }
                }

                while (i < length && char.IsWhiteSpace(line[i])) i++;
                if (i >= length || line[i] != '=') { continue; }

                if (i + 1 < length)
                {
                    value = line.Substring(i + 1, length - (i + 1));
                    value = value.Trim();
                }

                if (key != string.Empty)
                {
                    mTable.Add(key, value);
                }
            }
        }
        #endregion

        public ConfigFile(string aFilename)
        {
            _Load(aFilename);
        }

        public void Load(string aFilename) { _Load(aFilename); }
        public void Save(string aFilename) { _Save(aFilename); }

        public bool Exists(string key)
        {
            bool bReturn = (mTable.ContainsKey(key.ToLower()));
            return bReturn;
        }

        public bool GetBoolean(string key)
        {
            bool ret = XmlConvert.ToBoolean(mTable[key.ToLower()]);
            return ret;
        }

        public float GetSingle(string key)
        {
            float ret = XmlConvert.ToSingle(mTable[key.ToLower()]);
            return ret;
        }

        public string GetString(string key)
        {
            string ret = (mTable[key.ToLower()]);
            return ret;
        }
    }

}
