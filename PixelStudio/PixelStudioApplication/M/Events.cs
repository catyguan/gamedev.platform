using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PixelStudioApplication.M
{
    public delegate void PropChangeHandler(object sender, String type, String propName);

    public delegate void CollectionChangeHandler(object sender, String type, String propName);

    public class TYPE
    {
        public const String ADD = "add";
        public const String CHANGE = "change";
        public const String REMOVE = "remove";

        public const String NEW = "new";
        public const String CLOSE = "close";
    }
}
