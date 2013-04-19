using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;

namespace PixelStudioApplication.M
{
    [DataContract]
    public class Project
    {        
        [DataMember]
        public string Title
        {
            get { return _title; }
            set { _title = value; }
        }
        private string _title;

        public event PropChangeHandler eventHandler;

        public bool isModify
        {
            get
            {
                return _modify;
            }
        }
        private bool _modify;
        public void modify(bool v)
        {
            _modify = v;
        }

        private String _fileName;
        public String fileName
        {
            get
            {
                return _fileName;
            }
            set
            {
                _fileName = value;
            }
        }

        internal void close()
        {
            if (eventHandler != null)
            {
                eventHandler(this, TYPE.CLOSE, "");
            }
        }
    }
}
