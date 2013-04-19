using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;

namespace PixelStudioApplication.M
{
    class ProjectManager
    {
        private static ProjectManager _instance = new ProjectManager(); 

        public static ProjectManager getInstance()
        {
            return _instance;
        }

        public event PropChangeHandler eventHandler;
        private Project project;

        public Project newProject()
        {
            if (project != null)
            {
                project.close();
            }
            project = new Project();
            if (eventHandler != null)
            {
                eventHandler(this, TYPE.NEW, "project");
            }            
            return project;
        }

        public Project current
        {
            get
            {
                return project;
            }
        }

        public void saveProject(String fileName)
        {
            var s = new DataContractJsonSerializer(typeof(Project));
            using(FileStream file = new FileStream(fileName, FileMode.OpenOrCreate))            
            {
                s.WriteObject(file, current);
            }
            current.fileName = fileName;
        }


        public void openProject(string fileName)
        {
            Project obj;
            var s = new DataContractJsonSerializer(typeof(Project));
            using (FileStream file = new FileStream(fileName, FileMode.Open))
            {
                obj = (Project) s.ReadObject(file);
            }
            if(project!=null) {
                project.close();
            }
            project = obj;
            if (eventHandler != null)
            {
                eventHandler(this, TYPE.NEW, "project");
            }
        }
    }
}
