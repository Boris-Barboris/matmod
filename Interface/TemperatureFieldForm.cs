﻿using System;
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
	public partial class TemperatureFieldForm : Form
	{
		public TemperatureFieldForm()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e)
		{
			var points = SolverLib.CircuitModel.plotTempareture(Convert.ToDouble(textBox1.Text), Convert.ToDouble(textBox2.Text));
			chart.Series[0].Points.DataBindXY(points.Item1, points.Item2);
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
