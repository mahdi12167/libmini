// (c) by Stefan Roettger, licensed under LGPL 2.1

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
   {
   repository=path;

   if (repository.endswith("\\"))
      if (repository.size()>1)
         repository.shrinksize();
      else
         repository="/";

   if (repository.size()>0)
      if (!repository.endswith("/"))
         repository+="/";
   }

void Object::set_relative_name(ministring name)
   {
   filename=name;

   if (filename.startswith("file://"))
      filename=filename.suffix("file://");

   // check for relative path to truncate path
   if (repository.size()>0)
      if (filename.startswith(repository))
         filename=filename.suffix(repository);

   // check for absolute path to clear repository
   if (filename.startswith("/") ||
       filename.startswith("\\") ||
       filename.startswith("http://") ||
       filename.startswith("https://") ||
       filename.startswith("ftp://") ||
       filename.startswith("ftps://"))
      repository="";
   }

BOOLINT Object::has_repo_path()
   {return(repository.empty());}

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

void Object::show(BOOLINT) {}
BOOLINT Object::is_shown() const {return(TRUE);}
