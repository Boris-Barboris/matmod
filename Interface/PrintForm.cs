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
	public partial class PrintForm : Form
	{
		public PrintForm(SolverLib.CircuitModel model)
		{
			InitializeComponent();

			string I = "t [с],		I [A],			U [B]\r\n";
			var ti = model.Time.GetEnumerator();
			var ii = model.I.GetEnumerator();
			var ui = model.U.GetEnumerator();

			while (ti.MoveNext())
			{
				ii.MoveNext();
				ui.MoveNext();
				I += ti.Current.ToString() + '\t' + ii.Current.ToString() + '\t' + ui.Current.ToString() + "\r\n";
			}

			textBox1.Text = I;
		}
	}
}
