// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/miniio.h>

#include "object.h"

Object::Object(const ministring &name,const ministring &repo)
   {
   set_relative_path(repo);
   set_relative_name(name);

   coord=minicoord();
   radius=0.0;
   }

Object::~Object()
   {}

ministring Object::get_full_name() const
   {return(repository+filename);}

void Object::set_full_name(ministring name)
   {
   repository="";
   filename=name;
   }

ministring Object::get_relative_path() const
   {return(repository);}

ministring Object::get_relative_name() const
   {return(filename);}

void Object::set_relative_path(ministring path)
   {repository=normalize_path(path);}

void Object::set_relative_name(ministring name)
   {
   filename=normalize_file(name);

   // check for relative path to truncate path
   if (has_repo_path())
      if (filename.startswith(repository))
         filename=filename.suffix(repository);

   // check for absolute path to clear repository
   if (is_absolute_path(filename))
      repository="";
   }

BOOLINT Object::has_repo_path()
   {return(!repository.empty());}

BOOLINT Object::is_absolute_path(ministring path)
   {
   return(checkfilepath(path.c_str())!=0 ||
          path.startswith("http://") ||
          path.startswith("https://") ||
          path.startswith("ftp://") ||
          path.startswith("ftps://"));
   }

ministring Object::normalize_path(ministring path)
   {
   path.substitute("\\", "/");

   if (!path.empty())
      if (!path.endswith("/")) path += "/";

   return(path);
   }

ministring Object::normalize_file(ministring file)
   {
   if (file.startswith("file://"))
      file = file.suffix("file://");

   file.substitute("\\", "/");

   return(file);
   }

void Object::set_center(minicoord c,double r)
   {
   coord=c;
   coord.convert2ecef();
   radius=r;
   }

minicoord Object::get_center()
   {
   coord.convert2ecef();
   return(coord);
   }

miniv3d Object::get_normal()
   {
   coord.convert2ecef();
   miniv3d normal=coord.vec;
   normal.normalize();
   return(normal);
   }

double Object::get_radius()
   {return(radius);}

ministring Object::get_info()
   {
   return(ministring("Object")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename);
   }

void Object::show(BOOLINT /*yes*/) {}
BOOLINT Object::is_shown() const {return(TRUE);}

void Object::focus() {}
