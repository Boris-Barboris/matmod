using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Interface
{
	public partial class PlotForm : Form
	{
		public PlotForm()
		{
			InitializeComponent();
		}

		private void chart_CursorPositionChanged(object sender, System.Windows.Forms.DataVisualization.Charting.CursorEventArgs e)
		{
			switch (e.Axis.AxisName)
			{
				case System.Windows.Forms.DataVisualization.Charting.AxisName.X:
					x_label.Text = "X = " + e.NewPosition.ToString("G");
					break;
				case System.Windows.Forms.DataVisualization.Charting.AxisName.Y:
					y_label.Text = "Y = " + e.NewPosition.ToString("G");
					break;
			}
		}

		private void chart_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar == (char)Keys.Escape)
			{
				chart.ChartAreas[0].AxisX.ScaleView.ZoomReset();
				chart.ChartAreas[0].AxisY.ScaleView.ZoomReset();
			}
		}
	}
}
