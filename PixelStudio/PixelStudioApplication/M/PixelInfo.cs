using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace PixelStudioApplication.M
{
    [DataContract]
    class PixelInfo
    {
        [DataMember]
        public Color color
        {
            get
            {
                return _color;
            }
            set
            {
                _color = value;
            }
        }
        private Color _color;
    }
}
