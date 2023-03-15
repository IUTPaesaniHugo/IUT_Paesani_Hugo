using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RobotInterface
{

    public class PID
    {
        public float kp;
        public float ki;
        public float kd;
        public float promax;
        public float intmax;
        public float dermax;
        public float consigne;
        public float erreur;
        public float commande;
        public float corrP;
        public float corrI;
        public float corrD;

        public PID()
        {
        }
    }
}
