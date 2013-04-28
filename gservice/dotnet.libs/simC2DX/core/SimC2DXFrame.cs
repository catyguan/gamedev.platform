using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simC2DX.core
{
    public interface SimScene
    {
        void onEnter();
        void onExit();
    }

    public partial class SimC2DXFrame : Form
    {
        public SimC2DXFrame()
        {
            InitializeComponent();
        }

        public void runScene(Control scene)
        {
            
        }

        public Control currentScene
        {
            get
            {
                return null;
            }
        }

        public void pushScene(Control scene)
        {
        }

        public Control popScene()
        {
            return null;
        }

    }
}
