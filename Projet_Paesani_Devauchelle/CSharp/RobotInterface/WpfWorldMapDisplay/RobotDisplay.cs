﻿using Constants;
using EventArgsLibrary;
using PlayBook_NS;
using SciChart.Charting.Model.DataSeries;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using Utilities;

namespace WpfWorldMapDisplay
{
    public class RobotDisplay
    {
        private PolygonExtended robotShape;
        private PolygonExtended ghostShape;
        private Random rand = new Random();

        private Location robotLocation;
        private Location ghostLocation;
        private Location destinationLocation;
        private Location waypointLocation;
        public string robotName = "";
        public PlayingAction robotPlayingAction = PlayingAction.None;
        public string DisplayMessage = "";
        public PlayingSide playingSide = PlayingSide.ScoringOnRight;

        //public double[,] heatMapStrategy;
        //public double[,] heatMapWaypoint;
        List<PointDExtended> lidarMap;
        List<PointDExtended> lidarProcessedMap;
        List<PointDExtended> strategyPtsList;
        List<PolarPointListExtended> lidarObjectList;
        List<SegmentExtended> lidarSegmentList;
        ConcurrentBag<PolygonExtended> voronoiPolygonList;
        public List<Location> ballLocationList;

        public RobotDisplay(PolygonExtended rbtShape, PolygonExtended ghstShape, string name)
        {
            robotLocation = new Location(0, 0, 0, 0, 0, 0);
            destinationLocation = new Location(0, 0, 0, 0, 0, 0);
            waypointLocation = new Location(0, 0, 0, 0, 0, 0);
            ghostLocation = new Location(0, 0, 0, 0, 0, 0);

            robotShape = rbtShape;
            ghostShape = ghstShape;
            robotName = name;

            lidarMap = new List<PointDExtended>();
            lidarProcessedMap = new List<PointDExtended>();
            strategyPtsList = new List<PointDExtended>();

            ballLocationList = new List<Location>();
            lidarSegmentList = new List<SegmentExtended>();
            voronoiPolygonList = new ConcurrentBag<PolygonExtended>();
        }

        public void SetLocation(Location loc)
        {
            robotLocation = loc;
        }

        public void SetPlayingAction(PlayingAction action)
        {
            robotPlayingAction = action;
        }
        public void SetDisplayMessage(string displayMessage)
        {
            DisplayMessage = displayMessage;
        }

        public void SetPlayingSide(PlayingSide playSide)
        {
            this.playingSide = playSide;
        }

        public void SetGhostLocation(Location loc)
        {
            ghostLocation = loc;
        }
        public void SetDestination(double x, double y, double theta)
        {
            destinationLocation.X = x;
            destinationLocation.Y = y;
            destinationLocation.Theta = theta;
        }
        public void SetWayPoint(double x, double y, double theta)
        {
            waypointLocation.X = x;
            waypointLocation.Y = y;
            waypointLocation.Theta = theta;
        }

        //public void SetHeatMapStrategy(double[,] heatMap)
        //{
        //    this.heatMapStrategy = heatMap;
        //}
        //public void SetHeatMapWaypoint(double[,] heatMap)
        //{
        //    this.heatMapWaypoint = heatMap;
        //}

        public void SetLidarMap(List<PointDExtended> lidarMap, LidarDataType type)
        {
            switch (type)
            {
                case LidarDataType.RawPtsList:
                    this.lidarMap = lidarMap;
                    break;
                case LidarDataType.ProcessedPtsList:
                    this.lidarProcessedMap = lidarMap;
                    break;                
            }
        }

        public void SetStrategyObjectList(List<PointDExtended> strategyPtsList)
        {
            this.strategyPtsList= strategyPtsList;
        }
        //public void SetLidarProcessedMap1(List<PointD> lidarProcessedMap)
        //{
        //    this.lidarProcessedMaps[0] = lidarProcessedMap;
        //}
        //public void SetLidarProcessedMap2(List<PointD> lidarProcessedMap)
        //{
        //    this.lidarProcessedMaps[1] = lidarProcessedMap;
        //}
        //public void SetLidarProcessedMap3(List<PointD> lidarProcessedMap)
        //{
        //    this.lidarProcessedMaps[2] = lidarProcessedMap;
        //}

        public void SetLidarSegmentList(List<SegmentExtended> lidarSegmentList)
        {
            this.lidarSegmentList = lidarSegmentList;
        }

        public void SetVoronoiPolygons(ConcurrentBag<PolygonExtended> voronoiPolygons)
        {
            this.voronoiPolygonList = voronoiPolygons;
        }

        public void SetLidarObjectList(List<PolarPointListExtended> lidarObjectList)
        {
            this.lidarObjectList = lidarObjectList;
        }

        public void SetStrategyPtsList(List<PointDExtended> strategyPtsList)
        {
            this.strategyPtsList = strategyPtsList;
        }

        public void SetBallList(List<Location> ballLocationList)
        {
            this.ballLocationList = ballLocationList;
        }

        public Location GetRobotLocation()
        {
            return robotLocation;
        }


        public PolygonExtended GetRobotPolygon()
        {
            PolygonExtended polygonToDisplay = new PolygonExtended();
            foreach (var pt in robotShape.polygon.Points)
            {
                PointD polyPt = new PointD(pt.X * Math.Cos(robotLocation.Theta) - pt.Y * Math.Sin(robotLocation.Theta), pt.X * Math.Sin(robotLocation.Theta) + pt.Y * Math.Cos(robotLocation.Theta));
                polyPt.X += robotLocation.X;
                polyPt.Y += robotLocation.Y;
                polygonToDisplay.polygon.Points.Add(polyPt);
                polygonToDisplay.backgroundColor = robotShape.backgroundColor;
                polygonToDisplay.borderColor = robotShape.borderColor;
                polygonToDisplay.borderWidth = robotShape.borderWidth;
            }
            return polygonToDisplay;
        }
        
        public PolygonExtended GetRobotGhostPolygon()
        {
            PolygonExtended polygonToDisplay = new PolygonExtended();
            foreach (var pt in ghostShape.polygon.Points)
            {
                PointD polyPt = new PointD(pt.X * Math.Cos(ghostLocation.Theta) - pt.Y * Math.Sin(ghostLocation.Theta), pt.X * Math.Sin(ghostLocation.Theta) + pt.Y * Math.Cos(ghostLocation.Theta));
                polyPt.X += ghostLocation.X;
                polyPt.Y += ghostLocation.Y;
                polygonToDisplay.polygon.Points.Add(polyPt);
                polygonToDisplay.backgroundColor = ghostShape.backgroundColor;
                polygonToDisplay.borderColor = ghostShape.borderColor;
                polygonToDisplay.borderWidth = ghostShape.borderWidth;
            }
            return polygonToDisplay;
        }

        public PolygonExtended GetRobotSpeedArrow()
        {
            PolygonExtended polygonToDisplay = new PolygonExtended();
            double angleTeteFleche = Math.PI / 6;
            double longueurTeteFleche = 0.30;
            double LongueurFleche = Math.Sqrt(robotLocation.Vx * robotLocation.Vx + robotLocation.Vy * robotLocation.Vy);
            double headingAngle = Math.Atan2(robotLocation.Vy, robotLocation.Vx) + robotLocation.Theta;
            double xTete = LongueurFleche * Math.Cos(headingAngle);
            double yTete = LongueurFleche * Math.Sin(headingAngle);

            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X, robotLocation.Y));
            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + xTete, robotLocation.Y + yTete));
            double angleTeteFleche1 = headingAngle + angleTeteFleche;
            double angleTeteFleche2 = headingAngle - angleTeteFleche;
            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + xTete - longueurTeteFleche * Math.Cos(angleTeteFleche1), robotLocation.Y + yTete - longueurTeteFleche * Math.Sin(angleTeteFleche1)));
            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + xTete, robotLocation.Y + yTete));
            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + xTete - longueurTeteFleche * Math.Cos(angleTeteFleche2), robotLocation.Y + yTete - longueurTeteFleche * Math.Sin(angleTeteFleche2)));
            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + xTete, robotLocation.Y + yTete));
            polygonToDisplay.borderWidth = 2;
            polygonToDisplay.borderColor = System.Drawing.Color.Blue;//.FromArgb(0xFF, 0xFF, 0x00, 0x00);
            //polygonToDisplay.borderDashPattern = new double[] { 3, 3 };
            polygonToDisplay.borderOpacity = 0.4;
            polygonToDisplay.backgroundColor = System.Drawing.Color.FromArgb(0x00, 0x00, 0x00, 0x00);
            return polygonToDisplay;
        }
        public PolygonExtended GetRobotDestinationArrow()
        {
            PolygonExtended polygonToDisplay = new PolygonExtended();
            double angleTeteFleche = Math.PI / 6;
            double longueurTeteFleche = 0.30;
            double headingAngle = Math.Atan2(destinationLocation.Y - robotLocation.Y, destinationLocation.X - robotLocation.X);

            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X, robotLocation.Y));
            polygonToDisplay.polygon.Points.Add(new PointD(destinationLocation.X, destinationLocation.Y));
            double angleTeteFleche1 = headingAngle + angleTeteFleche;
            double angleTeteFleche2 = headingAngle - angleTeteFleche;
            polygonToDisplay.polygon.Points.Add(new PointD(destinationLocation.X - longueurTeteFleche * Math.Cos(angleTeteFleche1), destinationLocation.Y - longueurTeteFleche * Math.Sin(angleTeteFleche1)));
            polygonToDisplay.polygon.Points.Add(new PointD(destinationLocation.X, destinationLocation.Y));
            polygonToDisplay.polygon.Points.Add(new PointD(destinationLocation.X - longueurTeteFleche * Math.Cos(angleTeteFleche2), destinationLocation.Y - longueurTeteFleche * Math.Sin(angleTeteFleche2)));
            polygonToDisplay.polygon.Points.Add(new PointD(destinationLocation.X, destinationLocation.Y));
            polygonToDisplay.borderWidth = 3;
            polygonToDisplay.borderColor = System.Drawing.Color.Orange;// FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
            //polygonToDisplay.borderDashPattern = new double[] { 5, 5 };
            polygonToDisplay.borderOpacity = 0.4;
            polygonToDisplay.backgroundColor = System.Drawing.Color.FromArgb(0x00, 0x00, 0x00, 0x00);
            return polygonToDisplay;
        }
        public PolygonExtended GetRobotWaypointArrow()
        {
            PolygonExtended polygonToDisplay = new PolygonExtended();
            double angleTeteFleche = Math.PI / 6;
            double longueurTeteFleche = 0.30;
            double headingAngle = Math.Atan2(waypointLocation.Y - robotLocation.Y, waypointLocation.X - robotLocation.X);

            polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X, robotLocation.Y));
            polygonToDisplay.polygon.Points.Add(new PointD(waypointLocation.X, waypointLocation.Y));
            double angleTeteFleche1 = headingAngle + angleTeteFleche;
            double angleTeteFleche2 = headingAngle - angleTeteFleche;
            polygonToDisplay.polygon.Points.Add(new PointD(waypointLocation.X - longueurTeteFleche * Math.Cos(angleTeteFleche1), waypointLocation.Y - longueurTeteFleche * Math.Sin(angleTeteFleche1)));
            polygonToDisplay.polygon.Points.Add(new PointD(waypointLocation.X, waypointLocation.Y));
            polygonToDisplay.polygon.Points.Add(new PointD(waypointLocation.X - longueurTeteFleche * Math.Cos(angleTeteFleche2), waypointLocation.Y - longueurTeteFleche * Math.Sin(angleTeteFleche2)));
            polygonToDisplay.polygon.Points.Add(new PointD(waypointLocation.X, waypointLocation.Y));

            polygonToDisplay.borderWidth = 3;
            polygonToDisplay.borderColor = System.Drawing.Color.Yellow;// FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
            //polygonToDisplay.borderDashPattern = new double[] { 5, 5 };
            polygonToDisplay.borderOpacity = 0.8;


            return polygonToDisplay;
        }

        //public XyDataSeries<double, double> GetRobotLidarPoints()
        //{
        //    var dataSeries = new XyDataSeries<double, double>();
        //    if (lidarMap == null)
        //        return dataSeries;


        //    //lock (lidarMap)
        //    {
        //        var listX = lidarMap.Select(e => e.X);
        //        var listY = lidarMap.Select(e => e.Y);

        //        if (listX.Count() == listY.Count())
        //        {
        //            dataSeries.AcceptsUnsortedData = true;
        //            dataSeries.Append(listX, listY);
        //        }
        //    }
        //    return dataSeries;
        //}

        public XyDataSeries<double, double> GetRobotLidarPoints(LidarDataType type)
        {
            var dataSeries = new XyDataSeries<double, double>();
            if (lidarMap == null)
                return dataSeries;

            IEnumerable<double> listX = new List<double>();
            IEnumerable<double> listY = new List<double>();

            switch (type)
            {
                case LidarDataType.RawPtsList:
                    listX = lidarMap.Select(e => e.Pt.X);
                    listY = lidarMap.Select(e => e.Pt.Y);
                    break;
                //case LidarDataType.ProcessedData1:
                //    listX = lidarProcessedMaps[0].Select(e => e.Pt.X);
                //    listY = lidarProcessedMaps[0].Select(e => e.Pt.Y);
                //    break;
                //case LidarDataType.ProcessedData2:
                //    listX = lidarProcessedMaps[1].Select(e => e.Pt.X);
                //    listY = lidarProcessedMaps[1].Select(e => e.Pt.Y);
                //    break;
                //case LidarDataType.ProcessedData3:
                //    listX = lidarProcessedMaps[2].Select(e => e.Pt.X);
                //    listY = lidarProcessedMaps[2].Select(e => e.Pt.Y);
                //    break;
            }            

            if (listX.Count() == listY.Count())
            {
                dataSeries.AcceptsUnsortedData = true;
                dataSeries.Append(listX, listY);
            }
            return dataSeries;
        }

        public List<SegmentExtended> GetRobotLidarSegments()
        {
            return this.lidarSegmentList;
        }

        public ConcurrentBag<PolygonExtended> GetVoronoiPolygons()
        {
            return this.voronoiPolygonList;
        }
        public List<PointDExtended> GetRobotLidarExtendedPoints()
        {
            return this.lidarProcessedMap;
        }
        public List<PointDExtended> GetRobotStrategyPoints()
        {
            return this.strategyPtsList;
        }

        public List<PolygonExtended> GetRobotLidarObjects()
        {
            var polygonExtendedList = new List<PolygonExtended>();
            if (this.lidarObjectList == null)
                return polygonExtendedList;

            foreach (var obj in this.lidarObjectList)
            {
                PolygonExtended polygonToDisplay = new PolygonExtended();
                foreach (var pt in obj.polarPointList)
                {
                    polygonToDisplay.polygon.Points.Add(new PointD(robotLocation.X + pt.Distance * Math.Cos(pt.Angle+robotLocation.Theta), robotLocation.Y + pt.Distance * Math.Sin(pt.Angle + robotLocation.Theta)));
                }
                //Cas des polygones à un seul point (objets représentés par leur centre : 
                //on trace un second point juste à coté
                if(obj.polarPointList.Count==1)
                {
                    PointD pt = polygonToDisplay.polygon.Points[0];
                    polygonToDisplay.polygon.Points.Add(new PointD(pt.X+0.001, pt.Y + 0.001));
                }
                switch(obj.type)
                {
                    case ObjectType.ObstacleLidar:
                        polygonToDisplay.borderColor = System.Drawing.Color.Red;
                        polygonToDisplay.borderWidth = 2;
                        polygonToDisplay.backgroundColor = System.Drawing.Color.Yellow;
                        break;
                    case ObjectType.Balise:
                        polygonToDisplay.borderColor = System.Drawing.Color.Blue;
                        polygonToDisplay.borderWidth = 2;
                        polygonToDisplay.backgroundColor = System.Drawing.Color.Magenta;
                        break;
                    case ObjectType.Balle:
                        polygonToDisplay.borderColor = System.Drawing.Color.Black;
                        polygonToDisplay.borderWidth = 2;
                        polygonToDisplay.backgroundColor = System.Drawing.Color.Yellow;
                        break;
                    default:
                        polygonToDisplay.borderColor = System.Drawing.Color.Black;
                        polygonToDisplay.borderWidth = 2;
                        polygonToDisplay.backgroundColor = System.Drawing.Color.LightBlue;
                        break;
                }
                
                polygonExtendedList.Add(polygonToDisplay);
            }
            return polygonExtendedList;
        }
    }
}
