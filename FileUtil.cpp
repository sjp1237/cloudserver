#include"FileUtil.hpp"
#include<cstdint>    


size_t sjp::FileUtil::GetFileSize(){
    struct stat buf;
    if(stat(filename.c_str(),&buf)==-1){
      cout<<"GetFileSize fail.."<<endl;
    }
    return buf.st_size;
}

size_t sjp::FileUtil::GetFileAccessTime(){
    struct stat buf;
    if(stat(filename.c_str(),&buf)==-1){
      cout<<"GetFielAccessTime fail.."<<endl;
    }
    return buf.st_atime;
}

size_t sjp::FileUtil::GetFileModfityTime(){
    struct stat buf;
    if(stat(filename.c_str(),&buf)==-1){
      cout<<"GetFileSize fail.."<<endl;
    }
    return buf.st_mtime;
}

bool sjp::FileUtil::Exist(){
  struct stat buf;
  if(stat(filename.c_str(),&buf)==-1){
    return false; 
  }
  return true;
}

std::string sjp::FileUtil::GetFilename(){
  size_t pos=filename.rfind("/");
  if(pos==std::string::npos)
  {
    return filename;
  }
  return filename.substr(pos+1);
}

bool sjp::FileUtil::SetContent(std::string& body)
{
  std::ofstream ofs;
  ofs.open(filename,std::ios::binary);
  ofs.write(body.c_str(),body.size());
  if(ofs.good()==false)
  {
    cout<<"FileUtil.cpp 52 line:SetContent failing"<<endl;
    return false;
  }
  return true;
}

bool sjp::FileUtil::GetPosContent(size_t pos,size_t n,std::string& body){
  if(!Exist()){
    cout<<"GetPostContent: file is not exist"<<endl;
    return false;
  }

  if(GetFileSize()<pos+n){
    cout<<"GetFileSize fail:42"<<endl;
    return false;
  }
  std::ifstream ifs;
  ifs.open(filename,std::ios::binary);
  ifs.seekg(pos,std::ios::beg);
  body.resize(n);
  ifs.read(&body[0],n);
  if(ifs.good()==false){
    std::cout<<"get file content failed\n"<<endl;
    return false;
  }
  ifs.close();
  return true;
}

bool sjp::FileUtil::GetContent(std::string& body){
  size_t size=GetFileSize();
  return GetPosContent(0,size,body);
}

bool sjp::FileUtil::Remove(){
  remove(filename.c_str());
  return true;
}

bool sjp::FileUtil::GetPathInDir(std::vector<std::string>& arry)
{
  for(auto & p:fs::directory_iterator(filename)){
    arry.push_back(p.path().relative_path());
  }
  return true;
}

bool sjp::FileUtil::PackFile(const std::string& packname)
{
  std::ifstream ifs;
  ifs.open(filename.c_str(),std::ios::binary);
  ifs.seekg(0,ifs.end);
  int size=ifs.tellg();
  ifs.seekg(0,ifs.beg);

  std::string body;
  body.resize(size);
  ifs.read(&body[0],size);       
  std::string packed=bundle::pack(bundle::LZIP,body);                                            
  std::ofstream ofs;    
  ofs.open(packname.c_str());    
  ofs.write(&packed[0],packed.size());    
  ofs.close();    
  ifs.close();     
  return true;
}

bool sjp::FileUtil::UnpackFile(const std::string& unpackfile){
  std::ifstream ifs;    
    ifs.open(filename.c_str(),std::ios::binary);    
    ifs.seekg(0,ifs.end);    
    int size=ifs.tellg();    
    ifs.seekg(0,ifs.beg);

    std::string body;                                                    
    body.resize(size);                                              
    ifs.read(&body[0],size);                                        
    std::string unpacked=bundle::unpack(body);  

    std::ofstream ofs;
    ofs.open(unpackfile.c_str(),std::ios::binary);
    ofs.write(unpacked.c_str(),unpacked.size());
    ifs.close();
    ofs.close();
    return true;
}

