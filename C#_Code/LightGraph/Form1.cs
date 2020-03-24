using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using OxyPlot;
using OxyPlot.WindowsForms;
using OxyPlot.Series;
using OxyPlot.Axes;


namespace LightGraph
{
    public partial class Form1 : Form
    {
        private DBManager dbManager;
        private PlotView pv;
        public Form1()
        {
            pv = new PlotView();
            dbManager = new DBManager();
            InitializeComponent();
        }

        private FunctionSeries FillFunctionSeries(DateTime StartDate, DateTime EndDate)
        {
            FunctionSeries fs = new FunctionSeries();
            dbManager.Connect().ToString();
            var Start = StartDate.ToString("yyyy-MM-dd HH:mm:ss");
            var End = EndDate.ToString("yyyy-MM-dd HH:mm:ss");
            var query = "SELECT time_stamp, lux FROM payload WHERE time_stamp BETWEEN \"" + Start + "\" AND \"" + End + "\"";
            var ResultSet = dbManager.ExecuteQuery(query);
            if (ResultSet.Count > 0)
            {
                foreach (var element in ResultSet)
                {
                    var words = element.Split('\t');
                    var time_stamp = DateTime.Parse(words[0]);
                    var lux = double.Parse(words[1]);
                    fs.Points.Add(new DataPoint(DateTimeAxis.ToDouble(time_stamp), lux));
                }
            }
            dbManager.CloseConn();
            return fs;
        }

        private void InitPlotView()
        {
            pv.Location = new Point(0, 50);
            pv.Size = new Size(500, 200);
            this.Controls.Add(pv);
            pv.Model = new PlotModel { Title = "Light Level Plot" };

            pv.Model.Axes.Add(new DateTimeAxis
            {
                Position = AxisPosition.Bottom,
                StringFormat = "M/d",
                Title = "Date"
            });

            pv.Model.Axes.Add(new LinearAxis
            {
                Position = AxisPosition.Left,
                Minimum = DateTimeAxis.ToDouble(0),
                Title = "Lux"
            });
        }

        private void PlotData()
        {
            var StartDate = DateTime.Now.AddDays(-2);
            var EndDate = DateTime.Now;

            FunctionSeries fs = FillFunctionSeries(StartDate, EndDate);
            pv.Model.Series.Add(fs);
            SetLatestReading(fs);
        }


        private void SetLatestReading(FunctionSeries fs)
        {
            var LastElement = fs.Points.ElementAt(fs.Points.Count - 1);
            var LastDateTime = DateTimeAxis.ToDateTime(LastElement.X);
            var LastLux = LastElement.Y;
            label2.Text = (LastLux + " Lux at " + LastDateTime.ToString("HH:mm dd/MM/yyyy"));
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            InitPlotView();
            PlotData();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Controls.Remove(pv);
            InitPlotView();
            this.Controls.Add(pv);
            PlotData();
        }
    }
}
