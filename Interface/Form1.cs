using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;
using SolverLib;

namespace Interface
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		//
		//	Solution creation and managment section
		//

		private void button5_Click(object sender, EventArgs e)
		{
			CircuitModel model = CircuitModel.createDefaultModel();

			PropertyGrid solution = new PropertyGrid();
			solution.SelectedObject = model;
			solution.CommandsVisibleIfAvailable = true;

			Button button = new Button();
			button.Text = "Вычислить";
			button.Click += (obj, args) =>
				{
					(obj as Control).Enabled = false;
					Task task = new Task(() => 
					{
						try
						{
							model.calculate();
							drawSolution(model); 
						}
						catch (Exception error)
						{
							MessageBox.Show(error.Message);
						}
						enableControl(obj); 
					});
					task.Start();
				};

			Button button2 = new Button();
			button2.Text = "Печать";
			button2.Click += (obj, args) =>
				{
					var printform = new PrintForm(model);
					printform.Show();
				};

			Panel panel = new Panel();
			panel.Controls.Add(solution);
			panel.Controls.Add(button);
			panel.Controls.Add(button2);

			button.Dock = DockStyle.Bottom;
			button.Height = 30;
			button2.Dock = DockStyle.Bottom;
			button2.Height = 30;
			solution.Dock = DockStyle.Fill;

			SolutionLayout.Controls.Add(panel);
			panel.Height = Math.Min(440, SolutionLayout.Height - 10);
			panel.Width = Math.Max(200, (SolutionLayout.Width - 10) / SolutionLayout.Controls.Count);
		}

		private void SolutionLayout_Resize(object sender, EventArgs e)
		{
			foreach (var obj in SolutionLayout.Controls)
			{
				(obj as Panel).Height = Math.Min(440, SolutionLayout.Height - 15);
				(obj as Panel).Width = Math.Max(200, (SolutionLayout.Width - 15 * SolutionLayout.Controls.Count) /
					SolutionLayout.Controls.Count);
			}
			SolutionLayout.Invalidate();
		}

		private void SolutionLayout_ControlAdded(object sender, ControlEventArgs e)
		{
			SolutionLayout_Resize(this, null);
		}

		private void button6_Click(object sender, EventArgs e)
		{
			SolutionLayout.Controls.Clear();
			if (Iform != null) Iform.chart.Series.Clear();
			if (Uform != null) Uform.chart.Series.Clear();
			if (Rform != null) Rform.chart.Series.Clear();
			if (Pform != null) Pform.chart.Series.Clear();
		}

		//
		//	Plot managment section
		//

		PlotForm Iform = null;
		PlotForm Uform = null;
		PlotForm Rform = null;
		PlotForm Pform = null;

		private void _FormBtnClick(ref PlotForm form, string formText,
			string x_axis, string y_axis, Action form_closing_dlg, Action<CircuitModel> solution_drawing_dlg)
		{
			if (form == null)
			{
				form = new PlotForm();
				form.Text = formText;
				form.chart.ChartAreas[0].AxisX.Title = x_axis;
				form.chart.ChartAreas[0].AxisY.Title = y_axis;
				form.Show();
				form.FormClosing += (obj, args) => form_closing_dlg();
				iterate_Solutions(solution_drawing_dlg);
			}
			else
			{
				form.Close();
				form = null;
			}
		}

		// Applyes fucn to all active solutions
		private void iterate_Solutions(Action<CircuitModel> func)
		{
			foreach (var panel in SolutionLayout.Controls)
			{
				var model = (((panel as Panel).Controls[0] as PropertyGrid).SelectedObject as CircuitModel);
				func(model);
			}
		}

		private void IFormClick(object sender, EventArgs args)
		{
			_FormBtnClick(ref Iform, "График силы тока", "Время t [c]", "Сила тока I [A]",
				() => { Iform = null; }, drawSolutionI);
		}

		private void UFormClick(object sender, EventArgs args)
		{
			_FormBtnClick(ref Uform, "График напряжения", "Время t [c]", "Напряжение U [В]",
				() => { Uform = null; }, drawSolutionU);
		}

		private void RFormClick(object sender, EventArgs args)
		{
			_FormBtnClick(ref Rform, "График сопротивления лампы", "Время t [c]", "Сопротивление R [Ом]",
				() => { Rform = null; }, drawSolutionR);
		}

		private void PFormClick(object sender, EventArgs args)
		{
			_FormBtnClick(ref Pform, "График давления", "Время t [c]", "Давление p [Па]",
				() => { Pform = null; }, drawSolutionP);
		}

		private delegate void OjectDelegate(object obj);

		private void enableControl(object obj)
		{
			if (this.InvokeRequired)
			{
				OjectDelegate dlg = enableControl;
				this.Invoke(dlg, new object[] { obj });
			}
			else
				(obj as Control).Enabled = true;
		}

		private delegate void ModelDelegate(CircuitModel mdl);

		private void drawSolution(CircuitModel model)
		{
			if (this.InvokeRequired)
			{
				ModelDelegate dlg = drawSolution;
				this.Invoke(dlg, new object[] { model });
			}
			else
			{
				drawSolutionI(model);
				drawSolutionU(model);
				drawSolutionR(model);
				drawSolutionP(model);
			}
		}

		private void drawSolutionI(CircuitModel model)
		{
			if (Iform != null)
			{
				var series = Iform.chart.Series.FindByName(model.solution_name);
				if (series == null)
				{
					series = Iform.chart.Series.Add(model.solution_name);
					series.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
				}
				series.Color = model.color;
				series.Points.DataBindXY(model.Time, model.I);
			}
		}

		private void drawSolutionU(CircuitModel model)
		{
			if (Uform != null)
			{
				var series = Uform.chart.Series.FindByName(model.solution_name);
				if (series == null)
				{
					series = Uform.chart.Series.Add(model.solution_name);
					series.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
				}
				series.Color = model.color;
				series.Points.DataBindXY(model.Time, model.U);
			}
		}

		private void drawSolutionR(CircuitModel model)
		{
			if (Rform != null)
			{
				var series = Rform.chart.Series.FindByName(model.solution_name);
				if (series == null)
				{
					series = Rform.chart.Series.Add(model.solution_name);
					series.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
				}
				series.Points.DataBindXY(model.Time2, model.R);
				series.Color = model.color;
			}
		}

		private void drawSolutionP(CircuitModel model)
		{
			if (Pform != null)
			{
				var series = Pform.chart.Series.FindByName(model.solution_name);
				if (series == null)
				{
					series = Pform.chart.Series.Add(model.solution_name);
					series.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
				}
				series.Points.DataBindXY(model.Time2, model.P);
				series.Color = model.color;
			}
		}

		private void button7_Click(object sender, EventArgs e)
		{
			var testform = new TemperatureFieldForm();
			testform.Show();
		}

		private void button8_Click(object sender, EventArgs e)
		{
			var testform = new PressurePlotForm();
			testform.Show();
		}

		private void button9_Click(object sender, EventArgs e)
		{
			var testform = new ResistancePlotForm();
			testform.Show();
		}

		private void button10_Click(object sender, EventArgs e)
		{
			var testform = new XenonPlotForm();
			testform.Show();
		}

		private void button11_Click(object sender, EventArgs e)
		{
			var testform = new LightFluxForm();
			testform.Show();
		}
	}
}
