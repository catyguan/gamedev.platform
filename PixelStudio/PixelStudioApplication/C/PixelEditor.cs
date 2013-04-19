using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using PixelStudioApplication.M;

namespace PixelStudioApplication.C
{
    public partial class PixelEditor : UserControl
    {
        public PixelEditor()
        {
            InitializeComponent();
        }

        public Size pixelSize
        {
            get
            {
                return _pixelSize;
            }
            set
            {
                _pixelSize = value;
                PixelInfo[][] old = _pixels;
                _pixels = new PixelInfo[_pixelSize.Width][];
                for(int x=0;x<_pixelSize.Width;x++)
                {
                    _pixels[x] = new PixelInfo[_pixelSize.Height];
                }
                if (old != null)
                {
                    for (int x = 0; x < old.Length; x++)
                    {
                        if (x < _pixels.Length)
                        {
                            PixelInfo[] src = old[x];
                            PixelInfo[] desc = _pixels[x];
                            for (int y = 0; y < src.Length; y++)
                            {
                                if (y < desc.Length)
                                {
                                    desc[y] = src[y];
                                }
                            }
                        }
                    }
                }
                _surface = null;
                Invalidate();
            }
        }
        private Size _pixelSize;
        private PixelInfo[][] _pixels;

        public int cellSize
        {
            get
            { 
                return _cellSize; 
            }
            set
            {
                _cellSize = value;
                _cellSize -= _cellSize % 4;
                _surface = null;
                Invalidate();
            }
        }
        private int _cellSize;

        public Color penColor
        {
            get
            {
                return _penColor;
            }
            set
            {
                _penColor = value;
            }
        }
        private Color _penColor;

        public bool isTransparent
        {
            get
            {
                return _transparent;
            }
            set
            {
                _transparent = value;
            }
        }
        private bool _transparent;

        private void PixelEditor_Paint(object sender, PaintEventArgs e)
        {
            if (_surface == null)
            {
                doDraw();
            }
            e.Graphics.DrawImage(_surface, 0, 0);
        }

        private Bitmap _surface;
        private void doDraw()
        {
            if (_surface == null)
            {
                _surface = new Bitmap(_pixelSize.Width * _cellSize, _pixelSize.Height * _cellSize);
            }
            Graphics g = Graphics.FromImage(_surface);

            g.FillRectangle(Brushes.White, ClientRectangle);
            for (int y = 0; y < _pixelSize.Height; y++)
            {
                for (int x = 0; x < _pixelSize.Width; x++)
                {
                    drawCell(g, x, y);                    
                }
            }
            drawGrid(g);
            
            g.DrawString("Hello, world", Font, Brushes.Yellow, 0, 0);
        }

        private void drawGrid(Graphics g)
        {
            Pen linePen = new Pen(Color.FromArgb(130, 128, 120));
            int width = _pixelSize.Width * _cellSize;
            int heigth = _pixelSize.Height * _cellSize;
            for (int y = 0; y <= _pixelSize.Height; y++)
            {
                g.DrawLine(linePen, new Point(0, y * _cellSize), new Point(width, y * _cellSize));
            }
            for (int x = 0; x <= _pixelSize.Width; x++)
            {
                g.DrawLine(linePen, new Point(x * _cellSize, 0), new Point(x * _cellSize, heigth));
            }
        }

        private void drawCell(Graphics g, int x, int y)
        {
            int offX = x*_cellSize;
            int offY = y*_cellSize;
            int ts = _cellSize / 4;

            PixelInfo info = _pixels[x][y];

            if (info == null)
            {
                Brush grayBrush = new SolidBrush(Color.FromArgb(208, 205, 199));

                for (int ty = 0; ty < 4; ty++)
                {
                    for (int tx = 0; tx < 4; tx++)
                    {
                        Rectangle rect = new Rectangle(offX + tx * ts, offY + ty * ts, ts, ts);
                        g.FillRectangle((tx + ty) % 2 == 0 ? Brushes.White : grayBrush, rect);
                    }
                }
            }
            else
            {
                Rectangle rect = new Rectangle(offX, offY , _cellSize, _cellSize);
                Brush brush = new SolidBrush(info.color);
                g.FillRectangle(brush, rect);
            }
        }

        private void PixelEditor_MouseClick(object sender, MouseEventArgs e)
        {
            int x = e.X / _cellSize;
            int y = e.Y / _cellSize;

            if (!(x >= 0 && x < _pixelSize.Width && y >= 0 && y < _pixelSize.Height)) return;

            if (_transparent)
            {
                _pixels[x][y] = null;
            }
            else
            {
                PixelInfo p = _pixels[x][y];
                if(p==null)
                {
                    p = new PixelInfo();
                    _pixels[x][y] = p;
                }
                p.color = _penColor;
            }
            doDraw();
            Invalidate(new Rectangle(x*_cellSize,y*_cellSize,_cellSize,_cellSize));
        }       

        
    }
}
