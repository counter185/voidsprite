using System;
using System.Collections.Generic;
using System.Net;
using System.Security.Policy;
using System.Text;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace voidsprite_updater
{
    internal class Program
    {
        const string actionsURL = "https://api.github.com/repos/counter185/voidsprite/actions/artifacts?per_page=4";

        static int Main(string[] args)
        {
            try
            {
                ServicePointManager.SecurityProtocol = (SecurityProtocolType)3072;

                HttpWebRequest wrGETURL = (HttpWebRequest)WebRequest.Create(actionsURL);
                wrGETURL.Method = "GET";
                wrGETURL.UserAgent = "voidsprite-updater";

                string currentHash = File.ReadAllText("current-ver");

                WebResponse response = wrGETURL.GetResponse();
                if (response != null)
                {
                    //check response code
                    if (((HttpWebResponse)response).StatusCode == HttpStatusCode.OK)
                    {
                        //parse response
                        string responseString = new System.IO.StreamReader(response.GetResponseStream()).ReadToEnd();
                        JObject responseJson = (JObject)JsonConvert.DeserializeObject(responseString);
                        if (responseJson != null)
                        {
                            string sha = responseJson["artifacts"][0]["workflow_run"]["head_sha"].ToString();
                            int returnCode = sha == currentHash ? 0 : 1;
                            Console.WriteLine("Latest:" + sha + " -> " + returnCode);
                            return returnCode;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Error: " + ((HttpWebResponse)response).StatusCode);
                    }

                }
                return -1;
            } catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return -2;
            }
        }
    }
}
