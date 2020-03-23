using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MySql.Data.MySqlClient;
using System.Collections;
namespace LightGraph
{
    class DBManager
    {
        private const String usernanme = "fbakly";
        private const String password = "F0u@d871999";
        private const String dbName = "Micro2Final";
        private const String serverName = "192.168.2.97";
        private const String port = "3306";
        private static MySqlConnection conn;

        public bool Connect()
        {
           
            string connectionString = "server=" + serverName + ";port=" + port + ";database=" + dbName + ";uid=" + usernanme + ";pwd=" + password + ";";
            conn = new MySqlConnection(connectionString);
            try
            {
                conn.Open();
            } catch (Exception ex)
            {
                Console.WriteLine(ex);
                return false;
            }
            return true;
        }

        public List<String> ExecuteQuery(String query)
        {
            List<String> payloads = new List<String>();
            try
            {
                var command = conn.CreateCommand();
                command.CommandType = System.Data.CommandType.Text;
                command.CommandText = query;

                var reader = command.ExecuteReader();

                if (reader.HasRows)
                {
                    while (reader.Read())
                    {
                        payloads.Add(reader.GetString(0) + "\t" + reader.GetString(1));
                    }
                    
                }
            } catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
            var count = payloads.Count;
            return payloads;
        }

        public void CloseConn()
        {
            try
            {
                conn.Close();
            } catch(Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

    }
}
