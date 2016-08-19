namespace Interface
{
	partial class PlotForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
			this.chart = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.x_label = new System.Windows.Forms.Label();
			this.y_label = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.chart)).BeginInit();
			this.SuspendLayout();
			// 
			// chart
			// 
			this.chart.BackGradientStyle = System.Windows.Forms.DataVisualization.Charting.GradientStyle.DiagonalRight;
			this.chart.BackSecondaryColor = System.Drawing.Color.Silver;
			chartArea1.AxisX.IntervalAutoMode = System.Windows.Forms.DataVisualization.Charting.IntervalAutoMode.VariableCount;
			chartArea1.AxisX.IsMarginVisible = false;
			chartArea1.AxisX.LabelStyle.Format = "G8";
			chartArea1.AxisX.MajorGrid.Interval = 0D;
			chartArea1.AxisX.MajorGrid.IntervalOffset = 0D;
			chartArea1.AxisX.MajorGrid.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisX.MajorGrid.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisX.MajorGrid.LineColor = System.Drawing.Color.Silver;
			chartArea1.AxisX.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dot;
			chartArea1.AxisX.ScrollBar.BackColor = System.Drawing.Color.Silver;
			chartArea1.AxisX.ScrollBar.ButtonColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			chartArea1.AxisY.IntervalAutoMode = System.Windows.Forms.DataVisualization.Charting.IntervalAutoMode.VariableCount;
			chartArea1.AxisY.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisY.IsStartedFromZero = false;
			chartArea1.AxisY.LabelStyle.Format = "G8";
			chartArea1.AxisY.MajorGrid.Interval = 0D;
			chartArea1.AxisY.MajorGrid.IntervalOffset = 0D;
			chartArea1.AxisY.MajorGrid.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisY.MajorGrid.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisY.MajorGrid.LineColor = System.Drawing.Color.Silver;
			chartArea1.AxisY.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dot;
			chartArea1.AxisY.MinorGrid.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisY.MinorGrid.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
			chartArea1.AxisY.ScrollBar.BackColor = System.Drawing.Color.Silver;
			chartArea1.AxisY.ScrollBar.ButtonColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			chartArea1.BackColor = System.Drawing.Color.Transparent;
			chartArea1.CursorX.Interval = 0D;
			chartArea1.CursorX.IsUserEnabled = true;
			chartArea1.CursorX.IsUserSelectionEnabled = true;
			chartArea1.CursorX.SelectionColor = System.Drawing.Color.MistyRose;
			chartArea1.CursorY.Interval = 0D;
			chartArea1.CursorY.IsUserEnabled = true;
			chartArea1.CursorY.IsUserSelectionEnabled = true;
			chartArea1.CursorY.SelectionColor = System.Drawing.Color.MistyRose;
			chartArea1.Name = "ChartArea1";
			this.chart.ChartAreas.Add(chartArea1);
			this.chart.Dock = System.Windows.Forms.DockStyle.Fill;
			legend1.BackColor = System.Drawing.Color.Transparent;
			legend1.DockedToChartArea = "ChartArea1";
			legend1.Name = "Legend1";
			this.chart.Legends.Add(legend1);
			this.chart.Location = new System.Drawing.Point(0, 0);
			this.chart.Name = "chart";
			this.chart.Size = new System.Drawing.Size(592, 408);
			this.chart.TabIndex = 0;
			this.chart.Text = "Холст";
			this.chart.TextAntiAliasingQuality = System.Windows.Forms.DataVisualization.Charting.TextAntiAliasingQuality.SystemDefault;
			this.chart.CursorPositionChanged += new System.EventHandler<System.Windows.Forms.DataVisualization.Charting.CursorEventArgs>(this.chart_CursorPositionChanged);
			this.chart.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.chart_KeyPress);
			// 
			// x_label
			// 
			this.x_label.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.x_label.BackColor = System.Drawing.Color.Transparent;
			this.x_label.Location = new System.Drawing.Point(0, 391);
			this.x_label.Name = "x_label";
			this.x_label.Size = new System.Drawing.Size(210, 17);
			this.x_label.TabIndex = 1;
			this.x_label.Text = "X = ";
			this.x_label.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// y_label
			// 
			this.y_label.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.y_label.BackColor = System.Drawing.Color.Transparent;
			this.y_label.Location = new System.Drawing.Point(216, 391);
			this.y_label.Name = "y_label";
			this.y_label.Size = new System.Drawing.Size(213, 17);
			this.y_label.TabIndex = 2;
			this.y_label.Text = "Y = ";
			this.y_label.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// PlotForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(592, 408);
			this.Controls.Add(this.y_label);
			this.Controls.Add(this.x_label);
			this.Controls.Add(this.chart);
			this.MinimumSize = new System.Drawing.Size(450, 300);
			this.Name = "PlotForm";
			this.Text = "График";
			((System.ComponentModel.ISupportInitialize)(this.chart)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		public System.Windows.Forms.DataVisualization.Charting.Chart chart;
		private System.Windows.Forms.Label x_label;
		private System.Windows.Forms.Label y_label;
	}
}