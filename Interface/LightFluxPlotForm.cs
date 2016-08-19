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
	public partial class LightFluxForm: Form
	{
		public LightFluxForm()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e)
		{
			button1.Enabled = false;
			Int32 divisions = Convert.ToInt32(textBox1.Text);
			Double pressure = Convert.ToDouble(textBox3.Text);
			Double current = Convert.ToDouble(textBox4.Text);
			Double radius = Convert.ToDouble(textBox2.Text);
			//var points = SolverLib.CircuitModel.plotFlux(divisions, pressure, current, radius, SolverLib.BVP_algorithm.DiffApprox);
			//chart.Series[0].Points.DataBindXY(points.Item1, points.Item2);
			var points = SolverLib.CircuitModel.plotFlux(divisions, pressure, current, radius, SolverLib.BVP_algorithm.Balance);
			chart.Series[0].Points.DataBindXY(points.Item1, points.Item2);
			chart.Series[0].LegendText = "Метод баланса";
			button1.Enabled = true;
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
